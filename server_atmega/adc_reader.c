#include <stdio.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../../avr_common/uart.h"
#include "adc_reader.h"


uint8_t v;
uint8_t in_pin;

int main(int argc, char** argv) {

  DDRF &= 0x00;
  PORTF &= 0xFF;
  printf_init();
  int v = 0;
  uint8_t pin = 1;
  while(1){
    v = get_digital_value(pin);
    printf("value: %u\n", v);
    _delay_ms(100);
  }
  return 0;
}

int get_digital_value(uint8_t pin)
{
  //paolo, test dell'adc su singolo pin
  uint8_t low, high;
  
  // imposta MUX5 a 1 in caso si vogliano leggere i pin A8-A15
  ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);

  // imposto il canale corrispondente al pin (bit3:0 di ADMUX) mentre
  //imposto ADLAR REFS1 REFS0 a 0 in modo d avere Vcc come riferimento
  // ed il risultato della conversione right-adjusted
  ADMUX = (pin & 0x07);

  //imposto ad 1 ADSC per iniziare una conversione
  ADCSRA |= 1<<ADSC;

  // busy-waiting su ADSC per aspettare la fine conversione
  while (ADCSRA | 1<<ADSC);

  // la lettura di ADCL blocca qualsiasi converione fino a che
  //non venga letto ADCH, se leggessi prima ADCH l'altro registro
  //verrebbe scartato
  low  = ADCL;
  high = ADCH;

  // combino i due byte per avre 10-bit di precisione
  return (high << 8) | low;
}
