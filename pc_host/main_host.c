#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include "libraries/serial.h"
#include "../common_lib/defs.h"

//AA default baudrate
#define BAUD 115200
//AA default connection port (check using dmesg)
#define DEV_PATH "/dev/ttyACM0"


int main (int* argc, char** argv) {
	
    InitPkg config_pkg;
    uint8_t freq, mode = 2, channels = 0;
    int trigger = -1;
    printf("Welcome to oscilloscope project, powered by Alessio & Paolo\n");

    if(argc <= 3) {
        //insert list of settings
        printf("Give your sampling frequency (in Hz): ");
        scanf("%hhu",&freq);
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
        printf("How many channels do you want to use? (max 8) ");
        scanf("%hhu", &channels);
    } else if(argc == 4) {
        freq = argv[1];
        mode = argv[2];
        channels = argv[3];
    }
	
    //check error for frequency
    
    
    //check error for mode
    while(mode < 0 || mode > 1) {
        printf("Unknown mode, try again\n");
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
    }
    //if buffered mode is on, select the trigger
    if(mode) {
        printf("You selected buffered mode, so do select trigger value: ");
        scanf("%d", &trigger);
    }
    
    //check error for channels
    while(channels <= 0 || channels => 9) {
        printf("Wrong number of channels, try again\n");
        printf("How many channels do you want to use? (max 8): ");
        scanf("%hhu", &channels);
    }

    //start
    config_pkg.sampling_freq = freq;
    config_pkg.channels = channels;
    config_pkg.mode = mode;
    config_pkg.trigger = trigger;
    //setting up serial communication
    int fd = serial_open(DEV_PATH);
    if(fd < 0) 
    	return EXIT_FAILURE;
    if(uart_set(fd, BAUD, 'n') == -1)
    	return EXIT_FAILURE;
    //first of all send primary info to server (atmega)
    if(uart_write(fd, &config_pkg, sizeof(InitPkg)) == -1) {
    	exit(EXIT_FAILURE);
    	//return EXIT_FAILURE;
    }
    
    //waiting for info from server
    DataPkg data_pkg;
    while(/* non arriva un cmd che comunica che il trigger è stato attivato da server*/) {
    	if(uart_read(fd, &data_pkg, sizeof(DataPkg)) == -1)
    		return EXIT_FAILURE;
    	//save data (pin & value?) onto a file
    	FILE* file_fd = fopen("samples.plt", "w");
    	if(!file_fd) {
    		printf("Error while creating the file.\n");
    		return EXIT_FAILURE;
    	}
    	fprintf(file_fd, "%hhu ", data_pkg.mask_pin);
    	fprintf(file_fd, "%hhu\n", data_pkg.data);
    }
    //end of operations
    fclose(file_fd);
    return EXIT_SUCCESS;
}
