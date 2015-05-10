#include <stdint.h>

#include "MinProtocol.h"
extern "C"
{
    #include "min.h"
    #include "layer2_helper.h"
}
//we have a singleton protocol instance for now
MinProtocol Min = MinProtocol();
Stream* comms; //the global communication stream

MinProtocol::MinProtocol() {
    //nothing to do for now
}

void MinProtocol::begin(Stream & ccomms) {
	comms = &ccomms;
    min_init_layer1();
}

/**
 * Attaches an default function for commands that are not explicitly attached
 */
void MinProtocol::attach(minCallbackFunction newFunction)
{
    default_callback = newFunction;
    m_cmd_id=0;
}

/**
 * Attaches a function to a command ID
 */
boolean MinProtocol::attach(uint8_t cmd_id, minCallbackFunction newFunction)
{
    if (cmd_id >= 0 && cmd_id < MIN_PROTOCOL_MAXCALLBACKS) {
        callbackList[cmd_id] = newFunction;
        return true;
    } else {
    	return false;
    }
}

/**
 * Send start of command. This makes it easy to send multiple arguments per command
 */
void MinProtocol::sendCmdStart(uint8_t cmd_id)
{
    if (cmd_id && !m_cmd_id) {
		m_cmd_id   = cmd_id;
		pause_processing = true;
		m_control = MIN_PROTOCOL_MESSENGERBUFFERSIZE;
		m_cursor = 0;
	} else {
        //TODO error handling?!
    }
}

    //some methods to send various arguments
void MinProtocol::sendCmdArg(char value) {
    if (!m_cmd_id) {
        //TODO error handling
        return;
    }
    if (m_cursor < m_control) {
        m_buf[m_cursor] = (uint8_t)value;
        m_cursor++;
    } else {
        //TODO error handling?!
    }
}

void MinProtocol::sendCmdArg(int value) {
    if (!m_cmd_id) {
        //TODO error handling
        return;
    }
    if (m_cursor + 2U <= m_control) {
        encode_16((uint16_t*)&value, m_buf + m_cursor);
        m_cursor += 2U;    
    } else {
        //TODO error handling?!
    }
}

void MinProtocol::sendCmdArg(long value) {
    if (!m_cmd_id) {
        //TODO error handling
        return;
    }
    if (m_cursor + 4U <= m_control) {
        encode_32((uint32_t*)&value, m_buf + m_cursor);
        m_cursor += 4U;     
    } else {
        //TODO error handling?!
    }
}

void MinProtocol::sendCmdArg(float value) {
    if (!m_cmd_id) {
        //TODO error handling
        return;
    }
    if (m_cursor + 4U <= m_control) {
        encode_32((uint32_t*)&value, m_buf + m_cursor);
        m_cursor += 4U;     
    } else {
        //TODO error handling?!
    }
}

void MinProtocol::sendCmdArg(double value) {
    if (!m_cmd_id) {
        //TODO error handling
        return;
    }
    if (m_cursor + 8U <= m_control) {
        encode_64((uint64_t*)&value, m_buf + m_cursor);
        m_cursor += 8U;     
    } else {
        //TODO error handling?!
    }
}

void MinProtocol::sendCmdEnd() {
    min_tx_frame(m_cmd_id, m_buf, m_cursor);
    //todo do we check if the other side understood us?
    m_cmd_id   = 0;
    pause_processing = false;
    m_cursor = 0;
}

/* Called by Layer 2 when a byte is received from its UART handler */
void min_rx_byte(uint8_t byte);

/* Callback; ask Layer 2 to queue a byte into its UART handler */
void min_tx_byte(uint8_t byte);                                     

/* Callback; indicate to Layer 2 that a valid frame has been received */
void min_frame_received(uint8_t buf[], uint8_t control, uint8_t id);        

/* Callback; returns to MIN the space in the transmit FIFO */
uint8_t min_tx_space(void);



