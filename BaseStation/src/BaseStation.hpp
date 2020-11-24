/* BaseStation.hpp */

#pragma once

#include "Uart.h"
#include "Output.hpp"
#include "HC12.hpp"

namespace BaseStation{
	extern unsigned long loopTime;
	extern Output packetLed;
	extern Output readyLed;
	extern Output debugLed;
	extern Output systemLed;
	void heartbeat();
};
extern HC12 rocket;
