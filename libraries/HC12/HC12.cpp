

#include "HC12.hpp"

HC12::HC12(Stream* serialIn, unsigned short commandPinIn) : serialIn(serial) : commandPinIn(commandPin) {
	HC12ReadBuffer.reserve(32);
};

bool HC12::init(){
	serial->start(9600);
};

void HC12::sendCommand(char * ptr, unsigned int length){
	digitalWrite(commandPin, LOW);
	delay(200);
	serial.write();
	delay(200);
	digitalWrite(commandPin, HIGH);
};
bool HC12::send(	 input){
	serial.println(input);
};
unsigned int HC12::receive(){
	retVal=0;
	if (serial.available()) {
    while (serial.available()) {
      HC12ByteIn = seral.read();
      if (HC12ByteIn == '\n')                   // At the end of the line
        HC12End = true;                           // Set HC12End flag to true
      else
        HC12ReadBuffer += char(HC12ByteIn);         // Write each character of byteIn to HC12ReadBuffer
    }
    if (HC12End) {                                // If HC12End flag is true
      if (HC12ReadBuffer.startsWith("AT")) {      // Check to see if a command is received from remote
        digitalWrite(HC12SetPin, LOW);            // Enter command mode
        delay(100);                               // Delay before sending command
        Serial.print(SerialReadBuffer);           // Echo command to serial if connected.
        delay(500);                               // Wait 0.5 s for reply
        digitalWrite(HC12SetPin, HIGH);           // Exit command / enter transparent mode
        delay(100);                               // Delay before proceeding
        Serial1.println("Remote Command Executed");  // Acknowledge execution
        while (Serial1.available()) {
          Serial1.write(Serial1.read());
        }
      } else {
		  // It wasn't a local command. Set it as the ruturn value;
      }
      HC12ReadBuffer = "";                        // Empty buffer
      HC12End = false;                            // Reset flag
    }
  }
  return retVal;
};
bool HC12::changeBaud(unsigned int in){
};
bool HC12::changePower(unsigned int in){
};
bool HC12::changeChannel(unsigned int in){
};