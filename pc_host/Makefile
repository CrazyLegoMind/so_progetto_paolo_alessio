CC = gcc
CFLAGS = -Wall
SRCS = libraries/serial.c main_host.c ../common_lib/serial_utils.c ../common_lib/checksum.c
HEADERS = libraries/serial.h ../common_lib/defs.h ../common_lib/checksum.h ../common_lib/serial_utils.h
OBJS = $(SRCS:.c=.o)
MAIN = main_host


.PHONY: depend clean

all: $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) $(SRCS) $(HEADERS) -o $(MAIN)

*.o: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -c

clean:
	rm -rf *.h.gch
	rm -rf *.o
	rm -rf $(MAIN)
