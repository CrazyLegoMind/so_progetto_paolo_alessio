#include "serial.h"

/* 
   AA: funzione che imposta la comunicazione seriale UART
   -fd: file descriptor della seriale
   -baude: bauderate da impostare (4800, 9600. 19200, 38400, 115200)
   -parity: impostazione controllo di parità (n-N default, o-O, e-E)
*/
int serial_set(int fd, int speed, int parity) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) {
    printf ("error from tcgetattr");
    return -1;
  }
  switch (speed){
  case 9600:
    speed=B9600;
  break;
  case 19200:
    speed=B19200;
    break;
  case 57600:
    speed=B57600;
    break;
  case 115200:
    speed=B115200;
    break;
  default:
    printf("cannot sed baudrate %d\n", speed);
    return -1;
  }
  cfsetospeed (&tty, speed);
  cfsetispeed (&tty, speed);
  cfmakeraw(&tty);
  // enable reading
  tty.c_cflag &= ~(PARENB | PARODD);               // shut off parity
  tty.c_cflag |= parity;
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;      // 8-bit chars

  if (tcsetattr (fd, TCSANOW, &tty) != 0) {
    printf ("error from tcsetattr");
    return -1;
  }
  return 0;
}



/*AA: funzione che restituisce il file descriptor corrispondente al server
  -path: percorso della seriale (in genere /dev/ttyACM0)
*/
int serial_open(const char* name) {
  int fd = open (name, O_RDWR | O_NOCTTY | O_SYNC );
  if (fd < 0) {
    fprintf (stderr,"error opening serial, fd %d\n", fd);
  }
  return fd;
}

void serial_set_blocking(int fd, int should_block) {
  struct termios tty;
  memset (&tty, 0, sizeof tty);
  if (tcgetattr (fd, &tty) != 0) {
      printf ("error from tggetattr");
      return;
  }

  tty.c_cc[VMIN]  = should_block ? 1 : 0;
  tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

  if (tcsetattr (fd, TCSANOW, &tty) != 0)
    printf ("error setting term attributes");
}

/*AA: funzione di lettura di 8 bit alla volta dal server
  -fd: file descriptor generato da uart_fd
  -buf: riferimento all'oggetto che si vuole leggere
  -size: quanto si vuole leggere dal server
*/ 
int serial_read(int fd, uint8_t* buf, size_t size) {
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
  ///*
  printf("DATA RECEIVED  with size %ld: ",size);
  for(i = 0; i < size; i++){
    printf("%hhx ",locbuf[i]);
  }
  /*
  printf("\n");
  printf("[DEBUG] searching header %s\n",HEADER);
  //*/
  if(serial_align_data(locbuf,buf,size) == -1) {
    printf("Error while aligning data!\n");
    return -1;
  }
  free(locbuf);
  return 1;
}

/*AA: funzione di scrittura dati verso il server */ 
int serial_write(int fd, void* buf, size_t size) {
  uint8_t* b = malloc(size + HEADER_SIZE);
  memcpy(b, HEADER, HEADER_SIZE);
  memcpy(b+HEADER_SIZE, buf, size);
  int i;
  size += HEADER_SIZE;
  printf("DATA sent with size %ld: ",size);
  for(i = 0; i < size; i++){
    printf("%hhx ",b[i]);
  }
  printf("\n");
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

void serial_send_data(int fd, uint8_t * data, uint32_t data_size, uint8_t data_type){
  Data d = serial_wrap_data(data,data_size,data_type);
  serial_write(fd,(uint8_t*)&d,sizeof(Data));
}
