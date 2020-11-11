//// S_MPU6050.hpp /////

#pragma once

#include "Sensor.hpp"
#include "MPU6050.h"
#include "global.hpp"

class S_MPU6050 : public virtual Sensor{
	public:
	S_MPU6050();
	~S_MPU6050();
	short initialize() override;
	void tick() override;
	double getMeasurement() override;
	private:
	MPU6050* accel;
	bool dmpReady;
	uint8_t mpuIntStatus;
	uint8_t devStatus;
	uint16_t packetSize; // default is 42 bytes.
	uint16_t fifCount;
	uint8_t fifoBuffer[64];
	uint16_t counter;

	// Data types
	Quaternion q;
	VectorInt16 aa;
	VectorInt16 gy;
	VectorInt16 aaReal;
	VectorInt16 aaWorld;
	VectorFloat gravity;
};

namespace TeleMax {
	void dmpDataReady();
	extern volatile bool dataReady;
};
