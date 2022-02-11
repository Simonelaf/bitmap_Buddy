#pragma once
#include "bit_map.h"

#define MAX_LEVELS 16

typedef struct  {
  BitMap bitmap;
  int buddy_levels;
  char* memory; // the buffer area to be managed
  int memory_size;
  int min_bucket_size; // the minimum page of RAM that can be returned
} BuddyAllocator;

// initializes the buddy allocator, and checks that the buffer is large enough
int BuddyAllocator_init(BuddyAllocator* buddy,
                         int buddy_levels,
                         char* memory, // buffer per l'allocator
                         int memory_size,
                         char* bitmap_buf, // buffer per la bitmap
                         int bitmap_buf_size,
                         int min_bucket_size);

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* buddy, int size);

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* buddy, void* mem);

// set the status of all parents
void set_parent(BitMap *bit_map, int bit_num, int status);

// set the status of all children
void set_children(BitMap *bit_map, int bit_num, int status);
