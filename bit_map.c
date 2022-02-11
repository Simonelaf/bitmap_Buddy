#include <assert.h>
#include <stdio.h>
#include "bit_map.h"

// numero di Bytes necessari a contenere i nodi della bitmap
int BitMap_getBytes(int bits){
    int i = 0;
    if ((bits%8)!=0) i = 1;
  return bits/8 + i;
}

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, char* buffer){
  bit_map->buffer=buffer;
  bit_map->num_bits=num_bits;
  bit_map->buffer_size=BitMap_getBytes(num_bits);
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_num, int status){
  // get byte
  int byte_num=bit_num>>3;
  assert(byte_num<bit_map->buffer_size);
  int bit_in_byte=bit_num&0x07;
  if (status) {
    bit_map->buffer[byte_num] |= (1<<bit_in_byte);
  } else {
    bit_map->buffer[byte_num] &= ~(1<<bit_in_byte);
  }
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num){
  int byte_num=bit_num>>3; //(n°byte = n°bit/8)
  assert(byte_num<bit_map->buffer_size);
  int bit_in_byte=bit_num&0x07; //(bit_num%8)
  return (bit_map->buffer[byte_num] & (1<<bit_in_byte))!=0;
}

void Bitmap_print(BitMap *bit_map){
	printf("\n[STATO PARZIALE DELLA BITMAP]\n");
  int p = 0;
  int current_lvl = -1;
  for (int i = 0; i < bit_map->num_bits; i++){
    if (p == 0){ 
      printf("\n");
      current_lvl++;
      for (int j = 0; j < (1 << 6) - (1 << current_lvl); j++) printf(" ");
      p = 1 << current_lvl; // (2^current_lvl)
    }
    printf("%d ", BitMap_bit(bit_map, i));
    p--;
  }
  printf("\n");
}
