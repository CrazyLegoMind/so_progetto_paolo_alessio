#include "serial_utils.h"
#include "defs.h"
#include <stdint.h>

int serial_align_data(uint8_t* src, uint8_t* dest, size_t size) {
    uint8_t* tmp = malloc(size*2);
    memcpy(tmp, src, size);
    memcpy(tmp+size, src, size);
    int i,c;
    for(i=0; i < size; i++) {
        //trova posizione dell'header
        c = memcmp(tmp+i, HEADER, HEADER_SIZE);
        if(!c) break;
    }

    if(i==size)
        return -1;
    i += HEADER_SIZE;
    for(int j=0; j < size - HEADER_SIZE; j++) { 
        i = i%size;
        *(dest+j) = *(src+i);
        i++;
    }
    free(tmp);
    return 1;
}


Data serial_wrap_data(uint8_t * data, uint32_t data_size, uint8_t data_type){
  Data d;
  memset(&d,0,sizeof(Data));
  d.data_size=data_size;
  d.data_type=data_type;

  uint16_t i;
  for(i = 0; i<data_size ; i++){
    d.data[i]=*data;
    data++;
  }
  for(; i<MAX_DATA;i++){
    d.data[i]=0;
  }

  return d;
}

void serial_extract_data(Data * src, uint8_t * dest, uint32_t data_size){
  if(src->data_size!=data_size){
    printf("extracr error: size mismatch %d != %d\n", src->data_size, data_size);
    return;
  }
  for(uint16_t i = 0; i<src->data_size; i++){
    *dest=src->data[i];
    dest++;
  }
}


void print_pkg(Data* d){
  if(d->data_type ==TYPE_DATAPKG){
    printf("printing DATAPKG\n");
    DataPkg* pkg = malloc(sizeof(DataPkg));
    serial_extract_data(d, (uint8_t*)pkg, sizeof(DataPkg));
    fprintf(stdout,"Data Package info:\nChecksum: %d\nSignal:%d\nPin: %hhu\nCommand: %hhu\nEpoch: %d\n\n",
	    pkg->checksum, pkg->data, pkg->mask_pin, pkg->cmd, pkg->timestamp);
 
  }else if(d->data_type == TYPE_INITPKG){
    printf("printing INITPKG\n");
    InitPkg* pkg = malloc(sizeof(InitPkg));
    serial_extract_data(d, (uint8_t*)pkg, sizeof(InitPkg));
    fprintf(stdout,"Init Package info:\nFrequency: %c\nChannels: %c\nMode: %c\nTime: %c\nTrigger: %d\n\n",
	    pkg->sampling_freq, pkg->channels, pkg->mode, pkg->time, pkg->trigger);
  }else if(d->data_type ==TYPE_TEXTPKG){
    printf("printing TEXTPKG\n");
    TextPkg* pkg = malloc(sizeof(TextPkg));
    serial_extract_data(d, (uint8_t*)pkg, sizeof(TextPkg));
    char* txt = pkg->text;
    fprintf(stdout,"Text Package info: \n text: ");

    if(txt){
      for(int c=0; c < pkg->text_size; c++){
	fprintf(stdout,"%c",txt[c]);
      }
    }
    fprintf(stdout,"\n");
  }else if(d->data_type ==TYPE_EMPTYPKG){
    fprintf(stdout,"Blank package.\n");
  }
  return;
}
