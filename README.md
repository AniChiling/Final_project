As you can see, the code was written in google colab, due to my laptop not being able to handle linux. Here's the link to it, and short explanation for commands

!wget https://raw.githubusercontent.com/nothings/stb/master/stb_image.h 
Downloads the lightweight stb_image.h library for image processing

!gcc analyzer.c -o analyzer -lm
!gcc classifier.c -o classifier
Compiles the programs (-lm links math library for analyzer)

!./analyzer petri_dish.png
Processes an image of Petri dish and saves zone diameters to zones.txt

!./classifier
Starts interactive mode to read measurements from zones.txt prompt for antibiotic names and classify resistance
