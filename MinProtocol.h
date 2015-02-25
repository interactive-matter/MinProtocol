#ifndef _MIN_PROTOCOL_H_
#define _MIN_PROTOCOL_H_
#include <inttypes.h>
#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif

#include "layer2.h"
	

/*  a lot of the high level logic is basically stolen from CmdMessenger v3 - see http://playground.arduino.cc/Code/CmdMessenger
	Perhaps not the best structure - but at least close to the above (what can be an advantage if you replace it)

	Adds an Arduino like Interface to the min protocol, trying to stick with layer 3 and reducing layer 2.

*/

typedef char* buffer_reference;

extern "C"
{
  // callback functions always follow the signature: void cmd(void);
  typedef void (*minCallbackFunction) (buffer_reference buf);
}
#define MIN_PROTOCOL_MAXCALLBACKS        50   // The maximum number of commands   (default: 50)
#define MIN_PROTOCOL_MESSENGERBUFFERSIZE 512   // The maximum length of the buffer (default: 64)

class MinProtocol {

public:
	MinProtocol(Stream & ccomms);

	void attach (minCallbackFunction newFunction);
  	boolean attach (byte msgId, minCallbackFunction newFunction);

  	void sendCmdStart (int cmdId);
  	//we have to unwrap the commands to unwrap them
	void sendCmdArg (char value) {
		if (m_cursor < m_control)  {
			m_buf[m_cursor] = (value);
			m_cursor++;
		}
	}
private:
	Stream* comms; //ths communication stream

	//buffer and pointers for sending commands
	char m_buf[MIN_PROTOCOL_MESSENGERBUFFERSIZE];
	uint8_t m_control;
	uint8_t m_cursor;
	uint8_t sending_cmd;

	//state stuff
	//TODO isnt' this  a bitfield?
	bool pause_processing;             // pauses processing of new commands, during sending

	minCallbackFunction default_callback;            // default callback function  
  	minCallbackFunction callbackList[MIN_PROTOCOL_MAXCALLBACKS];  // list of attached callback functions 

};

#endif