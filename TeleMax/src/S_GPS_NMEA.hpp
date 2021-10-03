//// S_GPS.hpp /////

#pragma once

// So it won't passthru messages.
#define GPSECHO false

#include "Sensor.hpp"
#include "global.hpp"
#include "Adafruit_GPS.h"
#include "TeleMax.hpp"


class S_GPS_NMEA : public virtual Sensor{
	public:
	S_GPS_NMEA();
	~S_GPS_NMEA();
	short initialize() override;
	void tick() override;
	double getMeasurement() override;
	unsigned char getStatus() override;
	private:
	bool fixLocked;
	bool newData;
	bool hasReceivedNMEA;
};
