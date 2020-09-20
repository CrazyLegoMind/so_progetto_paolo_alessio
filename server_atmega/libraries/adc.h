
void ADC_init(void);

uint8_t ADC_read_pin(uint8_t pin);

uint8_t ADC_single_conversion(uint8_t pin);

void ADC_freerunning_start(uint8_t pin);

void ADC_freerunnig_stop(int arg);
