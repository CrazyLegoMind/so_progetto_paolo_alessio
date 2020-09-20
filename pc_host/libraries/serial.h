#ifndef SERIAL_H
#define SERIAL_H

#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "../../common_lib/defs.h"
#include "../../common_lib/serial_utils.h"


//AA: funzioni per la gestione della comunicazione lato host
int serial_set(int fd, int speed, int parity);
int serial_open(const char* path);
void serial_set_blocking(int fd, int should_block);
int serial_read(int fd, uint8_t* buf, size_t size);
int serial_write(int fd, void* buf, size_t size);
void serial_send_data(int fd, uint8_t * data, uint32_t data_size, uint8_t data_type);

#endif
