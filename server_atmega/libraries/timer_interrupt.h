// PDGZ definisco l'interrupt del timer come quello generato
// dal registro compare del timer 5

#define TIMER_INTERRUPT TIMER5_COMPA_vect

void TIMER_set_frequency(uint32_t hz);
void TIMER_enable_interrupt(uint8_t status);
