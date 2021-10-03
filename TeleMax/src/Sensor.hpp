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
	virtual unsigned char getStatus();
	protected:
	Filter1<void>* filter;
	unsigned short measurementReady();
	unsigned long long updateTime;
	unsigned char sensorStatus;
	private:

};
