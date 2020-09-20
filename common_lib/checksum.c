#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

//AA: funzione di generazione e confronto del checksum

// In this method a checksum is calculated based on the given binary strings which is sent with the data as redundant bits. 
// This data + checksum is received at receiver end and checksum is calculated again,
// if any error occurs, the function returns 0, the calculated value otherwise.

#define MAX_BYTES sizeof(uint8_t)

char ext_mem[MAX_BYTES+1];


//AA: LRC (longitudinal redundancy check)
uint32_t checksum_calc(void *buffer, size_t len) {
    uint8_t seed = 0;
    unsigned char *buf = (unsigned char *)buffer;
    int i;
    for (i = 0; i < len; ++i)
        seed += (unsigned int)(*buf++);
    return seed;
}

//AA: controllo bit a bit
//1: successo, 0: errore
int checksum_cmp(uint8_t* c1, uint8_t* c2) {
    if(strlen(c1) != strlen(c2))
        return 0;
    while(c1 != NULL || c2 != NULL) {
        if(*c1++ != *c2++)
            return 0;
    }
    return 1;
} 
