/*
 * Microcontroller Interconnect Network (MIN) Version 1.0
 * 
 * Layer 1 C API.
 * 
 * Author: Ken Tindell
 * Copyright (c) 2014-2015 JK Energy Ltd.
 * Licensed under MIT License.
 */
#ifndef MIN_H_
#define MIN_H_

#include <stdint.h>

/* Maximum size of a MIN frame:
 *
 * 3 (header) + 1 (id) + 1 (control) + 15 (payload) + 2 (checksum) + 1 (EOF) + stuff bytes
 */
#define MAX_FRAME_PAYLOAD_SIZE		(256U)

/* Initialize Layer 1 */
void min_init_layer1();

/* Called by Layer 2 to transmit a MIN Layer 1 frame */
void min_tx_frame(uint8_t id, uint8_t payload[], uint8_t length);

/* Called by Layer 2 when a byte is received from its UART handler */
void min_rx_byte(uint8_t byte);

/* Callback; ask Layer 2 to queue a byte into its UART handler */
void min_tx_byte(uint8_t byte);										

/* Callback; indicate to Layer 2 that a valid frame has been received */
void min_frame_received(uint8_t buf[], uint8_t control, uint8_t id);		

/* Callback; ask Layer 2 to handle an error frame - can be left at NULL*/
typedef void (*min_frame_dropped_function) ();
extern min_frame_dropped_function min_frame_dropped_callback;

#endif /* MIN_H_ */
