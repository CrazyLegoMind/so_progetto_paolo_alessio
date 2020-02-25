#pragma once


//AA: struttura pacchetto
typedef struct _data_pkg {
  uint32_t* checksum;                // generato dal server
  uint16_t cmd;                      // codice per l'attivazione di una routine
  uint16_t mask_pin;                 // pin da dove si legge il dato
  uint8_t data_rate;                 // frequenza di lettura 
  uint16_t data;                     // valore misurato
  //int timestamp;
} DataPkg;

//AA: blocco di DataPkg basata su SLAB allocator
typedef struct _buffering_mode_allocator {
  char* memory;                      // memoria esterna messa a disposizione
  int memory_size;                   // quanta memoria è messa a disposizione
  int pkg_list_size;                 // quanti pacchetti contiene il blocco
  DataPkg* pkg_list;                 // lista di pacchetti da inviare in blocco
  int pkg_size;                      // grandezza in bytes di un singolo pacchetto
  int first_pkg;                     // indice del primo pacchetto disponibile
} BufferingModeAllocator;


//metodi
