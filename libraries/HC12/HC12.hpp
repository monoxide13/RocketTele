	
class HC12{
	private:
	const unsigned short commandPin;
	unsigned int power;
	unsigned int channel;
	unsigned int baud;
	char SerialByteIn;                              // Temporary variable
	char HC12ByteIn;                                // Temporary variable
	String HC12ReadBuffer = "";                     // Read/Write Buffer 1 for HC12
	String SerialReadBuffer = "";                   // Read/Write Buffer 2 for Serial
	boolean SerialEnd = false;                      // Flag to indicate End of Serial String
	boolean HC12End = false;                        // Flag to indiacte End of HC12 String
	boolean commandMode = false;                    // Send AT commands
	const Stream * serial;
	void sendCommand(char *, unsigned int);
	
	public:
	HC12(Stream *, unsigned short);
	bool init();
	bool send(char *);
	unsigned int receive();
	bool changeBaud(unsigned int);
	bool changePower(unsigned int);
	bool changeChannel(unsigned int);
	
};