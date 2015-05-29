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

//protoype for the dropped frame callback
void frame_dropped_callback();

MinProtocol::MinProtocol() {
    //clear the callback list
    default_callback=NULL;
    for (uint8_t i=0; i<MIN_PROTOCOL_MAXCALLBACKS; i++) {
        callback_list[i]=NULL;   
    }
}

void MinProtocol::begin(Stream & ccomms, bool send_fast_ack) {
	comms = &ccomms;
	ack_receive = send_fast_ack;
    min_frame_dropped_callback = frame_dropped_callback;
    min_init_layer1();
}

/**
 * Attaches an default function for commands that are not explicitly attached
 */
void MinProtocol::attach(minCallbackFunction newFunction)
{
    default_callback = newFunction;
}

/**
 * Attaches a function to a command ID
 */
boolean MinProtocol::attach(uint8_t cmd_id, minCallbackFunction newFunction)
{
    if (cmd_id >= 0 && cmd_id < MIN_PROTOCOL_MAXCALLBACKS) {
        callback_list[cmd_id] = newFunction;
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
void MinProtocol::sendCmdArg(unsigned char value) {
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

void MinProtocol::sendCmdArg(unsigned int value) {
    if (!m_cmd_id) {
        //TODO error handling
        return;
    }
    if (m_cursor + 2U <= m_control) {
        _encode_16((uint16_t*)&value, m_buf + m_cursor);
        m_cursor += 2U;    
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
        _encode_16((uint16_t*)&value, m_buf + m_cursor);
        m_cursor += 2U;    
    } else {
        //TODO error handling?!
    }
}

void MinProtocol::sendCmdArg(unsigned long value) {
    if (!m_cmd_id) {
        //TODO error handling
        return;
    }
    if (m_cursor + 4U <= m_control) {
        _encode_32((uint32_t*)&value, m_buf + m_cursor);
        m_cursor += 4U;     
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
        _encode_32((uint32_t*)&value, m_buf + m_cursor);
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
        _encode_32((uint32_t*)&value, m_buf + m_cursor);
        m_cursor += 4U;     
    } else {
        //TODO error handling?!
    }
}

/*
TODO this does not work - but we do not need it right now - find out how to make it work
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
*/

void MinProtocol::sendCmdEnd() {
    min_tx_frame(m_cmd_id, m_buf, m_cursor);
    //todo do we check if the other side understood us?
    m_cmd_id   = 0;
    pause_processing = false;
    m_cursor = 0;
}

/**
 * Feeds serial data over the MinProtocol
 */
void MinProtocol::feedinSerialData()
{
   uint8_t available_bytes = comms->available();
   if (available_bytes) {
        char rx_buffer[available_bytes];
        comms->readBytes(rx_buffer, available_bytes);
        for (uint8_t i=0; i<available_bytes; i++) {
            min_rx_byte(rx_buffer[i]);
        }
   }
}

uint16_t MinProtocol::decode_int(unsigned char* buffer) {
    return _decode_16(buffer);
}

uint32_t MinProtocol::decode_long(unsigned char* buffer) {
    return _decode_32(buffer);
}

float MinProtocol::decode_float(unsigned char* buffer) {
    return _decode_float(buffer);
}

/* Callback; ask Layer 2 to queue a byte into its UART handler */
void min_tx_byte(uint8_t byte) {
    comms->write(byte);
}                                     

/* Callback; indicate to Layer 2 that a valid frame has been received */
void min_frame_received(uint8_t buf[], uint8_t control, uint8_t id) {
    if (Min.callback_list[id]!=NULL) {
        if (Min.ack_receive) {
            Min.sendCmdStart(FRAME_ACK_PACKAGE_ID);
            Min.sendCmdArg(id);
            Min.sendCmdEnd();
        }
        Min.callback_list[id](id, buf, control);
    } else if (Min.default_callback!=NULL) {
        Min.default_callback(id, buf, control);
    } else {
        //TODO error handling
    }
}        

void frame_dropped_callback() {
    //send a frame to signal that something went wrong
    Min.sendCmdStart(FRAME_DROPPED_PACKAGE_ID);
    Min.sendCmdEnd();
}


