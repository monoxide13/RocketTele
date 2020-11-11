//// S_GPS.hpp /////

#pragma once

#include "Sensor.hpp"
#include "global.hpp"
#include "UBLOX.h"
#include "TeleMax.hpp"

class S_GPS : public virtual Sensor{
	public:
	S_GPS();
	~S_GPS();
	short initialize() override;
	void tick() override;
	double getMeasurement() override;
	private:
	//UBLOX* gps;
	bool fixLocked;
	bool newData;
};
