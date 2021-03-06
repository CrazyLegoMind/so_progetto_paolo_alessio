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

#define MAX_CHANNELS 8

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
uint8_t channel_list[MAX_CHANNELS] = {8,8,8,8,8,8,8,8};
uint8_t channel_count = 0;
  
uint8_t scan_channels(){
  char ch;
  uint8_t mask = 0;
  int ch_num = 0;
  channel_count = 0;
  printf("\n\ninsert one channel at time (0-7) or channels in a single string\ni.e. 245q for channel A2 + A4 +A5\n-- 'a' for all channels\n-- 'q' to end channel list \n-- if quit with no input default pin is A0:\n");
  scanf(" %c", &ch);
  while(ch != 'q'){
    if(ch == 'a'){
      channel_count = 8;
      mask = 255;
      break;
    }
    ch_num = atoi(&ch);
    if (ch_num < 8 && ch_num >= 0){
      uint8_t ch_mask = 1 << ch_num;
      if( mask & ch_mask ){
	      printf("channel already in the list, skipping...\n");
      }else{
        printf("added A%d\n",ch_num);
        mask |= ch_mask;
        channel_count++;
      }
    }
    else{
      printf("invalid channel, insert again:\n ");
    }
    scanf(" %c", &ch);
  }
  
  if(ch == 'q' && channel_count == 0){
    printf("no input: default to read only from A0\n");
    channel_count = 1;
    mask = 1;
  }
  printf("selected %d channels with mask %hhu\n",channel_count,mask);
  return mask;
}



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
        channels = scan_channels();
        if(mode != 1){
          printf("How many seconds after self-stop: ");
          scanf("%hhu", &seconds);
        }
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
      printf("only %d param, please put none or 5 params in fomat: \n <hz> <mode> <channel_mask> <seconds> <trigger>\n", argc-1);
	    return EXIT_FAILURE;
    }
    
    //check error for mode
    while(mode < 0 || mode > 1) {
        printf("Unknown mode, try again.\n");
        printf("Which mode you want to operate? (0 for continuous sampling, 1 for buffered mode): ");
        scanf("%hhu", &mode);
    }

    //if buffered mode is on, select the trigger, if not given by argv
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
    
    //check error for seconds, valid only in continuous mode
    while(!mode && seconds <= 0) {
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
    int res_set = serial_set(fd, BAUD, 0);
    if(res_set == -1){
      printf("failed to set serial\n");
      return EXIT_FAILURE;
    }
    serial_set_blocking(fd, 1);
    sleep(1);
    //waiting for info from server, creating file(s)
    //AA: costruzione files per gnuplot (aggiustare script)
    int init_mask = channel_count ? 0 : 1;
    for(int f=0; f < 8; f++) {
      if(channels & 1 << f) {
	if(init_mask)channel_count++;
        FILE* file_fd = fopen(filenames[f], "w");
        if(!file_fd) {
          fprintf(stderr,"Error while creating the A%d pin file.\n",f);
          return EXIT_FAILURE;
        }
        fprintf(stdout,"Created file %s\n", filenames[f]);
        fprintf(file_fd, "#dati campionati\n#time(x)\treading(y)\n");
        //fflush(file_fd);
        fclose(file_fd);
      }
    }
    //first of all send primary info to server (atmega)
    serial_send_data(fd, (uint8_t*)&config_pkg, sizeof(InitPkg), TYPE_INITPKG);



    /*
    FILE* file_fd = fopen("samples.txt", "w");
    if(!file_fd) {
        printf("Error while creating the file.\n");
        return EXIT_FAILURE;
    }
    fprintf(file_fd, "#dati campionati\n#time(x)\treading(y)\n"); */
    
    //printf("Entering while loop...");
    int epoch_current=0,packet_count=0,packet_end_current =0,epoch_max=0, packet_max = 0; //epoch max, packet epoch i need to read when i read last packet of a channel
    float time = 0.0;
    float step = 1.0 /((float)freq);

    if(!mode){
      epoch_max = (freq*seconds)-1;
      packet_max = freq*seconds*channel_count;
    }else{
      epoch_max = 255;
      packet_max = 256*channel_count;
    }
    while(packet_end_current < channel_count) {
      if(packet_count >= packet_max){
	printf("lost some final packet: exiting anyway\n");
	break;
      }
      //clear data at every iteration
      memset(pkg, 0, sizeof(Data));
      memset(&data_pkg, 0, sizeof(DataPkg));
      memset(&text_pkg, 0, sizeof(TextPkg));
    
      while(serial_read(fd, (uint8_t*)pkg, sizeof(Data)) == -1);
      print_pkg(pkg);
      if(pkg->data_type == TYPE_TEXTPKG){
	      serial_extract_data(pkg, (uint8_t*)&text_pkg, sizeof(TextPkg));
      }else if(pkg->data_type == TYPE_DATAPKG){  
        serial_extract_data(pkg, (uint8_t*)&data_pkg, sizeof(DataPkg));
        printf("extract successful\n");
	packet_count++;
        epoch_current = data_pkg.timestamp;
	if(epoch_current == epoch_max) packet_end_current++;
	
        int p = (int)(data_pkg.mask_pin);
        FILE* pin_file = fopen(filenames[p],"a");
        time = (float)data_pkg.timestamp * step;
        int value = data_pkg.data;
        fprintf(pin_file, "%lf\t\t%d\n", time, value);
        fclose(pin_file);
      }
      
    }
    //printf("packet arrived: %f (percentage)\n", (packet_count * 100)/packet_max);
    //printf("END OF OPERATIONS.\n");
    //fclose(file_fd);
    free(pkg);
    return EXIT_SUCCESS;
}
