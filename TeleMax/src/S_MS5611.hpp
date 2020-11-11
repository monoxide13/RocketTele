//// S_MS5611.hpp /////

#pragma once

#include "Sensor.hpp"
#include "ms5611.h"
#include "global.hpp"

class S_MS5611 : public virtual Sensor{
	public:
	S_MS5611();
	~S_MS5611();
	short initialize() override;
	void tick() override;
	double getMeasurement() override;
	private:
	MS5611* baro;
	bool readingInProgress;
	bool readingReady;
	char cmd;
	unsigned long temp;
	unsigned long pres;
	unsigned long waitingLoops;
	unsigned long readingLoops;
};
