#include <stdio.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../../avr_common/uart.h"

uint8_t v;
uint8_t in_pin;

int main(int argc, char** argv) {
  printf_init();

  //abilitato la porta A in lettura
  ADMUX = 1<<ADLAR;                      //imponi risultato right adjusted
  ADCSRA |= 1<<7;                         //ADEN 
  //const uint8_t mask = (1<<6);
  uint8_t mask = 1;
  while(1) {
    v=0;
    ADCSRA |= 1<<ADSC;                       //inizio la conversione (ADSC)
    while((ADCSRA&mask) != 0);            //attendo la fine della conversione
    v = ADCH;
    printf("value: %u\n", v);
    _delay_ms(1000);
  }
  
  return 0;
}
