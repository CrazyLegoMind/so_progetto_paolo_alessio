#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <string.h>
#include "serial.h"
#include "../../common_lib/defs.h"

/* 
AA: funzione che imposta la comunicazione seriale UART
-fd: file descriptor della seriale
-baude: bauderate da impostare (4800, 9600. 19200, 38400, 115200)
-parity: impostazione controllo di parità (n-N default, o-O, e-E)
*/
int serial_set(int fd, const unsigned int baude, uint8_t parity) {
    struct termios options;

    //AA: impostazione iniziale del riferimento termios basato su fd
    if(tcgetattr(fd,&options) < 0) {
	perror("tcgetattr error");
        return -1;
    }

    //set dei baudrate in input e output
    switch(baude) {
        case 4800:
            cfsetispeed(&options,B4800);
            cfsetospeed(&options,B4800);
            break;
        case 9600:
            cfsetispeed(&options,B9600);
            cfsetospeed(&options,B9600);
            break;
        case 19200:
            cfsetispeed(&options,B19200);
            cfsetospeed(&options,B19200);
            break;
        case 38400:
            cfsetispeed(&options,B38400);
            cfsetospeed(&options,B38400);
            break;
	 case 115200:
            cfsetispeed(&options,B115200);
            cfsetospeed(&options,B115200);
            break;
        default:
            printf("Unkown baude!\n");
            return -1;
    }

    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;


    // AA qui si imposta la grandezza dei caratteri durante la trasmissione
    // default 8 bit
    /*
    switch(bits) {
        case 5:
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS5;
            break;
        case 6:
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS6;
            break;
        case 7:
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS7;
            break;
        case 8: */
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS8;
    /*
            break;
        default:
            fprintf(stderr,"Unkown bits!\n");
            return -1;
    } */

    //AA qui si imposta il controllo di parità sui dati (n-N: normal, e-E: even, o-O: odd)
    switch(parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            options.c_iflag |= INPCK;
            options.c_iflag |= ISTRIP;
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            options.c_iflag |= ISTRIP;
            break;
        default:
            fprintf(stderr,"Unkown parity!\n");
            return -1;
    }

    /*impostazione del numero di stop bits (1 o 2)
    switch(stop)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr,"Unkown stop!\n");
            return -1;
    } */

    //AA: impostazioni output
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //impostazioni outoput in non-canonical mode (quando ICANON non è abilitato)
    //in questo caso ogni lettura tramite syscall read è bloccata fino a quando 1 byte è disponibile, e ritorna il numero di byte richiesti (fonte: linux man)
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 1;	//0.1 seconds timeout

    tcflush(fd,TCIFLUSH);
    
    //AA: si settano i parametri imposti nella struct termios con effetto immediato (dal flag TCSANOW)
    if(tcsetattr(fd,TCSANOW,&options) < 0)
    {
        perror("tcsetattr failed");
        return -1;
    }

    /* AA: debug check sulle nuove impostazioni
    if(tcgetattr(fd,&options) < 0) {
	perror("tcgetattr error");
        return -1;
    }
    */

    return 0;

}

/*AA: funzione che restituisce il file descriptor corrispondente al server
-path: percorso della seriale (in genere /dev/ttyACM0)
*/
int serial_open(const char* path) {
	uint32_t fd = open(path, O_RDWR | O_SYNC | O_NOCTTY);
	if(fd < 0) {
		fprintf(stderr, "Error opening serial device %s\n", path);
		return -1;
	}
	return fd;
}

/*AA: funzione ausiliaria che ricopia i dati contenuti in un buffer e genera un pacchetto di informazioni
-buf: buffer da cui prendere le informazioni
-bufsize: grandezza buffer
*/
static DataPkg* gen_pkg(uint8_t* buf, size_t bufsize) {
	DataPkg* pkg = (DataPkg*)malloc(sizeof(DataPkg));
	memcpy(pkg, buf, bufsize);
	return pkg;
}

static char* select_header(size_t dim) {
    return (dim == 11 ? DATA_HEADER : INIT_HEADER);
}

/*AA: funzione per allineamento dati riicevuti dalla seriale
-src: puntatore ad area da esaminare
-dest: puntatore ad area da restituire
-size: dimensione memoria
*/
static int serial_align_data(uint8_t* src, uint8_t* dest, size_t size) {
    uint8_t* tmp = malloc(size*2);
    memcpy(tmp, src, size);
    memcpy(tmp+size, src, size);
    int i,c;
    char* header = select_header(size);
    for(i=0; i < size; i++) {
        //trova posizione dell'header
        c = memcmp(tmp+i, header, HEADER_SIZE);
        if(!c) break;
    }

    if(i==size)
        return -1;
    i += HEADER_SIZE;
    for(int j=0; j < size - HEADER_SIZE; j++) { 
        i = i%size;
        *(dest+j) = *(src+i);
        i++;
    }
    free(tmp);
    return 1;
}

/*AA: funzione di lettura di 8 bit alla volta dal server
-fd: file descriptor generato da uart_fd
-buf: riferimento all'oggetto che si vuole leggere
-size: quanto si vuole leggere dal server
*/ 
int serial_read(int fd, void* buf, size_t size) {
    size += HEADER_SIZE;
	uint8_t* locbuf = malloc(size);
	uint8_t aux;
	int i;
	for(i=0; i < size; i++) {
		if(read(fd, &aux, sizeof(uint8_t)) == -1) {
			fprintf(stderr, "Error while reading byte %d", i);
			free(locbuf);	
			return -1;
		}
		*(locbuf+i) = aux;
	}
	//printf("Read completed.\n");
    //align data
    if(serial_align_data(locbuf,buf,size) == -1) {
        printf("Error while aligning data!\n");
        return -1;
    }
	//save local package 
    buf = gen_pkg(locbuf, buf);
	free(locbuf);
	return 1;
}

/*AA: funzione di scrittura dati verso il server */ 
int serial_write(int fd, void* buf, size_t size) {
	uint8_t* b = malloc(size + HEADER_SIZE);
    char* header = select_header(size);
    memcpy(b, header, HEADER_SIZE);
	memcpy(b+HEADER_SIZE, buf, size);
	int i;
	for(i=0; i < size; i++) {
		//send data to server 1 byte per time
		if(write(fd, b+i, sizeof(uint8_t)) == -1) {
			fprintf(stderr,"Error while writing byte %d\n",i);
			free(b);	
			return -1;
		}
	} 
	//printf("Write completed.\n");
	free(b);
	return 1;
}
