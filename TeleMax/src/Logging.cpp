///// Logging.cpp /////

#include "Logging.hpp"
#include "SD.h"
#include "RH_RF95.h"
#include <cmath>

#define LOGGING_STATUS_MASK_UNUSED			B11000000
#define LOGGING_STATUS_MASK_DOWNLINK		B00110000
#define LOGGING_STATUS_MASK_SD				B00001100
#define LOGGING_STATUS_MASK_USB				B00000011
#define LOGGING_STATUS_OFFSET_UNUSED(x)		x<<6
#define LOGGING_STATUS_OFFSET_DOWNLINK(x)	x<<4
#define LOGGING_STATUS_OFFSET_SD(x)			x<<2
#define LOGGING_STATUS_OFFSET_USB(x)		x<<0

#define TX_BUFFER_SIZE 128
#define TX_POWER 2 // 2-20

namespace {
	char loggingStatus;
	bool ready; // A faster temp way to know if files open.
	File logFile;
	#if LOG_DOWNLINK > 0
	char sendBuffer[TX_BUFFER_SIZE];
	#endif
	RH_RF95* downlink;
	// This must be defined like this so the telemetryPacket can be accessed on 2&4 byte boundaries. If not, causes hard fault and stops MCU. See adafruits write-up on aligned memory access. It should work on 16-bit boundary, but no reason to risk it.
	uint8_t * downlinkPacket = (uint8_t*) new uint32_t[(int)ceil(TELEMETRY_PACKET_LENGTH/4)+2];
}

union Telemetry_Packet * Logging::telemetryData;

void Logging::init(){
	// Set downlink packet to ptr on 16bit boundary.
	Logging::telemetryData = (union Telemetry_Packet *)(downlinkPacket + 4);
	downlinkPacket[2]='T';
	downlinkPacket[3]=':';
	downlinkPacket[TELEMETRY_PACKET_LENGTH+4]='\n'; // 4 initial bits, then packet, then newline.
	Telemetry_Packet_Init(Logging::telemetryData);
	ready=false;
	loggingStatus=0;
	#if LOG_USB > 0
	Serial.begin(115200);
	//while (!Serial.available()){ delay (1);};
	Serial.println("-USB Debugging! Starting system...");
	loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_USB | LOGGING_STATUS_OFFSET_USB(3); 
	#endif
	
	#if LOG_SD > 0
	openFile();
	#endif

	#if LOG_DOWNLINK > 0
	Serial.println("-Downlink init starting");
	downlink = new RH_RF95(RF95_CS_PIN, RF95_IRQ0_PIN, hardware_spi);
	Serial.println("-Downlink init");
	if(downlink->init()){
		#if LOG_USB > 0
		Serial.print("-Getting RF95 version: ");
		Serial.println((uint8_t)downlink->getDeviceVersion()); // Should return 18
		#endif
		loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_DOWNLINK | LOGGING_STATUS_OFFSET_DOWNLINK(3);
		downlink->setModemConfig(RH_RF95::Bw500Cr45Sf128);
		// Throughput is 977bps @ -138dbm
		downlink->setSpreadingFactor(10);
		downlink->setSignalBandwidth(125000);
		downlink->setCodingRate4(5);
		downlink->setTxPower(TX_POWER); // Valid values 2-20
		downlink->setModeRx(); // Start listening.
		//downlink->setCADTimeout(0);
		#if LOG_USB > 0
		Logging::log(2, "-Downlink Started.\n");
		#endif
	}else{
		loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_DOWNLINK | LOGGING_STATUS_OFFSET_DOWNLINK(2);
		#if LOG_USB > 0
		Logging::log(2, "-Downlink init failed!\n");
		#endif
	}
	#endif
};

