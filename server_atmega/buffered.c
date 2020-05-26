#include <unistd.h>
#include <fnctl.h>
#include "buffered.h"
#include "defs.h"

//costante che comunica che si è arrivati alla fine del blocco di memoria disponibile
static const int NullIdx = -1; 
//costante che comunica che il blocco è stato rilasciato
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

  //popolazione dei blocchi: inizializza a 0 (clear bits)
  for (int i=0; i < bma->pkg_list_size-1; ++i)
    bma->pkg_list[i] = 0;

  
  bma->pkg_list[ba->size] = NullIdx;
  bma->first_idx = 0;

  return 1;
}

void* BufferedModeAllocator_getBlock(BufferedModeAllocator* bma) {
  //if (bma->first_idx == NullIdx)
  if (bma->first_idx == NULL)    
    return 0;

  //rimuovi il primo blocco della lista
  int detached_idx = bma->first_idx;
  // aggiorno l'indice
  bma->first_idx = bma->pkg_list[bma->first_idx];
  --(bma->pkg_list_size);

  //questo valore segnala che il blocco i-esimo è stato rilasciato
  bma->pkg_list[detached_idx] = DetachedIdx;
  
  //recupero il puntatore all'elemento del buffer
  char* block_address = bma->memory + (detached_idx*bma->pkg_size);
  return block_address;
}

int BufferedModeAllocator_releaseBlock(BufferedModeAllocator* bma, void* block_) {
  //trovo l'indice dall'indirizzo
  char* block = (char*) block_;
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
  bma->pkg_list[idx] = bma->first_idx;
  bma->first_idx = idx;
  ++bma->memory_size;
  return 0;
}
