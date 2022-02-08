#include <assert.h>
#include <stdio.h>
#include "bit_map.h"

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits)
{
  return bits / 8 + ((bits % 8) != 0);
}

// initializes a bitmap on an external array
void BitMap_init(BitMap *bit_map, int num_bits, uint8_t *buffer)
{
	bit_map->buffer = buffer;
	bit_map->num_bits = num_bits;
	bit_map->buffer_size = BitMap_getBytes(bit_map->num_bits);
}

// sets a the bit bit_num in the bitmap
// status= 0 or 1
void BitMap_setBit(BitMap *bit_map, int bit_num, int status)
{
	// get byte
	int byte_num = bit_num >> 3;			 // divisione intero per 8
	assert(byte_num < bit_map->buffer_size); // die if byte_num > buffer_size
	int bit_in_byte = byte_num & 0x07;		 // int bit_in_byte = 7-(bit_num%8);
	if (status)
	{
		bit_map->buffer[byte_num] |= (1 << bit_in_byte);
	}
	else
	{
		bit_map->buffer[byte_num] &= ~(1 << bit_in_byte);
	}
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap *bit_map, int bit_num)
{
	int byte_num = bit_num >> 3;			 //(n°byte = n°bit/8)
	assert(byte_num < bit_map->buffer_size); // die if byte_num > buffer_size
	int bit_in_byte = byte_num & 0x07;		 // int bit_in_byte = 7-(bit_num%8);
	return (bit_map->buffer[byte_num] & (1 << bit_in_byte)) != 0;
}