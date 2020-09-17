#ifndef SERIAL_H
#define SERIAL_H

//AA: funzioni per la gestione della comunicazione lato host
int serial_set(int fd, int speed, int parity);
int serial_open(const char* path);
int serial_read(int fd, void* buf, size_t size);
int serial_write(int fd, void* buf, size_t size);

#endif
