#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
//#include <signal.h>

#include "../pc_host/libraries/serial.h"
#include "defs.h"

//AA: serve per ricevere messaggi di debug provenienti dall'atmega e stamparli a schermo
const char* module_name = "msg_reader";


int main(int argc, char** argv) {
    //AA: leggi header, tutti i pacchetti che non hanno header corrispondente non vengono considerati
    int fd = serial_open("/dev/ttyACM0");
    if(fd < 0) 
    	return EXIT_FAILURE;
    if(serial_set(fd, 115200, 'n') == -1)
    	return EXIT_FAILURE;
    printf("[%s] Serial opened.\n", module_name);
    
    char choice = '';
    char header_buf[SERVER_MSG_HEADER_SIZE];
    char msg_buf[32];                           //le funzioni leggono 8 byte alla volta, quindi ho scelto un multiplo
    do {
        memset(header_buf, 0, SERVER_MSG_HEADER_SIZE);
        memset(msg_buf, 0, 32);
        if(serial_read(fd, header_buf, SERVER_MSG_HEADER_SIZE) == -1) {
            printf("[%s] Something went wrong.\n", module_name);
            return EXIT_FAILURE;
        }
        //controlla header, se è quello giusto mostra il messaggio
        if(memcmp(header_buf, SERVER_MSG_HEADER, SERVER_MSG_HEADER_SIZE) == 0) {
            //so la prossima informazione sarà il messaggio del server
            serial_read(fd, msg_buf, sizeof(msg_buf));
            printf("[%s] %s\n", module_name, msg_buf);
        }
        //diamo scelta all'utente se continuare a mostrare messaggi dal server
        printf("[%s] Do you want to listen other messages from server? (y or n) ", module_name);
        scanf("%c", &choice);
        if(choice != 'y') break;
    } while(1);
    printf("[%s] Stop working.\n\n", module_name);
    return EXIT_SUCCESS;
}