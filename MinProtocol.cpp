#include <stdio.h>

#include "MinProtocol.h"


MinProtocol::MinProtocol(Stream & ccomms) {
	comms = &ccomms;
}

/**
 * Attaches an default function for commands that are not explicitly attached
 */
void MinProtocol::attach(minCallbackFunction newFunction)
{
    default_callback = newFunction;
    sending_cmd=0;
}

/**
 * Attaches a function to a command ID
 */
boolean MinProtocol::attach(byte msgId, minCallbackFunction newFunction)
{
    if (msgId >= 0 && msgId < MIN_PROTOCOL_MAXCALLBACKS) {
        callbackList[msgId] = newFunction;
        return true;
    } else {
    	return false;
    }
}

/**
 * Send start of command. This makes it easy to send multiple arguments per command
 */
void MinProtocol::sendCmdStart(int cmdId)
{
    if (!sending_cmd) {
		sending_cmd   = cmdId;
		pause_processing = true;
		comms->print(cmdId);
		m_control = MIN_PROTOCOL_MESSENGERBUFFERSIZE;
		m_cursor = 0;
	}
}