void Logging::openFile(){

	if(!digitalRead(SD_CD_PIN)){
		loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_SD | LOGGING_STATUS_OFFSET_SD(0); 
		Logging::log(2, "-SD Card not present\n");
		return;
	}
	loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_SD | LOGGING_STATUS_OFFSET_SD(1); 
	Logging::log(2, "-SD Card present\n");
	if(SD.begin(SD_CS_PIN)){
		Logging::log(2, "-SD Card opened.\n");
	}else{
		loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_SD | LOGGING_STATUS_OFFSET_SD(2);
		Logging::log(2, "-SD Card unable to be opened\n");
		return;
	}
	unsigned short trialCount=0;
	char fileName[] = EXPANDSTRING(FILENAME_PREFIX) "00.log\0";
	while(SD.exists(fileName)){
			sprintf(fileName, EXPANDSTRING(FILENAME_PREFIX) "%02d.log\0", ++trialCount);
	}
	Logging::log(1, "-Using filename: " + String(fileName) + " - ");
	logFile = SD.open(fileName, FILE_WRITE);
	if(!logFile){ // Unable to open file
		loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_SD | LOGGING_STATUS_OFFSET_SD(2);
		Logging::log(1, "-Unable to open file.\n");
		return;
	}
	loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_SD | LOGGING_STATUS_OFFSET_SD(3);
	Logging::log(1, "-File opened.\n");
	ready=true;
};

void Logging::closeFile(){
	logFile.close();
};

void Logging::log(unsigned char level, String input){
	#if LOG_USB > 0
	if( level <= LOG_USB & !(~loggingStatus & LOGGING_STATUS_MASK_USB)){
		Serial.print(input);
	}
	#endif
	#if LOG_SD > 0
	if( level <= LOG_SD & !(~loggingStatus & LOGGING_STATUS_MASK_SD)){
		logFile.print(input);
	}
	#endif
	#if LOG_DOWNLINK > 0
	if( level <= LOG_DOWNLINK & !(~loggingStatus & LOGGING_STATUS_MASK_DOWNLINK)){
		input.toCharArray(sendBuffer, input.length()+1); // +1 required to allow space for null terminator which will be trimmed off later.
		downlink->send((uint8_t*)sendBuffer, input.length());
		//Serial.println("=Downlink Sent 1. Length: " + String(input.length()));
	}
	#endif

}

void Logging::log(unsigned char level, char* input, unsigned short length){
	#if LOG_USB > 0
	if( level <= LOG_USB & !(~loggingStatus & LOGGING_STATUS_MASK_USB)){
		Serial.write(input, length);
		/* // Prints out data in HEX.
		int x;
		for(x=0; x<length; ++x){
			if(input[x]<0x10)
				Serial.print('0');
			Serial.print(input[x], HEX);
			Serial.print(" ");
		}
		// */	
	}
	#endif
	#if LOG_SD > 0
	if( level <= LOG_SD & !(~loggingStatus & LOGGING_STATUS_MASK_SD)){
		logFile.write(input, length);
	}
	#endif
	#if LOG_DOWNLINK > 0
	if( level <= LOG_DOWNLINK & !(~loggingStatus & LOGGING_STATUS_MASK_DOWNLINK)){
		downlink->send((uint8_t *)input, length);
		//Serial.println("=Downlink Sent 2. Length: " + String(length));
	}
	#endif
};

void Logging::flush(){
	#if LOG_USB > 0
	Serial.flush();
	#endif
	#if LOG_SD > 0
	logFile.flush();
	#endif
	#if LOG_DOWNLINK > 0
	downlink->waitPacketSent(); // Possibly time intensive
	#endif
};

void Logging::sendTelemetry(){
	#if LOG_USB > 0
	/* // Prints out data in HEX.
	int length=TELEMETRY_PACKET_LENGTH+3;
	int x;
	for(x=2; x<length+2; ++x){
		if(downlinkPacket[x]<0x10)
			Serial.print('0');
		Serial.print(downlinkPacket[x], HEX);
		Serial.print(" ");
	}
	Serial.println();
	// */
	#endif
	#if LOG_DOWNLINK > 0
	downlink->send(downlinkPacket+2, (uint8_t)TELEMETRY_PACKET_LENGTH+3);
	#endif
}

bool Logging::loggingReady(){
	return ready;
};
