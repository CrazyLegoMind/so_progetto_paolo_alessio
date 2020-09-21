#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "libraries/serial.h"
#include "../common_lib/defs.h"
//#include "../common_lib/checksum.h"
#include "../common_lib/serial_utils.h"

//AA default baudrate
#define BAUD 57600
//AA default connection port (check using dmesg)
#define DEV_PATH "/dev/ttyACM0"


Data pkg;
InitPkg config_pkg;
DataPkg data_pkg;
TextPkg text_pkg;

int main (int argc, char** argv) {
	
    InitPkg config_pkg;
    uint8_t freq, mode = 2, channels = 0, seconds = 0, trigger;
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
        freq = (uint8_t)argv[1];
        mode = (uint8_t)argv[2];
        channels = (uint8_t)argv[3];
        seconds = (uint8_t)argv[4];
    }
	
    //check error for frequency
    
    
    //check error for mode
    while(mode < 0 || mode > 1) {
        printf("Unknown mode, try again.\n");
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
    }

    //if buffered mode is on, select the trigger
    if(mode) {
        printf("You selected buffered mode, so do select trigger value: ");
        scanf("%hhu", &trigger);
    }
    
    //check error for channels
    while(channels <= 0 || channels >= 9) {
        printf("Wrong number of channels, try again.\n");
        printf("How many channels do you want to use? (max 8): ");
        scanf("%hhu", &channels);
    }
    
    //check error for seconds
    while(seconds <= 0) {
        printf("Wrong time value, try again.\nHow many seconds after self-stop: ");
        scanf("%hhu", &seconds);
    }


    //start
    memset(&config_pkg, 0, sizeof(InitPkg));
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
    serial_set_blocking(fd, 1);
    sleep(1);
    //first of all send primary info to server (atmega)
    serial_send_data(fd, (uint8_t*)&config_pkg, sizeof(InitPkg), TYPE_INITPKG);

    //waiting for info from server
    FILE* file_fd = fopen("samples.txt", "w");
    if(!file_fd) {
        printf("Error while creating the file.\n");
        return EXIT_FAILURE;
    }
    /* AA potrebbero servire
    pkg = malloc(sizeof(Data));
    data_pkg = malloc(sizeof(DataPkg));
    text_pkg = malloc(sizeof(TextPkg));
    */
    //uint8_t* c;
    char* x_label = "times", y_label = "values";
    printf("Entering while loop...");
    while(1) {
        //clear data at every iteration
        memset(&pkg, 0, sizeof(Data));
        memset(&data_pkg, 0, sizeof(DataPkg));
        memset(&text_pkg, 0, sizeof(TextPkg));

    	while(serial_read(fd, (uint8_t*)&pkg, sizeof(Data)) == -1);
        print_pkg(&pkg);
        if(pkg.data_type == TYPE_DATAPKG) 
            serial_extract_data(&pkg, (uint8_t*)&data_pkg, sizeof(DataPkg));
        else if(pkg.data_type == TYPE_TEXTPKG)
            serial_extract_data(&pkg, (uint8_t*)&text_pkg, sizeof(TextPkg));
        
        /*
        //controllo checksum
        c = checksum_calc(&data_pkg, sizeof(data_pkg), 0);
        if(!checksum_cmp(c, &data_pkg.checksum))
            printf("Pacchetto scartato");
        else {
            //printf("Checksum OK\n"); */
            //AA: costruzione file per gnuplot
            fprintf(file_fd, "#dati campionati\n#%s(x)\t%s(y)\n", x_label, y_label);
            double time = data_pkg.timestamp * (1/freq);
            int value = (data_pkg.data == 0 ? 0 : 5);
            fprintf(file_fd, "%lf\t\t%d\n", time, value);
        //}
    }

    //printf("END OF OPERATIONS.\n");
    fclose(file_fd);
    return EXIT_SUCCESS;
}
