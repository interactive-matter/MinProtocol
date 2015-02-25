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
