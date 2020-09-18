#include "serial_utils.h"


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
  if(src->data_size!=data_size)return;
  for(uint16_t i = 0; i<src->data_size; i++){
    *dest=src->data[i];
    dest++;
  }
}
