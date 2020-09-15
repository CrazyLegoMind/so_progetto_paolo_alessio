#include "serial_utils.h"

static char* select_header(size_t dim) {
    return (dim == 12 ? DATA_HEADER : INIT_HEADER);
}

static int serial_align_data(uint8_t* src, uint8_t* dest, size_t size, char* header) {
    uint8_t* tmp = malloc(size*2);
    memcpy(tmp, src, size);
    memcpy(tmp+size, src, size);
    int i,c;
    //char* header = select_header(size);
    for(i=0; i < size; i++) {
        //trova posizione dell'header
        c = memcmp(tmp+i, header, HEADER_SIZE);
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
