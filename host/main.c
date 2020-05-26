#include <stdio.h>
#include <stdlib.h>

int main (int* argc, char** argv) {

    uint8_t freq, mode = 2;
    printf("Welcome to oscilloscope project, powered by Alessio & Paolo\n");

    if(argc <= 2) {
        //insert list of possibility
        printf("Give your sampling frequency (in MHz): ");
        scanf("%hhu",&freq);
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", mode);
    } else if(argc == 3) {
        freq = argc[1];
        mode = argc[2];
    }

    //check error for freq

    while(mode < 0 || mode > 1) {
        printf("Unknown mode, try again\n");
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", mode);
    }

    //start
    return EXIT_SUCCESS;
}