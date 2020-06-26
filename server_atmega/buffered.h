#pragma once
#include "defs.h"

/* AA:  metodi per la gestione del buffered mode */

/* funzione che inizializza l'allocatore */
int BufferedModeAllocator_init(BufferedModeAllocator* bma, int pkg_size, int pkg_list_size, char* memory, int memory_size);

/* funzione che restituisce il primo blocco disponibile */
void* BufferedModeAllocator_getBlock(BufferedModeAllocator* allocator);

/* funzione che elimina dall'allocatore il blocco di memoria specificato */
int BufferedModeAllocator_releaseBlock(BufferedModeAllocator* allocator, void* block);
