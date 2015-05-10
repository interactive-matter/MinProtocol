#include <MinProtocol.h>

const byte info = -100;
const unsigned long ping_ms = 1000;

void setup() {
  //initialize the first serial port
  Serial.begin(9600);
  //and use it for the min protocol
  Min.begin(Serial);

}

unsigned long last_now = millis();

void loop() {
  //create a time stamp
  unsigned long now = millis(); 
  int time_diff = now-last_now;
  if (time_diff>ping_ms) {
    Min.sendCmdStart(info);
    Min.sendCmdArg(now);
    Min.sendCmdArg(time_diff);
    Min.sendCmdEnd();
    last_now = now;
  }
  

}

