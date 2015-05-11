/* Layer 2 definitions (application layer).
 * 
 * Defines all the MIN setup for the application, selecting IDs to be used
 * and how to pack and unpack signal data and commands.
 * 
 * Author: Ken Tindell
 * Copyright (c) 2014-2015 JK Energy Ltd.
 * Licensed under MIT License.
 */ 

#ifndef LAYER2_H_
#define LAYER2_H_

/* some nice encoder functions*/
void _encode_64(uint64_t* data, uint8_t buf[]);

void _encode_32(uint32_t* data, uint8_t buf[]);

void _encode_16(uint16_t* data, uint8_t buf[]);

uint32_t _decode_32(uint8_t buf[]);

uint16_t _decode_16(uint8_t buf[]);

float _decode_float(uint8_t buf[]);

#endif /* LAYER2_H_ */
