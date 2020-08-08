fndef SERIAL_UTILS_H
#define SERIAL_UTILS_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "defs.h"



//int serial_order_data(uint8_t * src,uint8_t * dst, size_t size);
//struct data fill_data(uint8_t * data, uint32_t data_size, uint8_t data_type);
//void parse_data (struct data * message_data, uint8_t * data, uint32_t data_size);

static char* select_header(size_t dim);
static int serial_align_data(uint8_t* src, uint8_t* dest, size_t size);
#endif
