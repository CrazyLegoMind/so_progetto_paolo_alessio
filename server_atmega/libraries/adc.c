#include <stdio.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include "../avr_common/uart.h"
#include "adc.h"

uint8_t v;
uint8_t in_pin;


int ADC_single_conversion(uint8_t pin){
  uint8_t low, high;

  DIDR0 = 0x00;           //disabilito gli input digitali sui pin a0-a15
  PRR0 &= ~(1<<PRADC);    //azzero il bit ADC nel power reduction register
  
  ADCSRA = 1 << ADEN;     //imposto il bit enable sulla porta A
  
  // imposto il bit MUX5 sulla porta B a seconda che il pin da leggere sia
  // 0-7 oppure 8-15

  ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
  
  // imposto ADMUX, dove i 4 bit meno significativi sono MUX0:4 (il resto
  // del valore del pin tolto MUX5) ed i primi due bit impostano il risultato
  // come left adjusted ed il voltage reference a 5v
  
  ADMUX = (1 << 6) | (pin & 0x07);

  
  ADCSRA |= (1 << ADSC); // imposto il bit per iniziare la conversione
  
  while (ADCSRA & (1 << ADSC)); // quando leggerò  uno 0 la conversione sarà terminata
  //la conversione richiede 25 ADC clock (se si inizializza ADC) 13 per le successive
  
  // con il risultato left adjusted la lettura di ADCL blocca entrambi i registri ADCL
  // ed ADCH fino alla lettura di quest'ultimo, quindi li leggo in ordine
  
  low  = ADCL;
  high = ADCH;

  // unisco i bit per creare il numero a precisione 10bit
  return (high << 8) | low;
}

void ADC_freerunnig_start(uint8_t pin){

  DIDR0 = 0x00;           //disabilito gli input digitali sui pin a0-a15
  PRR0 &= ~(1<<PRADC);    //azzero il bit ADC nel power reduction register
  
  ADCSRA = 1 << ADEN;     //imposto il bit enable sulla porta A
  
  // imposto il bit MUX5 sulla porta B a seconda che il pin da leggere sia
  // 0-7 oppure 8-15

  ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
  
  // imposto ADMUX, dove i 4 bit meno significativi sono MUX0:4 (il resto
  // del valore del pin tolto MUX5) ed i primi due bit impostano il risultato
  // come left adjusted ed il voltage reference a 5v
  
  ADMUX = (1 << 6) | (pin & 0x07);

  ADCSRA |= 1 << ADATE; //imposto il bit di auto trigger
  ADCSRB &= 0xF8; //azzero ADTS, bit 2:0 per selezionare la free running mode
  ADCSRA |= (1 << ADSC); // imposto il bit per iniziare la prima conversione


  //NB: la lettura dei valori andra fatta durante le ISR degli interrupt di conversion complete
}

void ADC_freerunnig_stop(int arg){
  ADCSRA &= ~(1 << ADEN) & ~(1<<ADATE); //azzero il bit che abilita l'ADC e quello che abilita l'auto trigger
}

