/* Telemetry.hpp */

#pragma once
#define RX_BUFFER_LENGTH 128
#define SNR_HYSTERESIS 5

#include "packet_def.h"
#include "RH_RF95.h"
	

class Telemetry{
	public:
	Telemetry();
	~Telemetry();
	bool receive();
	int init();
	int getSNR();
	unsigned long lastGoodTime;
	private:
	inline bool checkPacket(Telemetry_Packet *);
	inline void processPacket(Telemetry_Packet *);
	RH_RF95 * downlink;
    uint8_t rxBuffer[RX_BUFFER_LENGTH];
    unsigned short offset;
	unsigned char rxLength;
	int snrArray[SNR_HYSTERESIS];
	unsigned short snrIter;
};

