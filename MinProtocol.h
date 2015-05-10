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


/* Macros for unpacking and packing MIN frames in various functions.
 *
 * Declaring stuff in macros like this is not pretty but it cuts down
 * on obscure errors due to typos.
 */

#define DECLARE_UNPACK()		uint8_t m_cursor = 0
#define UNPACK8(v)				((m_cursor < m_control) ? ((v) = m_buf[m_cursor]), m_cursor++ : ((v) = 0))
#define UNPACK16(v)				((m_cursor + 2U <= m_control) ? ((v) = decode_16(m_buf + m_cursor)), m_cursor += 2U : ((v) = 0))
#define UNPACK32(v)				((m_cursor + 4U <= m_control) ? ((v) = decode_32(m_buf + m_cursor)), m_cursor += 4U : ((v) = 0))


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
  	void sendCmdArg(double value);

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