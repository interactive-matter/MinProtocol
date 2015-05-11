#ifndef _MIN_PROTOCOL_H_
#define _MIN_PROTOCOL_H_
#include <inttypes.h>
#if ARDUINO >= 100
#include <Arduino.h> 
#else
#include <WProgram.h> 
#endif

/*  a lot of the high level logic is basically stolen from CmdMessenger v3 - see http://playground.arduino.cc/Code/CmdMessenger
	Perhaps not the best structure - but at least close to the above (what can be an advantage if you replace it)

	Adds an Arduino like Interface to the min protocol, trying to stick with layer 3 and reducing layer 2.

*/
#define FRAME_DROPPED_PACKAGE_ID 0xff


extern "C"
{
  typedef byte* buffer_reference;
  // callback functions always follow the signature: void cmd(void);
  typedef void (*minCallbackFunction) (uint8_t id, buffer_reference buf, uint8_t buf_length);
}

#define MIN_PROTOCOL_MAXCALLBACKS        50   // The maximum number of commands   (default: 50)
#define MIN_PROTOCOL_MESSENGERBUFFERSIZE 255   // The maximum length of the buffer (default: 64)

class MinProtocol {

public:
	MinProtocol();

	void begin(Stream & ccomms);

	void attach (minCallbackFunction newFunction);
  	boolean attach (uint8_t m_cmd_id, minCallbackFunction newFunction);

  	//Initiate a command
  	void sendCmdStart(uint8_t m_cmd_id);

  	//some methods to send various arguments
  	void sendCmdArg(unsigned char value);
  	void sendCmdArg(char value);
  	void sendCmdArg(unsigned int value);
  	void sendCmdArg(int value);
  	void sendCmdArg(unsigned long value);
  	void sendCmdArg(long value);
  	void sendCmdArg(float value);
    //todo buggy, repair f needed
  	//void sendCmdArg(double value);

  	//finish the command
  	void sendCmdEnd();

  	//call this to feed in data from the serial port
  	void feedinSerialData();

	minCallbackFunction default_callback;            // default callback function  
  	minCallbackFunction callback_list[MIN_PROTOCOL_MAXCALLBACKS];  // list of attached callback functions 

private:
	//the id of the command to send
	uint8_t m_cmd_id;
	//buffer and pointers for sending commands
	uint8_t m_buf[MIN_PROTOCOL_MESSENGERBUFFERSIZE];
	uint8_t m_control;
	uint8_t m_cursor;

	//state stuff
	//TODO isnt' this  a bitfield?
	bool pause_processing;             // pauses processing of new commands, during sending


};

//we have a singleton protocol instance for now
extern MinProtocol Min;

#endif