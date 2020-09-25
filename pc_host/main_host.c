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

const char filenames[8][15] = 
{
    "samples_A0.txt",
    "samples_A1.txt",
    "samples_A2.txt",
    "samples_A3.txt",
    "samples_A4.txt",
    "samples_A5.txt",
    "samples_A6.txt",
    "samples_A7.txt" 
};


Data* pkg;
InitPkg config_pkg;
DataPkg data_pkg;
TextPkg text_pkg;

int main (int argc, char** argv) {

    InitPkg config_pkg;
    uint32_t freq = 0;
    uint8_t mode = 2, channels = 0, seconds = 0, trigger;
    pkg = (Data*) malloc(sizeof(Data));
    printf("Welcome to oscilloscope project, powered by Alessio & Paolo\n");

    //AA two insert options
    if(argc == 1) {
        //insert list of settings
        printf("Give your sampling frequency (in Hz): ");
        scanf("%d",&freq);
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
        printf("Insert mask pin value (from 0 to 255): ");
        scanf("%hhu", &channels);
        printf("How many seconds after self-stop: ");
        scanf("%hhu", &seconds);
    } 
    else if(argc == 6) {
      freq = (uint32_t)atoi(argv[1]);
      mode = (uint8_t)atoi(argv[2]);
      channels = (uint8_t)atoi(argv[3]);
      seconds = (uint8_t)atoi(argv[4]);
      trigger = (uint8_t)atoi(argv[5]);
      printf("starting with params:\n hz: %d \n mode: %d \n channels mask: %hhu \n seconds: %d \n trigger: %d\n",freq,mode,channels,seconds,trigger);
    }
    else{
      printf("only %d param, please put none or 5 params in fomat: \n <hz> <mode> <channels> <seconds> <trigger>\n", argc-1);
	  return EXIT_FAILURE;
    }
    
    //check error for mode
    while(mode < 0 || mode > 1) {
        printf("Unknown mode, try again.\n");
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
    }

    //if buffered mode is on, select the trigger
    if(mode && trigger == 0) {
        printf("You selected buffered mode, so do select trigger value: ");
        scanf("%hhu", &trigger);
    }


    //channel è una maschera a 8 bit va da 0 (nessun canale)
    //a 255 tutti i canali da A0 a A7, dove per esempio 8
    //vuoldire leggi il canale A3
    
    //check error for channels
    while(channels <= 0 || channels >= 256) {
        printf("Wrong number of channels, try again.\n");
        printf("Insert mask pin value (from 0 to 255): ");
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
    int res_set = serial_set(fd, 57600, 0);
    if(res_set == -1){
      printf("failed to set serial\n");
      return EXIT_FAILURE;
    }
    serial_set_blocking(fd, 1);
    sleep(1);
    //first of all send primary info to server (atmega)
    serial_send_data(fd, (uint8_t*)&config_pkg, sizeof(InitPkg), TYPE_INITPKG);


    //waiting for info from server, creating file(s)
    //AA: costruzione files per gnuplot (aggiustare script)
    for(int f=0; f < 8; f++) {
        if(channels & 1 << f) {
            FILE* file_fd = fopen(filenames[f], "w");
            if(!file_fd) {
                printf("Error while creating the file.\n");
                return EXIT_FAILURE;
            }
            fprintf(stdout,"Created file %s\n", filenames[f]);
            fprintf(file_fd, "#dati campionati\n#time(x)\treading(y)\n");
            //fflush(file_fd);
            fclose(file_fd);
        }
    }
    /*
    FILE* file_fd = fopen("samples.txt", "w");
    if(!file_fd) {
        printf("Error while creating the file.\n");
        return EXIT_FAILURE;
    }
    fprintf(file_fd, "#dati campionati\n#time(x)\treading(y)\n"); */
    
    printf("Entering while loop...");
    int epoch_current=0, epoch_max = 255;
    float time = 0.0;
    float step = 1.0 /((float)freq);
    if(!mode) epoch_max = freq*seconds;
    while(epoch_current < epoch_max) {
        //clear data at every iteration
        memset(pkg, 0, sizeof(Data));
        memset(&data_pkg, 0, sizeof(DataPkg));
        memset(&text_pkg, 0, sizeof(TextPkg));
    
    	while(serial_read(fd, (uint8_t*)pkg, sizeof(Data)) == -1);
        print_pkg(pkg);
        if(pkg->data_type == TYPE_DATAPKG) 
            serial_extract_data(pkg, (uint8_t*)&data_pkg, sizeof(DataPkg));
        else if(pkg->data_type == TYPE_TEXTPKG)
            serial_extract_data(pkg, (uint8_t*)&text_pkg, sizeof(TextPkg));
        printf("extract successful\n");
	    epoch_current = data_pkg.timestamp;
        /*
        //controllo checksum
        c = checksum_calc(&data_pkg, sizeof(data_pkg), 0);
        if(!checksum_cmp(c, &data_pkg.checksum))
            printf("Pacchetto scartato");
        else {
            //printf("Checksum OK\n"); */
        int p = (int)(data_pkg.mask_pin);
        FILE* pin_file = fopen(filenames[p],"w+");
	    time = (float)data_pkg.timestamp * step;
        int value = data_pkg.data;
        fprintf(pin_file, "%lf\t\t%d\n", time, value);
        //fprintf(file_fd, "%lf\t\t%d\n", time, value);
        fclose(pin_file);
        //}
    }
    
    //printf("END OF OPERATIONS.\n");
    //fclose(file_fd);
    return EXIT_SUCCESS;
}
