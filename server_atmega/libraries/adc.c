#include <stdint.h>
#include <avr/io.h>
#include "adc.h"

//PDGZ stuff
//funzioni per gestire l'adc

//inizializza l'adc in modo da avere meno overhead usando read_pin
void ADC_init(void){
  //disabilito gli input digitali sui pin a0-a15
  DIDR0 = 0x00;

  //azzero il bit ADC nel power reduction register
  //in modo da abilitare l'uso dell'adc
  PRR0 &= ~(1<<PRADC);    

  //imposto il prescaler per il clock adc a 64
  //ADCSRA2:0 tutti ad 1
  ADCSRA = 0x06;

  //accendo l'adc
  ADCSRA |= 1 << ADEN;
  
  // imposto ADMUX, i primi due bit per il voltage reference a 5v
  //bit ADLAR per il risultato left adjusted
  
  ADMUX = (1 << 6) |  (1 << ADLAR);
}


uint8_t ADC_read_pin(uint8_t pin){
  uint8_t res;
  ADMUX = (1 << 6) |  (1 << ADLAR) |(pin & 0x07);

  
  ADCSRA |= (1 << ADSC); // imposto il bit per iniziare la conversione
  
  while (ADCSRA & (1 << ADSC)); // quando leggerò  uno 0 la conversione sarà terminata
  //la conversione richiede 25 ADC clock (se si inizializza ADC) 13 per le successive
  
  // con il risultato left adjusted posso leggere ADCH per ottenere un risultato ad 8-bit
  // in questo modo si occupa meno memoria e l'adc puo' leggere più velocemente
 
  res = ADCH;
  
  return res;
}

//funzione init + read_pin alto overhead
uint8_t ADC_single_conversion(uint8_t pin){
  uint8_t res;

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
  
  // con il risultato left adjusted posso leggere ADCH per ottenere un risultato ad 8-bit
  // in questo modo si occupa meno memoria e l'adc puo' leggere più velocemente
 
  res = ADCH;
  
  return res;
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

