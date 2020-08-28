#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>

//NB: in interrupt_timerh è definito che si userà OCR5

//PDGZ imposta il valore di OCR di modo da ottenere una frequenza
//di interrupt per la lettura dei pin con l'adc
void TIMER_set_frequency(int hz){
  
  int timer_duration_ms= (int) 1000.0/hz;
  
  // configurazione timer, prescaler a 1024
  TCCR5A = 0;
  TCCR5B = (1 << WGM52) | (1 << CS50) | (1 << CS52); 
  
  // con 1 lsb equivalente a 16Mhz/1024 ogni millisecondo si avranno
  // 15.62 counts, impostiamo OCR al valore necessario per ottenere
  //un interrupt ongi <timer_duration_ms> millisecondi
  uint16_t ocrval=(uint16_t)(15.62*timer_duration_ms);

  OCR5A = ocrval;
}


//PDGZ funzione per attivare o disattivare l'interrupt timer, cli e sei
//andranno rispettivamente chiamati prima e dopo questa funzione
void TIMER_enable_interrupt(int status){
  //abilito o disabilito l'interrupt di OCR5, lascio invariato il resto del registro
  if(status){
    TIMSK5 |= (1 << OCIE5A);
  }else{
    TIMSK5 &= ~(1 << OCIE5A);
  }
}
