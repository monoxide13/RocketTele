//// S_BMP388.hpp /////

#pragma once

#include "Sensor.hpp"
#include "BMP388_DEV.h"
#include "global.hpp"

#define STATUS_OFFSET 4

class S_BMP388 : public virtual Sensor{
	public:
	S_BMP388();
	~S_BMP388();
	short initialize() override;
	void tick() override;
	double getMeasurement() override;
	unsigned char getStatus() override;
	private:
	BMP388_DEV* baro;
	float altitude;
	float temp;
	float pres;
	unsigned long counter;
};

// Interrupt Handling
namespace S_BMP388_int{
	void callback();
	extern volatile bool dataReady;
};
