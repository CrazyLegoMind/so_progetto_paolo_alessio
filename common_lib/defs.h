#pragma once

#define PKG_SIZE 2816

//<<<<<<< HEAD
//AA: header per allineamento dati
#define DATA_HEADER "DataHeader"
#define INIT_HEADER "InitHeader"
#define HEADER_SIZE 11              //valida per entrambi gli header

//=======
//>>>>>>> master
//AA: struttura pacchetto (11 bytes = 2816 bits)
typedef struct _data_pkg {
  uint32_t checksum;                  // generato dal server
  uint16_t data;                      // valore misurato
  //<<<<<<< HEAD
  uint8_t cmd;                        // codice per la gestione dei pacchetti
  //=======
  uint8_t mask_pin;                  // pin da dove si legge il valore
  //>>>>>>> master
  int timestamp;                      // intero progressivo di mappatura dei pacchetti nell'allocatore
} DataPkg;

//AA: blocco di DataPkg basata su SLAB allocator
typedef struct _buffered_mode_allocator {
  char* memory;                       // memoria esterna messa a disposizione
  int* pkg_list;                      // lista dei pacchetti da inviare
  int memory_size;                    // quanta memoria è messa a disposizione
  int pkg_list_size;                  // quanti pacchetti vengono mappati
  int pkg_size;                       // grandezza in bytes di un singolo pacchetto
  int first_pkg;                      // indice del primo blocco disponibile per un pacchetto
} BufferedModeAllocator;

//AA: struttura che setta il server secondo i valori forniti dall'utente
typedef struct _init_pkg {
	uint8_t sampling_freq;
	uint8_t channels;
  uint8_t mode;
  uint8_t time;   //seconds
  int trigger;
} InitPkg;

