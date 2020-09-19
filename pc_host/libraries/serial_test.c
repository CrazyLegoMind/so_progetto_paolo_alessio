#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <inttypes.h>
#include "../../common_lib/defs.h"
#include "../libraries/serial.h"

//AA test ricezione e invio dati su host
// compile command =  gcc -Wall serial_test.c serial.c ../../common_lib/serial_utils.c -o serial_test
#define EOT 0x1


//global var
Data* data_received;



void print_pkg(DataPkg* pkg) {
    if(!pkg)    return;
    fprintf(stdout,"Package information:\nChecksum: " PRIu32 "\nSignal: " PRIu16 "\nPin: %hhu\nCommand: %hhu\nEpoch: %d\n\n", pkg->checksum, pkg->data, pkg->mask_pin, pkg->cmd, pkg->timestamp);
    return;
}

int main(int argc, char** argv) {

    printf("Test serial begin.\nOpening serial connection... \n");

    int fd = serial_open("/dev/ttyACM0");
    if(fd<0){
      printf("failed to open serial\n");
      return EXIT_FAILURE;
    }
    int res_set = serial_set(fd, 57600, 0);
    if(res_set == -1){
      printf("failed to set serial\n");
      return EXIT_FAILURE;
    }
    serial_set_blocking(fd,1);
    sleep(1);
    printf("Done.\n");

    /*
    //random values
    static uint8_t* freqs [AMOUNT_PKGS] = {500, 700, 1000, 250, 300};
    static uint8_t* channels[AMOUNT_PKGS] = {1, 2, 5, 7, 8};
    static uint8_t* times[AMOUNT_PKGS] = {4, 5, 8, 1, 10};
    //write test
    //try to send a bunch of init info
    printf("Creating packages... ");
    InitPkg** init_pkgs = (InitPkg**)malloc(sizeof(InitPkg*) * AMOUNT_PKGS);
    for(int i=0; i < AMOUNT_PKGS; ++i) {
        init_pkgs[i] = (InitPkg*)malloc(sizeof(InitPkg));
        init_pkgs[i]->sampling_freq = freqs[i];
        init_pkgs[i]->channels = channels[i];
        init_pkgs[i]->time = times[i];
        init_pkgs[i]->mode = 0;         //default: continuous sampling (change to 1 for buffered mode)
        init_pkgs[i]->trigger = -1;     //change this value for buffered mode
    }
    printf("Done.\n");

    //AA: several sends of packages let us know about uart behaviour
    printf("Sending packages to server... ");
    for(int i=0; i < AMOUNT_PKGS; i++) {
        InitPkg* p = init_pkgs[i];
        if(serial_write(fd, p, sizeof(InitPkg)) == -1) {
            fprintf(stderr,"An error occurs on package %d\n");
            return EXIT_FAILURE;
        }
        printf("Package %d sent to /dev/ACM0\n");
    }
    printf("Done.\n");
    //sleep(1);
    */

    
    data_received = malloc(sizeof(Data));
    InitPkg config_pkg;
    config_pkg.channels = 1;
    config_pkg.mode = 0;
    config_pkg.sampling_freq = 10;
    config_pkg.time = 2;
    config_pkg.trigger = 0;
    for(int tr= 0; tr <10; tr++){
      serial_send_data(fd, (uint8_t*)&config_pkg, sizeof(InitPkg),TYPE_INITPKG);
    }
    //serial_write(fd,"0000000000",10);
    //read test
    int num_data_pkgs = config_pkg.sampling_freq * config_pkg.time;
    DataPkg** data_pkgs = (DataPkg**)malloc(sizeof(DataPkg*) * num_data_pkgs);
    for(int i=0; i < num_data_pkgs; i++) data_pkgs[i] = (DataPkg*)malloc(sizeof(DataPkg));
    //waiting for info from server
    int counter = 0;
    uint8_t cmd = 0;

    while(counter <  num_data_pkgs && cmd != EOT) {
      printf("trying to read  %d\n",counter);
      int try = 1;
      while(serial_read(fd,(uint8_t*) data_received, sizeof(Data)) == -1){
	printf("try %d An error occurs while reading from server.\n",try++);
	
      }
      if(data_received->data_type == TYPE_DATAPKG){
	serial_extract_data(data_received,(uint8_t*)data_pkgs[counter],sizeof(DataPkg));
	print_pkg(data_pkgs[counter]);
	cmd = data_pkgs[counter]->cmd;
	counter++;
      }else if(data_received->data_type == TYPE_TEXTPKG){
	TextPkg* t = malloc(sizeof(TextPkg));
	serial_extract_data(data_received,(uint8_t*)t,sizeof(TextPkg));
	char* txt = t ->text;
	printf("[MSG] on with size %d:  ", t->text_size);
	for(int c = 0; c < t->text_size;c++) printf("%c", txt[c]);
	printf("\n");
      }else if(data_received->data_type == TYPE_INITPKG){
	printf("received echo packet");
      }

      /*
        if(serial_read(fd, data_pkgs[counter], sizeof(DataPkg)) == -1) {
	perror("An error occurs while reading from server.\n");
	break;
	//return EXIT_FAILURE;
	}
      */

    }

    /*
    for(int i=0; i < num_data_pkgs; i++)
        free(init_pkgs[i]);
    */
    for(int i=0; i < num_data_pkgs; i++)
        free(data_pkgs[i]);
    //free(init_pkgs);
    free(data_pkgs);
    free(data_received);
    return 0;
}
