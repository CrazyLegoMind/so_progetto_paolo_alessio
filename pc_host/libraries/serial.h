#ifndef SERIAL_H
#define SERIAL_H

//AA: funzioni per la gestione della comunicazione lato host
int uart_set(int, const unsigned int, uint8_t);
int serial_open(const uint8_t*);
int uart_read(int, void*, size_t);
int uart_write(int, void*, size_t);

#endif
