# AVR makefile stub

# to clean a folder
# from command line
#$> make clean

# to build the folder
#$> make 

# to upload a file
# from command line
# $> make <main_filename>.hex

#
# remember to give yourself the read/write permissions on the
# serial line
# $> sudo addgroup <your username> dialout
# logout/login



# !!!!!!!!!!!!!!!! MODIFY HERE !!!!!!!!!!!!!!!! 

# put here the file containing the main() routine
# to be uploaded on the avr
# you can add multiple files, they will be all generated

BINS=main_server.elf

# put here the additional .o files you want to generate
# one .c file for each .o should be present
OBJS= libraries/adc.o libraries/uart.o libraries/timer_interrupt.o ../common_lib/serial_utils.o

# put here the additional header files needed for compilation
HEADERS= libraries/adc.h libraries/uart.h libraries/timer_interrupt.h ../common_lib/defs.h ../common_lib/serial_utils.h  libraries/buffer_utils.h


# the file below contains the actual rules

# !!!!!!!!!!!!!!!! MODIFY HERE !!!!!!!!!!!!!!!! 

include avr_common/avr.mk
