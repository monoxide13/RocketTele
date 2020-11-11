///// Sensors.hpp /////

#pragma once

#include "Filter1.hpp"

class Sensor{
	public:
	virtual short initialize();
	virtual void tick();
	virtual double getMeasurement();
	unsigned short sensorStatus;
	bool sensorReady;
	protected:
	unsigned long long updateTime;
	unsigned short measurementReady();
	Filter1<void>* filter;
	private:

};
