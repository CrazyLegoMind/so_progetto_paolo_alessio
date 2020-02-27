#include <termios.h>
#include <stdlib.h>
#include <stdint.h>



/* 
AA: funzione che imposta la comunicazione seriale UART
-fd: file descriptor della seriale
-baude: bauderate da impostare (4800, 9600. 19200, 38400, 115200)
-bits: da quanti bit è formata l'informazione (5,6,7,8)
-parity: impostazione controllo di parità (n-N, o-O, e-E)
-stop: numero di spot bits desiderati (1,2)
*/
int uart_set(int fd, int baude, int bits, char parity, int stop)
{
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
            fprintf(stderr,"Unkown baude!\n");
            return -1;
    }

    options.c_cflag |= CLOCAL;
    options.c_cflag |= CREAD;


    // AA qui si imposta la grandezza dei caratteri durante la trasmissione
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
        case 8:
            options.c_cflag &= ~CSIZE;
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr,"Unkown bits!\n");
            return -1;
    }

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

    //impostazione del numero di stop bits (1 o 2)
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
    }

    //AA: impostazioni output
    options.c_oflag &= ~OPOST;
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    //impostazioni outoput in non-canonical mode (quando ICANON non è abilitato)
    //in questo caso ogni lettura tramite syscall read è bloccata fino a quando 1 byte è disponibile, e ritorna il numero di byte richiesti (fonte: linux man)
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 1;

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
