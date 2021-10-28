///// packet_def.h /////

#pragma once

#ifdef __cplusplus
extern "C"{
#endif

#define TELEMETRY_PACKET_LENGTH 64 //uint8_t
#include <stdint.h>

struct Telemetry_Packet_Struct{
	/* System Status   4+4+1+1+1=11 bytes */
	uint32_t key; // key to identify start of packet.
	uint32_t timer; // 1/10 seconds since start.
	uint8_t stage; // Stage the rocket is in.
	uint8_t sensorStatus; // Accel, Baro, GPS, SD/Tele
	uint8_t voltage; // x.xx volts. voltage*.02 = actual Voltage. For lipo should be between 3.2 and 4.2. 
	
	/* GPS   1+4*4=17 bytes */
	uint8_t fixqual; // first two bits fixquality (1=nofix, 2=2D, 2=3D), last 6 bits satellite count.
	float lat;
		// 16 byte boundary
	float lon;
	float galt;
	float hdop;

	/* Accel   4*7=28 bytes */
	float qw; // QuaternionA
		// 32 byte boundary
	float qx;
	float qy;
	float qz;
	float ax;
		// 16 byte boundary
	float ay;
	float az;
	
	
	/* Baro   4+2=6 bytes */
	float temp; // Celcius
	uint16_t balt; // meters
	
	/* Total Data Size -> 62 bytes */
	/* CRC    2 bytes */
	uint16_t crc; // Calculated by xoring 2 bytes at a time. 
};

union Telemetry_Packet{
	uint8_t byte[TELEMETRY_PACKET_LENGTH];
	struct Telemetry_Packet_Struct data;
};

uint16_t calculateCRC(union Telemetry_Packet *);
void Telemetry_Packet_Init(union Telemetry_Packet *);

#ifdef __cplusplus
}
#endif
