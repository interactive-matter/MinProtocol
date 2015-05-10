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
void encode_64(uint64_t* data, uint8_t buf[])
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

void encode_32(uint32_t* data, uint8_t buf[])
{
	buf[0] = (uint8_t)((*data & 0xff000000UL) >> 24);
	buf[1] = (uint8_t)((*data & 0x00ff0000UL) >> 16);
	buf[2] = (uint8_t)((*data & 0x0000ff00UL) >> 8);
	buf[3] = (uint8_t)(*data & 0x000000ffUL);
}

void encode_16(uint16_t* data, uint8_t buf[])
{
	buf[0] = (uint8_t)((*data & 0x0000ff00UL) >> 8);
	buf[1] = (uint8_t)(*data & 0x000000ffUL);
}

uint32_t decode_32(uint8_t buf[])
{
	uint32_t res;
	
	res = 
		((uint32_t)(buf[0]) << 24) | 
	    ((uint32_t)(buf[1]) << 16) | 
	    ((uint32_t)(buf[2]) << 8) | 
	    (uint32_t)(buf[3]);
	
	return res;
} 

uint16_t decode_16(uint8_t buf[])
{
	uint16_t res;
	
	res = 
		((uint16_t)(buf[0]) << 8) | 
		(uint16_t)(buf[1]);
	
	return res;
}   

	
/* Functions called by the application to report information via MIN frames 

This is done in LEvel 3 - so here are some examples how that could look like:

Report the current state of the environment 
void report_environment(uint16_t temperature, uint16_t humidity)
{
	DECLARE_BUF(4);
		
	PACK16(temperature);
	PACK16(humidity);
	
	SEND_FRAME(MIN_ID_ENVIRONMENT);
}

// Report the current motor position, including a status 
void report_motor(uint8_t status, uint32_t position)
{
	DECLARE_BUF(5);
	
	PACK8(status);
	PACK32(position);
	
	SEND_FRAME(MIN_ID_MOTOR_STATUS);
}

// Report 0xdeadbeef as a heartbeat signal 
void report_deadbeef(uint32_t deadbeef)
{
	DECLARE_BUF(4);
	
	PACK32(deadbeef);
	
	SEND_FRAME(MIN_ID_DEADBEEF);
}

/* Functions to unpack incoming MIN frames into application-specific data.
 
/* Example of how a MIN frame send from the host would be used to issue
 * a motor command to move to a given position at a given speed.
 * 
 * The motor_requested flag could be polled to know when to pick up the
 * position/speed setting for the motor.
 
uint32_t motor_position_request;
uint16_t motor_speed_request;
uint8_t motor_requested;

static void do_motor(uint8_t m_id, uint8_t m_buf[], uint8_t m_control)
{
	DECLARE_UNPACK();
	UNPACK32(motor_position_request);
	UNPACK16(motor_speed_request);
	motor_requested = 1U;
}

/* Other application-specific frame-decode functions would be added here.

/* Provides a 'ping' service by echoing back the ping frame with
 * the same ID and payload.
static void do_ping(uint8_t m_id, uint8_t m_buf[], uint8_t m_control)
{
	min_tx_frame(m_id, m_buf, m_control);
}

/* Main function to process incoming bytes and pass them into MIN layer 
void poll_rx_bytes(void)
{	
	/* Handle all the outstanding characters in the input buffer 
	while(uart_receive_ready()) {
		uint8_t byte;
		uart_receive(&byte, 1U);
		min_rx_byte(byte);
	}
}

/* Callback from MIN layer 1 to indicate the frame has been received
void min_frame_received(uint8_t buf[], uint8_t control, uint8_t id)
{	
    switch(id) {
        case MIN_ID_PING:
            do_ping(id, buf, control);
            break;
        case MIN_ID_MOTOR_REQUEST:
            do_motor(id, buf, control);
            break;
        /* Other IDs would be handled in this case statement, calling the
         * application-specific functions to unpack the frames into
         * application data for passing to the rest of the application
         
    }
}

/* Callback from MIN to send a byte over the UART (in this example, queued in a FIFO) 
void min_tx_byte(uint8_t byte)
{
	/* Ignore FIFO overrun issue - don't send frames faster than the FIFO can handle them
	 * (and make sure the FIFO is big enough to take a maximum-sized MIN frame).
	uart_send(&byte, 1U);
}

/* Callback from MIN to see how much transmit buffer space there is 
uint8_t min_tx_space(void)
{
	return uart_send_space();
}

void init_min(void)
{
	/* Set MIN Layer 0 settings of 8 data bits, 1 stop bit, no parity 
	init_uart(MIN_BAUD);
    min_init_layer1();
}
*/
