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
uint8_t channels_amount = 0;
volatile uint8_t interrupt_occurred = 0;
volatile uint16_t readings_done = 0;
DataPkg  pkg_temp;
Data data_received;

#define CHANNEL_BUFFER_SIZE 256
#define MAX_CHANNELS 8
typedef struct channels_buffers {
  uint8_t chbuf_matrix[MAX_CHANNELS][CHANNEL_BUFFER_SIZE];
  volatile uint8_t chbuf_start;
  volatile uint8_t chbuf_end;
  volatile uint8_t chbuf_size;


} Buffers;
Buffers buf;



ISR(TIMER5_COMPA_vect){
  readings_done++;
  interrupt_occurred = 1;
  uint8_t i = 0;
  for(i=0; i <channels_amount; i++){
    buf.chbuf_matrix[channels_list[i]][buf.chbuf_end] = ADC_read_pin(channels_list[i]);
  }
  buf.chbuf_size++;
  if(++buf.chbuf_end > CHANNEL_BUFFER_SIZE) buf.chbuf_end =0;
}

void send_msg(struct UART* fd,char*  message, size_t size){
  TextPkg pkg_msg;
  memcpy(pkg_msg.text,message,size);
  pkg_msg.text_size = size;
  UART_putData(fd, (uint8_t*) &pkg_msg, sizeof(TextPkg),TYPE_TEXTPKG);
  _delay_ms(100);
}

void fill_channels_list(uint8_t pin_mask){
  channels_amount = 0;
  for (int i = 0; i < 8; i++){
    if(pin_mask & 1 << i){
      channels_list[channels_amount] = i;
      channels_amount++;
    }
  }
}


int main(int argc, char** argv) {
  //inizializzo ADC ed UART
  struct UART* uart_fd = UART_init();
  buf.chbuf_end = 0;
  buf.chbuf_start = 0;
  buf.chbuf_size = 0;
  ADC_init();
  sei();
  _delay_ms(100);

  while(1){
    if(UART_getData(uart_fd, (uint8_t*)&data_received , sizeof(Data)) == 1){
      //se ricevo un pkg ne controllo il tipo
      if(data_received.data_type == TYPE_INITPKG){
	InitPkg pkg;
	
	serial_extract_data(&data_received,(uint8_t*)&pkg, sizeof(InitPkg));
	
	uint16_t readings_todo =  pkg.sampling_freq*pkg.time;
	if(!pkg.mode) {
	  //continuous
	  fill_channels_list(pkg.channels);
	  //pulisco il temp pkg
	  memset(&pkg_temp,0,sizeof(DataPkg));
	  readings_done = 0;
	  TIMER_set_frequency(pkg.sampling_freq);
	  TIMER_enable_interrupt(1);
	  int c,pin;
     	  while(readings_done < readings_todo){
	    if (buf.chbuf_size){
	      for(c = 0; c < channels_amount; c++){
		pin = channels_list[c];
		pkg_temp.data = buf.chbuf_matrix[pin][buf.chbuf_start];
		pkg_temp.mask_pin = pin;
		pkg_temp.timestamp = readings_done;
		UART_putData(uart_fd, (uint8_t*) &pkg_temp, sizeof(DataPkg),TYPE_DATAPKG);
	      }
	      if(++buf.chbuf_start > CHANNEL_BUFFER_SIZE) buf.chbuf_start =0;
	      buf.chbuf_size--;
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
