#pragma once


//------ AA ---------//
//attendo approvazione da Paolo...

static const int NullIdx=-1;
static const int DetachedIdx=-2;

typedef enum {
	      Triggered = -1,
	      NotTriggered = -2
} BufferingStatus;

typedef struct BufferingAllocator{
  
  char* buffer;        //contiguous buffer managed by the system
  int*  free_list;     //list of linked objects
  int buffer_size;     //size of the buffer in bytes

  int size;            //number of free blocks
  int size_max;        //maximum number of blocks
  int item_size;       //size of a block
  
  int first_idx;       //pointer to the first bucket
  int bucket_size;     // size of a bucket

  BufferingStatus status;
} BuuferingAllocator;


BufferingAllocator BufferingAllocator_init(BufferingAllocator* allocator,
			int item_size,
			int num_items,
			char* memory_block,
			int memory_size);

void* BufferingAllocator_getBlock(BufferingAllocator* allocator);

int BufferingAllocator_releaseBlock(BufferingAllocator* allocator, void* block);

void BufferingAllocator_sendBlocks(BufferingAllocator* allocator, void* start_block, void* end_block);
