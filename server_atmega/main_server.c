#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
//#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <util/delay.h>
//#include <unistd.h>

//#include "avr/io.h"  //da decommentare se serve o cancellare
#include "libraries/adc.h"
#include "libraries/uart.h"
#include "libraries/timer_interrupt.h"
#include "../common_lib/defs.h"


/* struttura pacchetti ricevuti
typedef struct _init_pkg {
  uint8_t sampling_freq;
  uint8_t channels;
  uint8_t mode;
  uint8_t time;   //seconds
  int trigger;
} InitPkg;

typedef struct _data_pkg {
  uint32_t checksum;                  // controllo integrità dati, generato dal server
  uint16_t data;                      // valore misurato
  uint8_t mask_pin;                   // pin da dove si legge il valore
  uint8_t cmd;                        // codice per la gestione dei pacchetti
  int timestamp;                      // intero che rappresenta l'epoca dei pacchetti
} DataPkg;
 */




//AA: gestione pacchetti da server
//Da host dobbiamo sapere quale modalità adottare, quindi in primis ricevo il pacchetto di configurazione
//a seconda del valore letto procedo all'invio continuous o buffered

//#define SERVER_MSG_HEADER "ServerMsgHeader"
#define MAX_SERVER_STORAGE 1024                 //1K bytes = 10 pacchetti circa


//AA: invia messaggi lato server da stampare su host 
static void print_server_to_host(struct UART* uart, server_msg* msg) {
    assert(msg->text_size > 0);
    UART_putString(uart, msg->header, sizeof(msg->header));
    _delay_ms(100);         //do tempo al modulo host di elaborare l'informazione
    UART_putString(uart, msg->text, msg->text_size);
    return;
}

/* alternativa
static void print_server_to_host2(struct UART* uart, const char* txt, size_t size) {
    server_msg* msg = malloc(sizeof(server_msg));
    msg->text = txt;
    msg->text_size = size;
    UART_putString(uart, msg->header, sizeof(msg->header));
    UART_putString(uart, msg->text, msg->text_size);
    free(msg);
} */




//PDGZ:

//dati condivisi con  l'isr della lettura adc
uint8_t channels_list[8] = {8,8,8,8,8,8,8,8};
volatile uint8_t interrupt_occurred = 0;
uint16_t readings_done = 0;
DataPkg  pkg_temp;


ISR(TIMER_INTERRUPT){
  readings_done++;
  uint8_t i = 0;
  while(i < 8){
    if(channels_list[i] != 8){
      uint16_t res = ADC_single_conversion(channels_list[i]);
      pkg_temp.data = res;
      pkg_temp.mask_pin = i;
      pkg_temp.timestamp = readings_done;
    }else{
      break;
    }
  }
  interrupt_occurred = 1;
}


int main(int argc, char** argv) {

    //AA: spazio per ricezione InitPkg e invio dati (in buffered mode)
    InitPkg pkg;

    struct UART* uart_fd = UART_init();
    //AA: waiting InitPkg from host
    while(!UART_getData(uart_fd, (uint8_t*)&pkg , sizeof(InitPkg)));
    //some examples
    /*
      if(buffer_init == NULL) {
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
    //*/
    //memcpy(&pkg, &buffer_init[0], sizeof(buffer_init));

    //PDGZ
    //get data from packet to set server
    uint8_t mode = pkg.mode; //data
    uint8_t sampling_freq = pkg.sampling_freq; //hz
    uint8_t channels_mask = pkg.channels; //channel mask
    uint8_t time = pkg.time;   //seconds
    int trigger = pkg.trigger; //adc reading
    uint16_t readings_todo = sampling_freq*time;
    if(!mode) {
        //continuous

      //inizializzo la lista dei canali da leggere con l'adc
      //la lista avrà i canali da leggere all'inizio e tutti
      // 8 nelle rimanenti posizioni vuote, es voglio leggere
      // 2 e 4 list= [2,4,8,8,8,8,8,8]
      int p = 0,e = 7;
      for (int i = 0; i < 8; i++){
        if(channels_mask & 1 << i){
          channels_list[p] = i;
          p++;
        }else{
          channels_list[e] = 8;
          e--;
        }
      }
      //pulisco il temp pkg
      pkg_temp.checksum = 0;
      pkg_temp.data = 0;
      pkg_temp.mask_pin = 0;
      pkg_temp.cmd = 0;
      pkg_temp.timestamp  = 0;
      //pkg_temp. = 0;
      TIMER_set_frequency(sampling_freq);
      TIMER_enable_interrupt(1);
      while(readings_done < readings_todo){
	if (interrupt_occurred){
	  UART_putString(uart_fd, (uint8_t*) &pkg_temp, sizeof(DataPkg));


	}
      }
      TIMER_enable_interrupt(0);

    } else {
        //buffered

    }

    //sampling data 

    //send data to host
    int test_epoch = 0;
    while(1){
      pkg_temp.timestamp = test_epoch;
      test_epoch++;
       UART_putString(uart_fd, (uint8_t*) &pkg_temp, sizeof(DataPkg));
       _delay_ms(1000);
    }
    
    return EXIT_SUCCESS;
}
