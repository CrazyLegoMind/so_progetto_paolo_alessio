#pragma once
#include "packets.h"

/* AA:  metodi per la gestione del buffered mode */

/* funzione che inizializza l'allocatore */
int BufferedModeAllocator_init(BufferingModeAllocator* bma, int pkg_size, int pkg_list_size, char* memory, int memory_size);

/* funzione che restituisce il primo blocco disponibile */
void* BufferedModeAllocator_getBlock(BufferingModeAllocator* allocator);

/* funzione che elimina dall'allocatore il blocco di memoria specificato */
int BufferedModeAllocator_releaseBlock(BufferingModeAllocator* allocator, void* block);

/* funzione che invia una parte di memoria (invoca trigger) */
/*
void BufferedAllocator_sendBlocks(BufferingModeAllocator* allocator, void* start_block, void* end_block, const char* serial_path);
//oppure fai una funzione che invia un bucket alla volta
*/
