#include <MinProtocol.h>

const byte info = 0xf0;
const byte digital_read = 0x10;
const byte pin_mode = 0x11;
const byte data_error = 0xff;
const byte encode_error = 0xff;
const unsigned long ping_ms = 60l*1000l;

//a nivce error function
void sendError(uint8_t id, int8_t error_code) {
  Min.sendCmdStart(data_error);
  Min.sendCmdArg(id);
  Min.sendCmdArg(error_code);
  Min.sendCmdEnd();
} 
//callbacks
void defaultCallback(uint8_t id, buffer_reference buf, uint8_t buf_length) {
  Serial.write(0xff);
  Serial.write(id);
  for (uint8_t i=0; i<buf_length; i++) {
    Serial.write(buf[i]);
  }
  Serial.write(0xff);
}

void pinModeCallback(uint8_t id, buffer_reference buf, uint8_t buf_length) {
  if (buf_length>1) {
    char pin = buf[0];
    if (0<= pin && pin<=18) {
      char mode = buf[1];
      char result;
      if (mode > 0) {
        pinMode(pin,OUTPUT);
        result = 1;
      } 
      else if (mode < 0) {
        pinMode(pin,INPUT_PULLUP);
        result = -1;
      } 
      else {
        pinMode(pin,INPUT);
        result = 0;
      }
      Min.sendCmdStart(pin_mode);
      Min.sendCmdArg(pin);
      Min.sendCmdArg(result);
      Min.sendCmdEnd();    
    } 
    else {
      sendError(digital_read,1);
    }
  }
}

void digitalReadCallback(uint8_t id, buffer_reference buf, uint8_t buf_length) {
  if (buf_length>0) {
    char pin = buf[0];
    if (0<= pin && pin<=18) {
      char result = digitalRead(pin);
      Min.sendCmdStart(digital_read);
      Min.sendCmdArg(pin);
      Min.sendCmdArg(result);
      Min.sendCmdEnd();    
    } 
    else {
      sendError(digital_read,1);
    }
  } 
  else {
    sendError(digital_read,0);
  }
}

void setup() {
  //initialize the first serial port
  Serial.begin(9600);
  //and use it for the min protocol
  Min.begin(Serial);
  //register the callbacks
  Min.attach(&defaultCallback);
  Min.attach(digital_read, digitalReadCallback);
  Min.attach(pin_mode, pinModeCallback);
}

unsigned long last_now = millis();

void loop() {
  //create a time stamp
  unsigned long now = millis(); 
  unsigned int time_diff = now-last_now;
  if (time_diff>=ping_ms) {
    Min.sendCmdStart(info);
    Min.sendCmdArg(now);
    Min.sendCmdArg(time_diff);
    Min.sendCmdEnd();
    last_now = now;
  }
  Min.feedinSerialData();


}



