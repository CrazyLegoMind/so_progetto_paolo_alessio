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
#include "../common_lib/serial_utils.h"
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
Data data_received;


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
  
  struct UART* uart_fd = UART_init();
  sei();
  _delay_ms(100);
  while(1){
    if(UART_getData(uart_fd, (uint8_t*)&data_received , sizeof(Data)) == 1){   
      if(data_received.data_type == TYPE_INITPKG){
	InitPkg pkg;
	TextPkg msg;
	memcpy(msg.text,"found init packet",sizeof("found init packet"));
	msg.text_size = sizeof("found init packet");
	UART_putData(uart_fd, (uint8_t*) &msg, sizeof(TextPkg),TYPE_TEXTPKG);
	_delay_ms(100);
	
	serial_extract_data(&data_received,(uint8_t*)&pkg,sizeof(InitPkg));
	UART_putData(uart_fd, (uint8_t*) &pkg, sizeof(InitPkg),TYPE_INITPKG);
	_delay_ms(100);
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
	      UART_putData(uart_fd, (uint8_t*) &pkg_temp, sizeof(DataPkg),TYPE_DATAPKG);
	      interrupt_occurred = 0;
	    }
	  }
	  TIMER_enable_interrupt(0);

	} else {
	  //buffered

	}
      }else{
	//data type non riconosciuto
	;
      }
    }else{
      /*
      TextPkg packet;
      memcpy(packet.text,"error no packet",15);
      packet.text_size = 15;
      UART_putData(uart_fd, (uint8_t*) &packet, sizeof(TextPkg),TYPE_TEXTPKG);
      _delay_ms(100);
      */
    }
  }
  return EXIT_SUCCESS;
}
