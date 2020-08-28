#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <inttypes.h>
#include "../../common_lib/defs.h"
#include "serial.h"

//AA test ricezione e invio dati su host

#define AMOUNT_PKGS 5
//#define PIN 1
#define EOT 0x1


void print_pkg(DataPkg* pkg) {
    if(!pkg)    return;
    fprintf(stdout,"Package information:\nChecksum: " PRIu32 "\nSignal: " PRIu16 "\nPin: %hhu\nCommand: %hhu\nEpoch: %d\n\n", pkg->checksum, pkg->data, pkg->mask_pin, pkg->cmd, pkg->timestamp);
    return;
}

int main(int argc, char** argv) {

    printf("Test serial begin.\nOpening serial connection... ");

    int fd = serial_open("/dev/ACM0");
    if(fd<0) return EXIT_FAILURE;
    int res_set = serial_set(fd, 115200, 'N');
    if(res_set == -1) return EXIT_FAILURE;
    printf("Done.\n");

    //random values
    static int* freqs [AMOUNT_PKGS] = {500, 700, 1000, 250, 300};
    static int* channels[AMOUNT_PKGS] = {1, 2, 5, 7, 8};
    static int* times[AMOUNT_PKGS] = {4, 5, 8, 1, 10};
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
            fprintf(stderr,"An error occurs on package %d\n", i);
            return EXIT_FAILURE;
        }
        printf("Package %d sent to /dev/ACM0\n",i);
    }
    printf("Done.\n");
    //sleep(1);

    //read test
    int num_data_pkgs = AMOUNT_PKGS*2;
    DataPkg** data_pkgs = (DataPkg**)malloc(sizeof(DataPkg*) * num_data_pkgs);
    for(int i=0; i < num_data_pkgs; i++) data_pkgs[i] = (DataPkg*)malloc(sizeof(DataPkg));
    //waiting for info from server
    int counter = 0;
    uint8_t cmd = 0;
    while(counter <  num_data_pkgs && cmd != EOT) {
        if(serial_read(fd, data_pkgs[counter], sizeof(DataPkg)) == -1) {
            perror("An error occurs while reading from server.\n");
            break;
            //return EXIT_FAILURE;
        }
        print_pkg(data_pkgs[counter]);
        cmd = data_pkgs[counter]->cmd;
    }

    for(int i=0; i < AMOUNT_PKGS; i++)
        free(init_pkgs[i]);
    for(int i=0; i < num_data_pkgs; i++)
        free(data_pkgs[i]);
    free(init_pkgs);
    free(data_pkgs);
    return 0;
}

//x compilare: gcc -Wall serial_test.c serial.c -o serial_test