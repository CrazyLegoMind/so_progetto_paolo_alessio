#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "libraries/serial.h"
#include "../common_lib/defs.h"
#include "../common_lib/checksum.h"

//AA default baudrate
#define BAUD 115200
//AA default connection port (check using dmesg)
#define DEV_PATH "/dev/ttyACM0"


int main (int* argc, char** argv) {
	
    InitPkg config_pkg;
    uint8_t freq, mode = 2, channels = 0, seconds = -1;
    int trigger = -1;
    printf("Welcome to oscilloscope project, powered by Alessio & Paolo\n");

    if(argc <= 4) {
        //insert list of settings
        printf("Give your sampling frequency (in Hz): ");
        scanf("%hhu",&freq);
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
        printf("How many channels do you want to use? (max 8) ");
        scanf("%hhu", &channels);
        printf("How many seconds after self-stop: ");
        scanf("%hhu", &seconds);
    } else if(argc == 5) {
        freq = argv[1];
        mode = argv[2];
        channels = argv[3];
        seconds = argv[4];
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
    
    //check error for seconds
    while(seconds <= 0) {
        printf("Wrong time value, try again\nHow many seconds after self-stop: ");
        scanf("%hhu", &seconds);
    }


    //start
    config_pkg.sampling_freq = freq;
    config_pkg.channels = channels;
    config_pkg.mode = mode;
    config_pkg.time = seconds;
    config_pkg.trigger = trigger;
    //setting up serial communication
    int fd = serial_open(DEV_PATH);
    if(fd < 0) 
    	return EXIT_FAILURE;
    if(serial_set(fd, BAUD, 'n') == -1)
    	return EXIT_FAILURE;
    //first of all send primary info to server (atmega)
    if(serial_write(fd, &config_pkg, sizeof(InitPkg)) == -1)
    	//exit(EXIT_FAILURE);
    	return EXIT_FAILURE;
    sleep(1);

    //waiting for info from server
    DataPkg data_pkg;
    uint8_t loc_cmd = 0;
    FILE* file_fd = fopen("samples.txt", "w");
    while(loc_cmd == 0) {
        //clear data at every iteration
        memset(&data_pkg, 0, sizeof(DataPkg));
    	if(serial_read(fd, &data_pkg, sizeof(DataPkg)) == -1)
    		return EXIT_FAILURE;
        //controllo checksum (primi 32 bit del pacchetto)
        if(!checksum_cmp(checksum_calc(&data_pkg, sizeof(data_pkg)), &data_pkg.checksum))
            printf("Pacchetto scartato");
        else {
            //printf("Checksum OK\n");
            //save data (pin & value?) onto a file
            if(!file_fd) {
                printf("Error while creating the file.\n");
                return EXIT_FAILURE;
            }
            fprintf(file_fd, "%hhu ", data_pkg.mask_pin);
            fprintf(file_fd, "%hhu\n", data_pkg.data);
        }
        loc_cmd = data_pkg.cmd;
    }

    //printf("END OF OPERATIONS.\n");
    fclose(file_fd);
    return EXIT_SUCCESS;
}
