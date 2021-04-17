/* Telemetry.hpp */

#pragma once
#define RX_BUFFER_LENGTH 64
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
	RH_RF95 * downlink;
	char rxBuffer[RX_BUFFER_LENGTH];
	unsigned short rxLength;
	int snrArray[SNR_HYSTERESIS];
	unsigned short snrIter;
};

