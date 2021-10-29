/* Telemetry.hpp */

#pragma once
#define RX_BUFFER_LENGTH 128
#define SNR_HYSTERESIS 3

#include "packet_def.h"
#include "RH_RF95.h"
	

class Telemetry{
	public:
	Telemetry();
	~Telemetry();
	bool receive();
	int init();
	float getSNR();
	unsigned long lastGoodTime;
	RH_RF95 * downlink;
	private:
	inline bool checkPacket(Telemetry_Packet *);
	inline void processPacket(Telemetry_Packet *);
    uint8_t rxBuffer[RX_BUFFER_LENGTH];
    unsigned short offset;
	unsigned char rxLength;
	short snrArray[SNR_HYSTERESIS];
	unsigned short snrIter;
};

