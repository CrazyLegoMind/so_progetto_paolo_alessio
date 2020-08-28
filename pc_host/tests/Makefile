#makefile for pc host tests with headers

#include "../../common_lib/defs.h"
#include "../libraries/serial.h"

IDIR = ../libraries
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=.
LDIR =../../common_lib

LIBS=-lm

_DEPS = serial.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = serial_test.o serial.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


<<<<<<< Updated upstream
$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
=======
# put here the file containing the main routine
# to be uploaded on the avr
# you can add multiple files, they will be all generated

BINS=serial_test.elf 

# put here the additional .o files you want to generate
# one .c file for each .o should be present
OBJS=

# put here the additional header files needed for compilation
HEADERS= ../../common_lib/defs.h ../libraries/serial.h

# the file below contains the actual rules

include ../../server_atmega/avr_common/avr.mk

# !!!!!!!!!!!!!!!! MODIFY HERE !!!!!!!!!!!!!!!! 
>>>>>>> Stashed changes

serial_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
