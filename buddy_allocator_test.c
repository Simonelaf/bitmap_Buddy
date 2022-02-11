#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 17 //<- LA MEMORIA PER LA BITMAP
#define BUDDY_LEVELS 6
#define MEMORY_SIZE 516//<- LA MEMORIA DA GESTIRE
#define MIN_BUCKET_SIZE (MEMORY_SIZE>>(BUDDY_LEVELS)) //BUCKET DI 8 -> MEMORY_SIZE : 2^BUDDY_LEVELS

char memory[MEMORY_SIZE]; 
char buffer[BUFFER_SIZE]; // spazio per la bitmap

BuddyAllocator alloc;
int main(int argc, char** argv) {

  // we initialize the allocator
  printf("init... \n");
  int init = BuddyAllocator_init(&alloc, 
                      BUDDY_LEVELS,
                      memory,  // memoria per l'allocatore
                      MEMORY_SIZE,
                      buffer,  // buffer per la bitmap
                      BUFFER_SIZE,
                      MIN_BUCKET_SIZE);
 
  if (init == 0) {
	  printf("[ERROR] Init failure.");
	  return 0;
	  } else printf("\nDONE\n");
	  
  printf("\n========[TEST DEL PROF.]========\n");
  
  int* p6=BuddyAllocator_malloc(&alloc, 96);
  int* p7=BuddyAllocator_malloc(&alloc, 96);
  int* p8=BuddyAllocator_malloc(&alloc, 999996); //questo fallisce
  BuddyAllocator_free(&alloc, p6);
  BuddyAllocator_free(&alloc, p7);
  BuddyAllocator_free(&alloc, p8);

  printf("\n========[TEST AGGIUNTIVI]========\n");

  int* p1=BuddyAllocator_malloc(&alloc, sizeof(int)*12);
  int* p2=BuddyAllocator_malloc(&alloc, 4);
  int* p3=BuddyAllocator_malloc(&alloc, sizeof(int)*9);
  int* p4=BuddyAllocator_malloc(&alloc, 4);
  int* p5=BuddyAllocator_malloc(&alloc, sizeof(int));
  
  BuddyAllocator_free(&alloc, p4);
  BuddyAllocator_free(&alloc, p5);
  BuddyAllocator_free(&alloc, p1);
  BuddyAllocator_free(&alloc, p2);
  BuddyAllocator_free(&alloc, p3); 

  int* p10=BuddyAllocator_malloc(&alloc, 508); // richiedo tutta la memoria
  int* p11=BuddyAllocator_malloc(&alloc, 8); // memoria piena
  BuddyAllocator_free(&alloc, p10);
  BuddyAllocator_free(&alloc, p11);

  printf("\n========[FINE DEI TEST]========\n");

}
