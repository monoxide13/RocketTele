///// packet_def.c /////

#include "packet_def.h"

uint16_t calculateCRC(union Telemetry_Packet * tp){
	uint8_t crc[] = {0,0};
	unsigned short x=0;
	for(x=0; x<TELEMETRY_PACKET_LENGTH-2; ++x){
		crc[0] ^= tp->byte[x];
		crc[1] ^= tp->byte[x+1];
	}
	return crc[1]<<8 & crc[0];
};

void Telemetry_Packet_Init(union Telemetry_Packet * tp){
	memset(tp, 0, TELEMETRY_PACKET_LENGTH);
}
