#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <util/delay.h>
#include "libraries/adc.h"
#include "libraries/uart.h"
#include "libraries/timer_interrupt.h"
#include "../common_lib/serial_utils.h"
#include "../common_lib/defs.h"

//AA: gestione pacchetti da server
//Da host dobbiamo sapere quale modalità adottare, quindi in primis ricevo il pacchetto di configurazione
//a seconda del valore letto procedo all'invio continuous o buffered


//PDGZ:

//dati condivisi con  l'isr della lettura adc
uint8_t channels_list[8] = {8,8,8,8,8,8,8,8};
volatile uint8_t interrupt_occurred = 0;
volatile uint16_t readings_done = 0;
DataPkg  pkg_temp;
Data data_received;


ISR(TIMER5_COMPA_vect){
  readings_done++;
  interrupt_occurred = 1;
  uint8_t i = 0;
  while(i < 8){
    if(channels_list[i] != 8){
      pkg_temp.data = ADC_read_pin(channels_list[i]);;
      pkg_temp.mask_pin = i;
      pkg_temp.timestamp = readings_done;
      i++;
    }else{
      break;
    }
  }
}

void send_msg(struct UART* fd,char*  message, size_t size){
  TextPkg pkg_msg;
  memcpy(pkg_msg.text,message,size);
  pkg_msg.text_size = size;
  UART_putData(fd, (uint8_t*) &pkg_msg, sizeof(TextPkg),TYPE_TEXTPKG);
  _delay_ms(100);
}


int main(int argc, char** argv) {
  //inizializzo ADC ed UART
  struct UART* uart_fd = UART_init();
  ADC_init();
  sei();
  _delay_ms(100);

  
 
  
  while(1){
    if(UART_getData(uart_fd, (uint8_t*)&data_received , sizeof(Data)) == 1){
      
      if(data_received.data_type == TYPE_INITPKG){
		InitPkg pkg;
		send_msg(uart_fd,"received INIT",sizeof("received INIT"));
		serial_extract_data(&data_received,(uint8_t*)&pkg, sizeof(InitPkg));
		

		uint8_t sampling_freq = pkg.sampling_freq; //hz
		uint8_t channels_mask = pkg.channels; //channel mask
		uint8_t time = pkg.time;   //seconds
		uint8_t trigger = pkg.trigger; //adc reading
		uint16_t readings_todo = sampling_freq*time;
		if(!pkg.mode) {
		//continuous
		//inizializzo la lista dei canali da leggere con l'adc
		//la lista avrà i canali da leggere all'inizio e tutti
		// 8 nelle rimanenti posizioni vuote, es voglio leggere
		// 2 e 4 list= [2,4,8,8,8,8,8,8]
		send_msg(uart_fd,"continuos mode",sizeof("continuous mode"));
		int p = 0,e = 7;
		for (int i = 0; i < 8; i++){
			if(channels_mask & 1 << i){
			channels_list[p] = i;
			p++;
			}else{
			channels_list[e] = 8;
			e--;
			}
		};
		//pulisco il temp pkg
		pkg_temp.checksum = 0;
		pkg_temp.data = 0;
		pkg_temp.mask_pin = 0;
		pkg_temp.cmd = 0;
		pkg_temp.timestamp  = 0;
		readings_done = 0;
		TIMER_set_frequency(sampling_freq);
		TIMER_enable_interrupt(1);
			while(readings_done < readings_todo){
			if (interrupt_occurred){
			UART_putData(uart_fd, (uint8_t*) &pkg_temp, sizeof(DataPkg),TYPE_DATAPKG);
			interrupt_occurred = 0;
			}
		}
		TIMER_enable_interrupt(0);
		send_msg(uart_fd,"finished reading",sizeof("finished reading"));

		} else {
		send_msg(uart_fd,"buffered mode",sizeof("buffered mode"));
		}
    }else{
		//data type non riconosciuto
		send_msg(uart_fd,"received UNVALID",sizeof("received UNVALID"));
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
