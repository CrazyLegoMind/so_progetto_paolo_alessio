#ifndef DEFS_H
#define DEFS_H

//AA: header per allineamento dati
#define HEADER "Oscilloscopio"
#define HEADER_SIZE strlen(HEADER)
#define MAX_DATA 27

typedef struct _data{
  uint32_t data_size;
  uint8_t data_type;
  uint8_t data[MAX_DATA];
} Data;



//AA: struttura pacchetto (11 bytes = 2816 bits)
#define TYPE_DATAPKG 1
typedef struct _data_pkg {
  uint8_t checksum;                // controllo integrità dati, generato dal server
  uint8_t data;                    // valore misurato
  uint8_t mask_pin;                // pin da dove si legge il valore
  uint8_t cmd;                     // codice per la gestione dei pacchetti
  uint8_t timestamp;               // intero che rappresenta l'epoca dei pacchetti
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
#define TYPE_INITPKG 0
typedef struct _init_pkg {
  uint8_t sampling_freq;            //frequenza di campionamento (in Hz)
  uint8_t channels;                 //quanti canali sono da leggere
  uint8_t mode;                     //continuous (0) o buffered (1)
  uint8_t time;                     //seconds
  int8_t trigger;
} InitPkg;


//AA: messaggio di debug per atmega
#define TYPE_TEXTPKG 2
typedef struct _text_pkg {
  uint8_t text_size; //max 20
  uint8_t text[23];
} TextPkg;

#define TYPE_EMPTYPKG 3

#endif
