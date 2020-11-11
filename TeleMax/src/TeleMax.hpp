///// TeleMax.hpp /////

#pragma once

#include "global.hpp"
#include "time.h"
#include "Uart.h"
#include "UBLOX.h"
#include "Output.hpp"

namespace TeleMax{
	void heartbeat();
	extern bool heartbeat_toggle;
	extern unsigned long loopTime;
	extern Output debugLed;
	extern Output systemLed;
	extern Output buzzer;
	extern unsigned long voltageReadTime;
	extern float voltage;
};
extern Uart gpsPort;
extern UBLOX gps;
