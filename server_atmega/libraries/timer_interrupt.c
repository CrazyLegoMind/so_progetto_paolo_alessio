#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//NB: in interrupt_timerh è definito che si userà OCR5

//PDGZ imposta il valore di OCR di modo da ottenere una frequenza
//di interrupt per la lettura dei pin con l'adc
void TIMER_set_frequency(uint32_t hz){
 
  // configurazione timer, prescaler a 1024
  TCCR5A = 0;
  TCCR5B = (1 << WGM52) | (1 << CS52); 
  
  // con 1 lsb equivalente a 16Mhz/256 ogni millisecondo si avranno
  // 62500 counts al secondo, impostiamo OCR al valore necessario per ottenere
  //un interrupt hertz richiesto, 100 hz = 100 volte al secondo
  // ovvero una volta ogni 1/100 di 62500
  //essendo uint16_t anche con 1hz non andrà in overflow
  uint16_t ocrval=(uint16_t)(62500.0/hz);

  OCR5A = ocrval;
}


//PDGZ funzione per attivare o disattivare l'interrupt timer, cli e sei
//andranno rispettivamente chiamati prima e dopo questa funzione
void TIMER_enable_interrupt(uint8_t status){
  //abilito o disabilito l'interrupt di OCR5, lascio invariato il resto del registro
  if(status){
    TIMSK5 |= (1 << OCIE5A);
  }else{
    TIMSK5 &= ~(1 << OCIE5A);
  }
}
