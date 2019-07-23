#include "HC12.hpp"

HC12::HC12(HardwareSerial* serialIn, unsigned short commandPinIn) : serial(serialIn), commandPin(commandPinIn) {
	HC12ReadBuffer.reserve(32);
	lastRead.reserve(8);
};

bool HC12::init(){
	serial->begin(9600);
	HC12ReadBuffer = "";                     // Read/Write Buffer 1 for HC12
	SerialReadBuffer = "";                   // Read/Write Buffer 2 for Serial
	SerialEnd = false;                      // Flag to indicate End of Serial String
	HC12End = false;                        // Flag to indiacte End of HC12 String
	commandMode = false;                    // Send AT commands
	lastHeartbeat=0;
	lastRead="";
	newStage=0;
};

void HC12::sendCommand(String command){
	serial->println(command);
	digitalWrite(commandPin, LOW);
	delay(200);
	serial->print(command);
	delay(200);
	digitalWrite(commandPin, HIGH);
};
bool HC12::send(String input){
	serial->println(input);
	
};
unsigned int HC12::receive(){
	int retVal=0;
	while (serial->available()) {
	  HC12ByteIn = serial->read();
	  if (HC12ByteIn == '\n'){                   // At the end of the line
		  if (HC12ReadBuffer.startsWith("AT")) {      // Check to see if a command is received from remote
			digitalWrite(commandPin, LOW);            // Enter command mode
			delay(100);                               // Delay before sending command
			serial->print(SerialReadBuffer);           // Echo command to HC12.
			delay(200);                               // Wait 0.2 s for reply
			digitalWrite(commandPin, HIGH);           // Exit command / enter transparent mode
			delay(100);                               // Delay before proceeding
			serial->println("Remote Command Executed");  // Acknowledge execution
			delay(10);
		  } else {
			  // It wasn't a local command. Set it as the ruturn value;
			  if(HC12ReadBuffer.length()>7){
				  HC12ReadBuffer="";
				  serial->println("BDTBDT");
			  }else{
				lastRead=HC12ReadBuffer;
			  }
		  }
		  HC12ReadBuffer = "";                        // Empty buffer
	  }
	  else
		HC12ReadBuffer += char(HC12ByteIn);         // Write each character of byteIn to HC12ReadBuffer
	}
	return retVal;
};
bool HC12::changeBaud(unsigned int in){
	char command[16];
	switch (in){
		case 2400:
			//command=String("AT+B2400");
			break;
	}
};
bool HC12::changePower(unsigned int in){
	//TODO: FIX  sendCommand(char * ptr, unsigned int length);
};
bool HC12::changeChannel(unsigned int in){
	String temp = String("AT+C" + in);
	sendCommand(temp);
};
unsigned short HC12::stageChange(bool resetVal){
	if(!resetVal)
		return newStage;
	unsigned short temp = newStage;
	newStage=0;
	return temp;
};