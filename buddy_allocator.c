#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation

//dato un index restituisce il livello a cui si trova
int levelIdx(size_t idx){ 
  return (int)floor(log2(idx+1));//floor arrotonda al numero più vicino
}


