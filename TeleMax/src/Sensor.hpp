///// Sensors.hpp /////

#pragma once

#include "Filter1.hpp"

class Sensor{
	public:
	virtual short initialize();
	virtual void tick();
	virtual double getMeasurement();
	/* Status codes */
	/* 0 = ready */
	/* 1 = start up */
	/* 2 = */
	/* 3 = error */
	unsigned char getSensorStatus();
	protected:
	unsigned long long updateTime;
	unsigned short measurementReady();
	unsigned char sensorStatus;
	Filter1<void>* filter;
	private:

};
