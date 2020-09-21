#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <inttypes.h>
#include "../../common_lib/defs.h"
#include "../../common_lib/serial_utils.h"
#include "../libraries/serial.h"

//AA test ricezione e invio dati su host
// compile command =  gcc -Wall serial_test.c serial.c ../../common_lib/serial_utils.c -o serial_test
#define EOT 0x1


//global var
Data* data_received;


int main(int argc, char** argv) {

    printf("Test serial begin.\nOpening serial connection... \n");
    /*
    printf("printing sizes:\n packet %ld \n init %ld\n data %ld\n text %ld",
      sizeof(Data),sizeof(InitPkg),sizeof(DataPkg),sizeof(TextPkg));
    */
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
    
    data_received = malloc(sizeof(Data));
    InitPkg config_pkg;
    config_pkg.channels = 3;
    config_pkg.mode = 0;
    config_pkg.sampling_freq = 1;
    config_pkg.time = 10;
    config_pkg.trigger = 0;
    //for(int tr= 0; tr <10; tr++){
    serial_send_data(fd, (uint8_t*)&config_pkg, sizeof(InitPkg),TYPE_INITPKG);
    //}
    
    while(1) {
      int try = 1;
      while(serial_read(fd,(uint8_t*) data_received, sizeof(Data)) == -1){
        printf("try %d An error occurs while reading from server.\n",try++);
      }
      print_pkg(data_received);
      printf("\n\n\n");
    }
    return 0;
}
