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

  printf_init();
  int v = 0;
  uint8_t pin = 0;
  int it = 0;
  while(1){
    it++;
    v = get_digital_value(pin);
    printf("%d value: %u\n", it, v);
    _delay_ms(100);
  }
  return 0;
}

int get_digital_value(uint8_t pin){
  uint8_t low, high;

  DIDR0 = 0x00;           //Digital input disabled on all ADC ports
  PRR0 &= ~(1<<PRADC);    //Enabled adc on the power reduction register

  ADCSRA = 1 << ADEN; //enabling ADC itself

  // the MUX5 bit of ADCSRB selects whether we're reading from channels
  // 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).

  ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
  
  // set the analog reference (high two bits of ADMUX) and select the
  // channel (low 4 bits).  this also sets ADLAR (left-adjust result)
  // to 0 (the default).
  
  ADMUX = (1 << 6) | (pin & 0x07);

  // start the conversion
  ADCSRA |= (1 << ADSC);
  // ADSC is cleared when the conversion finishes
  while (ADCSRA & (1 << ADSC));
  
  // we have to read ADCL first; doing so locks both ADCL
  // and ADCH until ADCH is read.  reading ADCL second would
  // cause the results of each conversion to be discarded,
  // as ADCL and ADCH would be locked when it completed.
  low  = ADCL;
  high = ADCH;

  // combine the two bytes
  return (high << 8) | low;
}
