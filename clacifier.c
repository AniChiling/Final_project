%%writefile classifier.c
#include <stdio.h>
#include <string.h>
#define MAX_ANTIBIOTICS 50  // max antibiotics we can store
#define MAX_TESTS 20        // max test results we can handle

// Stores antibiotic breakpoint data
typedef struct {
    char name[30];                      // antibiotic name
    float susceptible_breakpoint;       // min diameter for "Susceptible"
    float resistant_breakpoint;         // max diameter for "Resistant"
} antibioticinfo;

// Stores test results for an antibiotic
typedef struct {
    antibioticinfo info;                // antibiotic details
    float measured_diameter;            // zone size we measured
    char result[20];                    // Susceptible/Resistant/Intermediate
} antibiotictest;

// Determines if bacteria is Susceptible/Resistant/Intermediate
void interpret_result(antibiotictest *ab) {
    if (ab->measured_diameter >= ab->info.susceptible_breakpoint) {
        strcpy(ab->result, "Susceptible");      // big zone = good
    } else if (ab->measured_diameter <= ab->info.resistant_breakpoint) {
        strcpy(ab->result, "Resistant");        // small zone = bad
    } else {
        strcpy(ab->result, "Intermediate");    // in-between = meh
    }
}

// Finds antibiotic in our database by name
antibioticinfo* find_antibiotic_info(const char* name, antibioticinfo* table, int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(name, table[i].name) == 0) {  // compare names
            return &table[i];                    // return if match found
        }
    }
    return NULL;  // not found
}

int main() {
    // Our database of antibiotics and their breakpoints
    antibioticinfo antibioticTable[MAX_ANTIBIOTICS] = {
        {"Ampicillin", 17, 13},
        {"Ciprofloxacin", 25, 22},
        {"Gentamicin", 15, 12},
        {"Tetracycline", 19, 14},
        {"Cefotaxime", 26, 22},
        {"Meropenem", 25, 22},
        {"Trimethoprim-Sulfamethoxazole", 16, 10}
    };

    // Open the file with zone diameters
    FILE *file = fopen("zones.txt", "r");
    if (!file) {
        printf("Error: Could not open zones.txt\n");
        return 1;
    }

    antibiotictest tests[MAX_TESTS];
    int i = 0;
    // Read each zone diameter from file
    while (fscanf(file, "%f", &tests[i].measured_diameter) == 1 && i < MAX_TESTS) {
        printf("Enter antibiotic name for zone #%d (%.2f mm): ", i + 1, tests[i].measured_diameter);
        char inputName[50];
        scanf(" %[^\n]", inputName);  // read antibiotic name

        // Look up antibiotic in our database
        antibioticinfo *info = find_antibiotic_info(inputName, antibioticTable, MAX_ANTIBIOTICS);
        if (!info) {
            printf("Antibiotic '%s' not found in database, skipping.\n", inputName);
            tests[i].info.name[0] = '\0';  // mark as empty
            i++;
            continue;
        }

        tests[i].info = *info;            // store antibiotic info
        interpret_result(&tests[i]);      // determine result
        i++;
    }

    fclose(file);

    // Print all results
    printf("\n=== Final Results ===\n");
    for (int j = 0; j < i; j++) {
        if (tests[j].info.name[0] == '\0') {
            printf("Zone #%d: Skipped\n", j + 1);  // skipped tests
        } else {
            printf("%-30s : %s (Zone = %.1f mm)\n", tests[j].info.name, tests[j].result, tests[j].measured_diameter);
        }
    }

    return 0;
}