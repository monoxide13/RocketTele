/* Telemetry.hpp */

#pragma once
#define RX_BUFFER_LENGTH 64

#include "RH_RF95.h"
	

class Telemetry{
	public:
	Telemetry();
	~Telemetry();
	bool receive();
	int init();
	private:
	RH_RF95 * downlink;
	char rxBuffer[RX_BUFFER_LENGTH];
	unsigned short rxLength;
};

