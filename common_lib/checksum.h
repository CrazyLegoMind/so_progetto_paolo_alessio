#ifndef CHECKSUM_H
#define CHECKSUM_H
#include <stdint.h>
#include <stddef.h>
//void checksum_calc(char* a, char* b, char* checksum);

uint8_t checksum_calc(void *buffer, size_t len, unsigned int seed);

unsigned int checksum_cmp(uint8_t* c1, uint8_t* c2);

#endif 
