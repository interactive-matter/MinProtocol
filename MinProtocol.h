#ifndef _MIN_PROTOCOL_H_
#define _MIN_PROTOCOL_H_
#include <inttypes.h>
#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif
	

/* a lot of the high level logic is basically stolen from CmdMessenger v3 - see http://playground.arduino.cc/Code/CmdMessenger

	Adds an Arduino like Interface to the min protocol, trying to stick with layer 3 and reducing layer 2.

*/
class MinProtocol {
public:
	MinProtocol(Stream & comms)
};

#endif