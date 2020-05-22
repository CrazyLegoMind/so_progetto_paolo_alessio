#pragma once



//AA: struttura pacchetto
typedef struct _data_pkg {
  uint32_t* checksum;                // generato dal server
  uint16_t cmd;                      // codice per l'attivazione di una routine da ISR
  //uint16_t mask_pin;               // pin da dove si legge il valore
  //uint8_t data_rate;               // frequenza di lettura 
  uint16_t data;                     // valore misurato
  //int timestamp;                   //intero progressivo di mappatura dei pacchetti nell'allocatore
} DataPkg;

//AA: blocco di DataPkg basata su SLAB allocator
typedef struct _buffered_mode_allocator {
  char* memory;                      // memoria esterna messa a disposizione
  int memory_size;                   // quanta memoria è messa a disposizione
  int pkg_list_size;                 // quanti pacchetti vengono mappati
  int* pkg_list;                     // lista di indici che mappa i pacchetti da inviare contenuti nel buffer di memoria 
  int pkg_size;                      // grandezza in bytes di un singolo pacchetto
  int first_pkg;                     // indice del primo pacchetto disponibile
} BufferedModeAllocator;

