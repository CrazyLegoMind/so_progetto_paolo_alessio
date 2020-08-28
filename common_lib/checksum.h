#ifndef CHECKSUM_H
#define CHECKSUM_H

//void checksum_calc(char* a, char* b, char* checksum);

uint32_t checksum_calc(void *buffer, size_t len, unsigned int seed);

unsigned int checksum_cmp(uint32_t* c1, uint32_t* c2);

#endif 