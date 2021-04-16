/* BaseStation.hpp */

#pragma once

#include "Uart.h"
#include "Output.hpp"
#include "Telemetry.hpp"

namespace BaseStation{
	extern unsigned long loopTime;
	extern Output packetLed;
	extern unsigned long packetTime;
	extern Output readyLed;
	extern Output debugLed;
	extern Output systemLed;
	extern Telemetry telemetry;
	void heartbeat();
};
