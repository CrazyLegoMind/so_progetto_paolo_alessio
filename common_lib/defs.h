#ifndef DEFS_H
#define DEFS_H

#define PKG_SIZE 2816

//AA: header per allineamento dati
#define DATA_HEADER "DataHeader"
#define INIT_HEADER "InitHeader"
#define HEADER_SIZE 11              //valida per entrambi gli header

#define SERVER_MSG_HEADER "ServerMsgHeader"
#define SERVER_MSG_HEADER_SIZE 16

//AA: struttura pacchetto (11 bytes = 2816 bits)
typedef struct _data_pkg {
  uint32_t checksum;                  // controllo integrità dati, generato dal server
  uint16_t data;                      // valore misurato
  uint8_t mask_pin;                   // pin da dove si legge il valore
  uint8_t cmd;                        // codice per la gestione dei pacchetti
  int timestamp;                      // intero che rappresenta l'epoca dei pacchetti
} DataPkg;

/*
//AA: blocco di DataPkg basata su SLAB allocator
typedef struct _buffered_mode_allocator {
char* memory;                       // memoria esterna messa a disposizione
int* pkg_list;                      // lista dei pacchetti da inviare
int memory_size;                    // quanta memoria è messa a disposizione
int pkg_list_size;                  // quanti pacchetti vengono mappati
int pkg_size;                       // grandezza in bytes di un singolo pacchetto
int first_pkg;                      // indice del primo blocco disponibile per un pacchetto
} BufferedModeAllocator;
*/

//AA: struttura che setta il server secondo i valori forniti dall'utente
typedef struct _init_pkg {
  uint8_t sampling_freq;
  uint8_t channels;
  uint8_t mode;
  uint8_t time;   //seconds
  int trigger;
} InitPkg;


//AA: messaggio di debug per atmega
typedef struct server_msg_ {
  char* header; //= SERVER_MSG_HEADER;
  char* text;
  size_t text_size;
} server_msg;

#endif
