#ifndef UART_H
#define UART_H

#include <util/delay.h>
#include <stdio.h>
#include <stdint.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "../../common_lib/defs.h"

#ifdef _RENAME_UART_
#define UART UART_
#endif

#define BAUD 115200
#define MYUBRR (F_CPU/16/BAUD-1)

struct UART;

// initializes a uart object
// returns 0 on failure
struct UART * UART_init(void);

// puts a character in the buffer of a uart
// if the buffer is full, the function waits until
// there is room in the buffer
void UART_putChar(struct UART* uart, uint8_t c);

// returns a character from the uart.
// if no character is available, the function waits
uint8_t UART_getChar(struct UART* uart);

// returns the size of the rx buffer
int  UART_rxbufferSize(struct UART* uart);

// returns the size of the tx buffer
int  UART_txBufferSize(struct UART* uart);

// returns the number of characters to be read fron the rx buffer
int UART_rxBufferFull(struct UART* uart);

// returns the number of available characters in the tx buffer
int UART_txBufferFree(struct UART* uart);

// returns the number of characters to be read fron the rx buffer
int UART_txBufferFull(struct UART* uart);

void UART_putString(struct UART * uart, uint8_t * buf, size_t size);
//put a string of char on serial buffer

void UART_putData(struct UART * uart, uint8_t * data, uint32_t data_size, uint8_t data_type);
//add header and put data on serial buffer

uint8_t UART_getData(struct UART * uart, uint8_t * buf, size_t data_size);
//get Data from serial, return 1 if data are available, else 0
#endif
