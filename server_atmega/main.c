#include "libraries/uart.h"
#include "../common_lib/defs.h"
#include "../common_lib/serial_utils.h"


#define RECEIVE_AMOUNT 5
#define SEND_AMOUNT 10
UART uart_ptr;

int main(char agrc, char* argv){
  uart_ptr = UART_init();
  uint8_t counter = 0;
  uint8_t* receive_pkg[syzeof(_init_pkg)];
  uint8_t success;
  while(counter < RECEIVE_AMOUNT){
    success = UART_getData(uart_ptr,receive_pkg,syzeof(_init_pkg));
    if(success){
      
    }
  }
  
}
