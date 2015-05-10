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

/* Configure the UART speed (see serial.h for binding to UART and FIFO buffer sizes) */
#define MIN_BAUD                        (UART_BAUD_9600)

/* Command frames from the host */
#define MIN_ID_PING						(0x02U)			/* Layer 1 frame; Ping test: returns the same frame back */
#define MIN_ID_MOTOR_REQUEST			(0x36u)			/* Layer 1 frame; Set requested motor position */

/* Report messages to the host */
#define MIN_ID_DEADBEEF					(0x0eU)			/* Layer 1 frame; Payload always 4 bytes = 0xdeadbeef */
#define MIN_ID_ENVIRONMENT				(0x23U)			/* Layer 1 frame; Temperature and humidity sensor values */
#define MIN_ID_MOTOR_STATUS				(0x24U)			/* Layer 1 frame; Report the status of the motor */

/* some nice encoder functions*/
void encode_64(uint64_t* data, uint8_t buf[]);

void encode_32(uint32_t* data, uint8_t buf[]);

void encode_16(uint16_t* data, uint8_t buf[]);

uint32_t decode_32(uint8_t buf[]);

uint16_t decode_16(uint8_t buf[]);

/* Macros for unpacking and packing MIN frames in various functions.
 *
 * Declaring stuff in macros like this is not pretty but it cuts down
 * on obscure errors due to typos.
 */

#define DECLARE_UNPACK()		uint8_t m_cursor = 0
#define UNPACK8(v)				((m_cursor < m_control) ? ((v) = m_buf[m_cursor]), m_cursor++ : ((v) = 0))
#define UNPACK16(v)				((m_cursor + 2U <= m_control) ? ((v) = decode_16(m_buf + m_cursor)), m_cursor += 2U : ((v) = 0))
#define UNPACK32(v)				((m_cursor + 4U <= m_control) ? ((v) = decode_32(m_buf + m_cursor)), m_cursor += 4U : ((v) = 0))

#endif /* LAYER2_H_ */
