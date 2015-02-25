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

typedef char* buffer_reference;

extern "C"
{
  // callback functions always follow the signature: void cmd(void);
  typedef void (*minCallbackFunction) (buffer_reference buf);
}
#define MIN_PROTOCOL_MAXCALLBACKS        50   // The maximum number of commands   (default: 50)

class MinProtocol {

public:
	MinProtocol(Stream & ccomms);

	void attach (minCallbackFunction newFunction);
  	boolean attach (byte msgId, minCallbackFunction newFunction);
private:
	Stream* comms; //ths communication stream

	minCallbackFunction default_callback;            // default callback function  
  	minCallbackFunction callbackList[MIN_PROTOCOL_MAXCALLBACKS];  // list of attached callback functions 

};

#endif