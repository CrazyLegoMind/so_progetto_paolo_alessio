#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

//AA: funzione di generazione del checksum

// In this method a checksum is calculated based on the given binary strings which is sent with the data as redundant bits. 
// This data + checksum is received at receiver end and checksum is calculated again,
// if any error occurs, the function returns 0, the calculated value otherwise.

#define MAX_BYTES 5

char ext_mem[MAX_BYTES];

static uint32_t generate_checksum(char byte, char* seq) {
    double sum = 0;
    memset(ext_mem, 0, MAX_BYTES);
    ext_mem[0] = byte;
    memcpy(&ext_mem[1], seq, strlen(seq));
    int i;
    for(i = 0; i < MAX_BYTES; i++)
        sum +=  ext_mem[i] * pow(2,i);
    return (uint32_t)sum;
}

uint32_t checksum_calc(char* a, char* b) {
    int length;
    uint32_t ret_value = 0;
    if(strlen(a)==strlen(b)){
		length = strlen(a);
		char carry='0';
        
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
        
		printf("\nSum=%c%s",carry,sum);
		
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
        
		printf("\nChecksum=%c%s",carry,complement);
        ret_value = generate_checksum(carry, complement);
	}
	else {
		printf("\nWrong input strings");
	}
    return ret_value;
}

//debug main
/*
int main()
{
    char a[MAX_BYTES-1],b[MAX_BYTES-1];
    char sum[MAX_BYTES-1],complement[MAX_BYTES];
    
	printf("Enter first binary string\n");
    scanf("%s",&a);
    printf("Enter second binary string\n");
    scanf("%s",&b);
    checksum_calc(a,b);
}
*/