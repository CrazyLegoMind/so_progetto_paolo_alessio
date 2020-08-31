#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
//#include <fcntl.h>
#include <string.h>
#include <assert.h>

#include "avr/io.h"
#include "libraries/adc.h"
#include "libraries/uart.h"
#include "../common_lib/defs.h"


//AA: gestione pacchetti da server
//Da host dobbiamo sapere quale modalità adottare, quindi in primis ricevo il pacchetto di configurazione
//a seconda del valore letto procedo all'invio continuous o buffered

#define SERVER_MSG_HEADER "ServerMsgHeader"
#define MAX_SERVER_STORAGE 1024                 //1K bytes = 10 pacchetti circa

//AA: messaggio che comunica delle situazioni che possono rivelarsi sul atmega
typedef struct server_msg_ {
    char* header = SERVER_MSG_HEADER;
    char* text;
    size_t text_size;
} server_msg;

//AA: invia messaggi lato server da stampare su host 
static void print_server_to_host(struct UART* uart, server_msg* msg) {
    UART_putString(uart, msg->header, sizeof(msg->header));
    UART_putString(uart, msg->text, msg->text_size);
    return;
}

int main(int argc, char** argv) {

    //AA: spazio per ricezione InitPkg e invio dati (in buffered mode)
    InitPkg pkg;
    char* buffer_data[MAX_SERVER_STORAGE];
    char* buffer_init[sizeof(InitPkg)+1];
    buffer_init[sizeof(InitPkg)] = '\0';

    struct UART* uart_fd = UART_init();
    //AA: waiting InitPkg from host
    while(UART_getData(uart_fd, buffer_init, sizeof(buffer_init))) {
        printf("Waiting for settings from host...");
    }
    //some examples
    if(!buffer_init) {
        server_msg* msg1 = malloc(sizeof(server_msg));
        msg1->text = "Error while receveing init pkg";
        msg1->text_size = sizeof(msg1->text);
        print_server_to_host(uart_fd, msg1);
        free(msg1);
        return EXIT_FAILURE;
    } else if(strlen(buffer_init) < 11) { 
        server_msg* msg2 = malloc(sizeof(server_msg));
        msg2->text = "Package not completed";
        msg2->text_size = sizeof(msg2->text);
        print_server_to_host(uart_fd, msg2);
        free(msg2);
        return EXIT_FAILURE;
    }
    memcpy(&pkg, &buffer_init[0], sizeof(buffer_init));
    printf("Settings received from host. The selected mode is %s.\n", (pkg.mode == 0 ? "continuous" : "buffered"));

    //get data from packet to set server

    //sampling data 

    //send data to host

    return EXIT_SUCCESS;
}
