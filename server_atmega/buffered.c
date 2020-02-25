#include <uinstd.h>
#include <fnctl.h>
#include "buffered.h"


void BufferingAllocator_init(BufferingAllocator* ba,
		       int item_size,
		       int num_items,
		       char* memory_block,
		       int memory_size) {

  //controllo memoria
  int requested_size= num_items*(item_size+sizeof(int));
  if (memory_size<requested_size)
    return -1;

  ba->item_size = item_size;
  ba->size = num_items;
  ba->buffer_size = item_size*num_items;
  ba->size_max = num_items;
  
  ba->buffer=memory_block;
  ba->free_list= (int*)(memory_block+item_size*num_items); 

  //popolazione dei blocchi
  for (int i=0; i < ba->size-1; ++i){
    ba->free_list[i]=i+1;
  }
  ba->free_list[ba->size] = NullIdx;
  ba->status = NotTriggered;
  ba->first_idx=0;
}

void* PoolAllocator_getBlock(PoolAllocator* a) {
  if (ba->first_idx==-1)
    return 0;

  //rimuovi il primo blocco della lista
  int detached_idx = ba->first_idx;
  // aggiorno l'indice
  ba->first_idx=ba->free_list[ba->first_idx];
  --ba->size;
  
  ba->free_list[detached_idx] = DetachedIdx;
  
  //recupero il puntatore all'elemento del buffer
  char* block_address=ba->buffer+(detached_idx*ba->item_size);
  return block_address;
}

int PoolAllocator_releaseBlock(BufferingAllocator* ba, void* block_){
  //trovo l'indice dall'indirizzo
  char* block=(char*) block_;
  int offset=block - ba->buffer;

  //controllo che sono allineato al confine dei blocchi
  if (offset%ba->item_size)
    return -1;

  int idx=offset/ba->item_size;

  //controllo che sono nel buffer
  if (idx<0 || idx>=ba->size_max)
    return -1;

  //controllo se il blocco è stato rilasciato
  if (ba->free_list[idx] != DetachedIdx)
    return -1;

  //tutti i controlli superati, inserisco il blocco in testa
  ba->free_list[idx]=ba->first_idx;
  ba->first_idx=idx;
  ++ba->size;
  return 0;
}

int BufferingAllocator_sendBlock(BUfferingAllocator* ba, void* block_, const char* serial_path) {
  //invio i dati alla seriale, quindi al PC, e salvali sul file (predefinito)
  if(ba->status == NotTriggered) {
    fprintf("Send block not allowed.\n");
    return -1;
  }
  
  int fd = open(serial_path, O_RDWR | O_NONBLOCK | o_NOCTTY);
  if(!fd) {
    fprintf(stderr,"Error opening serial device.\n");
    return -1;
  }
  //AA: forse da usare la libreria termios per gestire la seriale
  

  char* block = (char*)block_;
  //invialo alla seriale tramite write
  //da controllare che invia tutti i byte
  ssize_t sent = write(fd, block, sizeof(block));

  
  /*
  if(!ret) {
    fprintf(stderr,"Error on sending block info.\n");
    return -1;
  }
  */
  return !(sent==sizeof(block)); 
}
