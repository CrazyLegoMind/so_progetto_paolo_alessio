#include <stdlib.h>
//#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include "buffered.h"
#include "defs.h"

//costante che comunica che si è arrivati alla fine del blocco di memoria disponibile
static const int NullIdx = -1; 
//costante che comunica che il blocco è stato rilasciato in precedenza
static const int DetachedIdx = -2;


int BufferedModeAllocator_init(BufferedModeAllocator* bma, int pkg_size, int pkg_list_size, char* memory, int memory_size) {
  //controllo memoria
  int requested_size= pkg_list_size*(pkg_size + sizeof(int));
  if(memory_size < requested_size)
    return -1;

  bma->pkg_size = pkg_size;
  bma->pkg_list_size = pkg_list_size;     
  bma->memory_size = pkg_size*pkg_list_size;  
  bma->memory = memory;
  bma->pkg_list = (int*)(memory + pkg_size*pkg_list_size); 

  //popolazione iniziale dei blocchi: indica la posizione del pacchetto nel buffer
  for (int i=0; i < bma->pkg_list_size-1; ++i)
    bma->pkg_list[i] = i+1;

  
  bma->pkg_list[bma->pkg_list_size-1] = NullIdx;
  bma->first_pkg = 0;

  return 1;
}

void* BufferedModeAllocator_getBlock(BufferedModeAllocator* bma) {
    //allocatore pieno    
  if (bma->first_pkg == NullIdx)    
    return NULL;

  //rimuovi il primo blocco della lista
  int detached_idx = bma->first_pkg;
  // aggiorno l'indice: trovo la prossima entry dummy
  bma->first_pkg = bma->pkg_list[bma->first_pkg];
  --(bma->pkg_list_size);

  //questo valore segnala che il blocco i-esimo è stato rilasciato
  bma->pkg_list[detached_idx] = DetachedIdx;
  
  //recupero il puntatore all'elemento del buffer
  char* block_address = bma->memory + (detached_idx*bma->pkg_size);
  return block_address;
}

int BufferedModeAllocator_releaseBlock(BufferedModeAllocator* bma, void* block_) {
  //trovo l'indice dall'indirizzo
  char* block = (char*)block_;
  int offset = block - bma->memory;

  //controllo che sono allineato al confine dei blocchi, altrimenti ritorno -1
  if (offset%bma->pkg_size)
    return -1;

  int idx = offset/bma->pkg_size;

  //controllo che sono nel buffer, altrimenti ritorno -1
  if (idx<0 || idx >= bma->memory_size)
    return -1;

  //controllo se il blocco è stato già rilasciato, altrimenti ritorno -1
  if (bma->pkg_list[idx] != DetachedIdx)
    return -1;

  //tutti i controlli superati, inserisco il blocco in testa
  bma->pkg_list[idx] = bma->first_pkg;
  bma->first_pkg = idx;
  ++bma->pkg_list_size;
  return 0;
}


///* AA debug main 
#define max_pkgs 10
const int buf_size = max_pkgs*(PKG_SIZE + sizeof(int));
BufferedModeAllocator all;

int main(int argc, char** argv) {
    char buf[buf_size];
    printf("Creazione SLAB allocator... ");
    int all_ok = BufferedModeAllocator_init(&all, PKG_SIZE, max_pkgs, buf, buf_size);
    if(!all_ok) {
      printf("Errore durante la creazione!\n");
      return EXIT_FAILURE;
    }
    printf("Done.\n");

    //test allocazione
    //int* random_signals[] = {34, 56, 12, 0, 5, 78, 256, 60, 1, 4};
    void* pkgs[max_pkgs];                                        //verrà definito nella parte host
    for(int i=0; i < max_pkgs; i++) {
      void* p = BufferedModeAllocator_getBlock(&all);
      pkgs[i] = p;
      printf("Allocazione %d: blocco %p, dimensione %d\n", i, p, all.pkg_list_size);
    }
    
    //test rilascio memoria
    for(int i=0; i < max_pkgs; i++) {
      void* p = pkgs[i];
      if(p) { 
        printf("Rilascio %d: blocco %p, dimensione %d\n", i, p, all.pkg_list_size);
        BufferedModeAllocator_releaseBlock(&all, p);
      }
    }
    printf("Test eseguito correttamente.\n");
    return EXIT_SUCCESS;  
}
//*/
