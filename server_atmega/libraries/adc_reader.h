#include "../avr_common/uart.h"

int ADC_single_conversion(uint8_t pin);
void ADC_freerunning_start(uint8_t pin);
void ADC_freerunnig_stop(int arg);
