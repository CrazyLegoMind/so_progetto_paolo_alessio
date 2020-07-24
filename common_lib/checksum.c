#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

//AA: funzione di generazione e confronto del checksum

// In this method a checksum is calculated based on the given binary strings which is sent with the data as redundant bits. 
// This data + checksum is received at receiver end and checksum is calculated again,
// if any error occurs, the function returns 0, the calculated value otherwise.

#define MAX_BYTES sizeof(uint32_t)

char ext_mem[MAX_BYTES+1];

/*
void checksum_calc(char* a, char* b, char* checksum) {
    if(strlen(a) != strlen(b)) {
		printf("\nWrong input strings\n");
        return;
	}
    
    int length = strlen(a);
    char carry='0';
    char sum[MAX_BYTES], complement[MAX_BYTES];
    int i;
    for(i=length-1;i>=0;i--) {
        if(a[i]=='0' && b[i]=='0' && carry=='0') {
            sum[i]='0';
            carry='0';
        }
        else if(a[i]=='0' && b[i]=='0' && carry=='1') {
            sum[i]='1';
            carry='0';

        }
        else if(a[i]=='0' && b[i]=='1' && carry=='0') {
            sum[i]='1';
            carry='0';

        }
        else if(a[i]=='0' && b[i]=='1' && carry=='1') {
            sum[i]='0';
            carry='1';

        }
        else if(a[i]=='1' && b[i]=='0' && carry=='0') {
            sum[i]='1';
            carry='0';

        }
        else if(a[i]=='1' && b[i]=='0' && carry=='1') {
            sum[i]='0';
            carry='1';

        }
        else if(a[i]=='1' && b[i]=='1' && carry=='0') {
            sum[i]='0';
            carry='1';

        }
        else if(a[i]=='1' && b[i]=='1' && carry=='1') {
            sum[i]='1';
            carry='1';

        }
        else
            break;
    }
    
    //printf("\nSum=%c%s",carry,sum);
    
    for(i=0;i<length;i++) {
        if(sum[i]=='0')
            complement[i]='1';
        else
            complement[i]='0';
    }
    
    if(carry=='1')
        carry='0';
    else
        carry='1';
    
    //printf("\nChecksum=%c%s\n",carry,complement);
    checksum[0] = carry;
    memcpy(&checksum[1], complement, MAX_BYTES);
}
*/


/*AA debug main

int main() {
    char a[MAX_BYTES],b[MAX_BYTES];
    char checksum[MAX_BYTES+1];
    printf("Enter first binary string (max %d chars): ", MAX_BYTES-1);
    scanf("%s",&a[0]);
    a[MAX_BYTES] = '/0';
    printf("Enter second binary string (max %d chars): ", MAX_BYTES-1);
    scanf("%s",&b[0]);
    b[MAX_BYTES] = '/0';
    printf("Calculating checksum... ");
    checksum_calc(a,b, &checksum[0]);
    printf("\nChecksum = %s\n", checksum);
    return 0;
}
*/

//AA: LRC (longitudinal redundancy check)
uint32_t checksum_calc(void *buffer, size_t len) {
    uint32_t seed = 0;
    unsigned char *buf = (unsigned char *)buffer;
    int i;
    for (i = 0; i < len; ++i)
        seed += (unsigned int)(*buf++);
    return seed;
}

//AA: controllo bit a bit
//1: successo, 0: errore
int checksum_cmp(uint32_t* c1, uint32_t* c2) {
    if(strlen(c1) != strlen(c2))
        return 0;
    while(c1 != NULL || c2 != NULL) {
        if(*c1++ != *c2++)
            return 0;
    }
    return 1;
} 
