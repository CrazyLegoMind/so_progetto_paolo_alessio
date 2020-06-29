#include <stdio.h>
#include <stdlib.h>
#include "../server_atmega/avr_common/uart.h"
#include "init_pkg.h"

#define DFLT_BAUD 155200

typedef struct InitPkg InitPkg;

int main (int* argc, char** argv) {
	
    InitPkg pkg;
    uint8_t freq, mode = 2, channels = 0;
    printf("Welcome to oscilloscope project, powered by Alessio & Paolo\n");

    if(argc <= 4) {
        //insert list of settings
        printf("Give your sampling frequency (in Hz): ");
        scanf("%hhu",&freq);
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
    } else if(argc == 5) {
        freq = argv[1];
        mode = argv[2];
        channels = argv[3];
        baud = argv[4];
    }
	
    //check error for frequency
    
    
    //check error for mode
    while(mode < 0 || mode > 1) {
        printf("Unknown mode, try again\n");
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
    }
    
    //check error for channels
    while(channels <= 0 || channels => 9) {
        printf("Too many channels, try again\n");
        printf("How many channels do you want to use? (max 8): ");
        scanf("%hhu", &channels);
    }

    //start
    pkg.sampling_freq = freq;
    pkg.mode = mode;
    pkg.channels = channels;
    
    //send to server
    usart_init(DFLT_BAUD);
    usart_putchar(pkg.sampling_freq);
    usart_putchar(pkg.mode);
    usart_putchar(pkg.channels);
    
    //waiting for info from server
    
    return EXIT_SUCCESS;
}
