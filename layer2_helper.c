/* 
 * Application layer (MIN layer 2)
 * 
 * Application-specific handling of data to and from the host.
 * 
 * Author: Ken Tindell
 * Copyright (c) 2014-2015 JK Energy Ltd.
 * Licensed under MIT License.
 */ 

#include <stdint.h>
#include "layer2_helper.h"

/* Worker functions to write words into a buffer in big endian format 
 * using generic C
 * 
 * A good C compiler will spot what is going on and use byte manipulation
 * instructions to get the right effect (functions declared static
 * to hint to compiler that code can be inlined)
 */
void _encode_64(uint64_t* data, uint8_t buf[])
{
	buf[0] = (uint8_t)((*data & 0xff00000000000000ULL) >> 56);
	buf[1] = (uint8_t)((*data & 0x00ff000000000000ULL) >> 48);
	buf[2] = (uint8_t)((*data & 0x0000ff0000000000ULL) >> 40);
	buf[3] = (uint8_t)((*data & 0x000000ff00000000ULL) >> 32);
	buf[4] = (uint8_t)((*data & 0x00000000ff000000ULL) >> 24);
	buf[5] = (uint8_t)((*data & 0x0000000000ff0000ULL) >> 16);
	buf[6] = (uint8_t)((*data & 0x000000000000ff00ULL) >> 8);
	buf[7] = (uint8_t)((*data & 0x00000000000000ffULL));
}

void _encode_32(uint32_t* data, uint8_t buf[])
{
	buf[0] = (uint8_t)((*data & 0xff000000UL) >> 24);
	buf[1] = (uint8_t)((*data & 0x00ff0000UL) >> 16);
	buf[2] = (uint8_t)((*data & 0x0000ff00UL) >> 8);
	buf[3] = (uint8_t)(*data & 0x000000ffUL);
}

void _encode_16(uint16_t* data, uint8_t buf[])
{
	buf[0] = (uint8_t)((*data & 0x0000ff00UL) >> 8);
	buf[1] = (uint8_t)(*data & 0x000000ffUL);
}

uint32_t _decode_32(uint8_t buf[])
{
	uint32_t res;
	
	res = 
		((uint32_t)(buf[0]) << 24) | 
	    ((uint32_t)(buf[1]) << 16) | 
	    ((uint32_t)(buf[2]) << 8) | 
	    (uint32_t)(buf[3]);
	
	return res;
} 

uint16_t _decode_16(uint8_t buf[])
{
	uint16_t res;
	
	res = 
		((uint16_t)(buf[0]) << 8) | 
		(uint16_t)(buf[1]);
	
	return res;
}   

float _decode_float(uint8_t buf[]) {
	uint32_t value = _decode_32(buf);
	return *((float*)&value);
}

	
