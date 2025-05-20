%%writefile analyzer.c
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"  //  needed for images

// Holds image stuff (width, height, etc.)
typedef struct {
    unsigned char *data;  // pixel data
    int width, height, channels;  // dimensions and color stuff
} Image;

// Stores disk info (position + size)
typedef struct {
    int x, y;  // position in the image
    float diameter_pixels;  // size in pixels
    float diameter_mm;  // size in real mm (converted)
} Disk;

// Measures how wide the disk is at a point
float measure_diameter(Image *img, int cx, int cy, int threshold) {
    int left = cx, right = cx;  // start at center
    // Move left until we hit the edge or bright pixel
    while (left > 0 && img->data[cy * img->width + left] < threshold) left--;
    // Same but right
    while (right < img->width && img->data[cy * img->width + right] < threshold) right++;
    return (float)(right - left);  // total width
}

// Converts pixels to millimeters (math stuff)
float pixel_to_mm(float pixels, float dish_pixel_diameter, float dish_mm_diameter) {
    return (pixels / dish_pixel_diameter) * dish_mm_diameter;  // ratio thing
}

// Checks if a new disk is too close to existing ones
int is_far_enough(Disk *disks, int count, int x, int y, int min_dist) {
    for (int i = 0; i < count; i++) {
        int dx = disks[i].x - x;  // x distance
        int dy = disks[i].y - y;  // y distance
        // Pythagorean theorem (kinda)
        if (dx*dx + dy*dy < min_dist * min_dist) {
            return 0;  // too close, skip it
        }
    }
    return 1;  // far enough, good to go
}

// Finds all the disks in the image
int find_disks(Image *img, Disk *disks, int max) {
    int count = 0;
    int min_dist = 200;  // min distance between disks (so they don't overlap)

    // Scan through the image (but skip some pixels for speed)
    for (int y = 10; y < img->height - 10; y += 5) {
        for (int x = 10; x < img->width - 10; x += 5) {
            unsigned char pixel_val = img->data[y * img->width + x];  // brightness at (x,y)

            // If it's dark enough, might be a disk
            if (pixel_val < 20) {  
                // Check if it's not too close to other disks
                if (is_far_enough(disks, count, x, y, min_dist)) {
                    float diameter_px = measure_diameter(img, x, y, 20);  // measure width
                    // Ignore tiny dots (must be big enough)
                    if (diameter_px > 20) {  
                        // Save disk info
                        disks[count].x = x;
                        disks[count].y = y;
                        disks[count].diameter_pixels = diameter_px;
                        disks[count].diameter_mm = pixel_to_mm(diameter_px, 540.0, 90.0);  // convert to mm
                        printf("Found disk %d at (%d, %d): %.1f px → %.1f mm\n", count + 1, x, y, diameter_px, disks[count].diameter_mm);
                        count++;
                        if (count >= max) return count;  // stop if too many disks
                        x += 20;  // skip ahead to avoid duplicates
                    }
                }
            }
        }
    }
    return count;  // total disks found
}

// Main function 
int main(int argc, char **argv) {
    if (argc < 2) {  // check if filename was given
        printf("Usage: ./analyzer image.png\n");  // yell at user if not
        return 1;
    }

    Image img;
    // Load the image (black & white)
    img.data = stbi_load(argv[1], &img.width, &img.height, &img.channels, 1);
    if (!img.data) {
        printf("Failed to load image.\n");  // oops
        return 1;
    }

    Disk disks[10];  // max 10 disks (should be enough)
    int count = find_disks(&img, disks, 10);  // find them!

    // Save results to a file
    FILE *f = fopen("zones.txt", "w");
    if (!f) {
        printf("Could not write zones.txt\n");  // file error
        stbi_image_free(img.data);
        return 1;
    }

    // Write each disk's diameter to file
    for (int i = 0; i < count; i++) {
        fprintf(f, "%.2f\n", disks[i].diameter_mm);
    }

    fclose(f);
    stbi_image_free(img.data);  // free memory (good practice?)

    printf("Saved %d diameters to zones.txt\n", count);
    return 0;
}