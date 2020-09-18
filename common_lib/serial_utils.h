#ifndef SERIAL_UTILS_H
#define SERIAL_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"

int serial_align_data(uint8_t* src, uint8_t* dest, size_t size);

Data serial_wrap_data(uint8_t * data, uint32_t data_size, uint8_t data_type);

void serial_extract_data(Data * src, uint8_t * dest, uint32_t data_size);

#endif
