#define HC12H
#include "Arduino.h"
	
class HC12{
	public:
	HC12(HardwareSerial*, unsigned short);
	bool init();
	bool send(String);
	unsigned int receive();
	bool changeBaud(unsigned int);
	bool changePower(unsigned int);
	bool changeChannel(unsigned int);
	unsigned short stageChange(bool);
	String lastRead;
	
	private:
	const unsigned short commandPin;
	unsigned int power;
	unsigned int channel;
	unsigned int baud;
	char SerialByteIn;                              // Temporary variable
	char HC12ByteIn;                                // Temporary variable
	String HC12ReadBuffer;                     // Read/Write Buffer 1 for HC12
	String SerialReadBuffer;                   // Read/Write Buffer 2 for Serial
	boolean SerialEnd;                      // Flag to indicate End of Serial String
	boolean HC12End;                        // Flag to indiacte End of HC12 String
	boolean commandMode;                    // Send AT commands
	unsigned short newStage;
	HardwareSerial* serial;
	void sendCommand(String);
};
