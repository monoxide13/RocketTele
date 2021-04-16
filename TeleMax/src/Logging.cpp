///// Logging.cpp /////

#include "Logging.hpp"
#include "SD.h"
#include "RH_RF95.h"

#define LOGGING_STATUS_MASK_UNUSED			B11000000
#define LOGGING_STATUS_MASK_DOWNLINK		B00110000
#define LOGGING_STATUS_MASK_SD				B00001100
#define LOGGING_STATUS_MASK_USB				B00000011
#define LOGGING_STATUS_OFFSET_UNUSED(x)		x<<6
#define LOGGING_STATUS_OFFSET_DOWNLINK(x)	x<<4
#define LOGGING_STATUS_OFFSET_SD(x)			x<<2
#define LOGGING_STATUS_OFFSET_USB(x)		x<<0

namespace {
	char loggingStatus;
	bool ready; // A faster temp way to know if files open.
	File logFile;
	#if LOG_DOWNLINK > 0
	RH_RF95* downlink;
	char sendBuffer[64];
	#endif
}

void Logging::init(){
	ready=false;
	loggingStatus=0;
	sendBuffer[0]='\0';
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
	Serial.println("Starting downlink init");
	downlink = new RH_RF95(RF95_CS_PIN, RF95_IRQ0_PIN);
	Serial.println("Downlink opened");
	if(downlink->init()){
		Serial.print("-Getting RF95 version -"); // Not sure what this will return.
		Serial.println((uint8_t)downlink->getDeviceVersion()); // Not sure what this will return.
		loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_DOWNLINK | LOGGING_STATUS_OFFSET_DOWNLINK(3);
		#if LOG_USB > 0
			Serial.println("-Downlink OK");
		#endif
		downlink->setTxPower(2); // Valid values 2-20
		downlink->setModeRx(); // Start listening.
	}else{
		loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_DOWNLINK | LOGGING_STATUS_OFFSET_DOWNLINK(2);
		Serial.println("Downlink init failed!");
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
		Logging::log(1, "-SD Card opened.\n");
	}else{
		loggingStatus = loggingStatus & ~LOGGING_STATUS_MASK_SD | LOGGING_STATUS_OFFSET_SD(2);
		Logging::log(1, "-SD Card unable to be opened\n");
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
		input.toCharArray(sendBuffer, 64);
		if(downlink->send((uint8_t*)sendBuffer, 64)){
			Serial.println("Message sent");
		}else{
			Serial.println("Message not sent");
		}
	}
	#endif

}

void Logging::log(unsigned char level, char* input, unsigned short length){
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
	#if LOG_TELE > 0
	if( level <= LOG_DOWNLINK & !(~loggingStatus & LOGGING_STATUS_MASK_DOWNLINK)){
		downlink->send(input, length);
	}
	#endif
};

void Logging::flush(){
	logFile.flush();
};

bool Logging::loggingReady(){
	return ready;
};
