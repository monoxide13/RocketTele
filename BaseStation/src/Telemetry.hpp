/* Telemetry.hpp */

#pragma once
#define RX_BUFFER_LENGTH 128
#define SNR_HYSTERESIS 5

#include "RH_RF95.h"
	

class Telemetry{
	public:
	Telemetry();
	~Telemetry();
	bool receive();
	int init();
	int getSNR();
	private:
	inline bool checkPacket(unsigned char *);
	RH_RF95 * downlink;
    uint8_t rxBuffer[RX_BUFFER_LENGTH];
    unsigned short offset;
	unsigned char rxLength;
	int snrArray[SNR_HYSTERESIS];
	unsigned short snrIter;
};

