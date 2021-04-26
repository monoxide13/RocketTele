///// Staging.hpp /////

#pragma once

#include "global.hpp"
#include "time.h"

// Funny thing to note, the function names don't expand. But who cares! The run just fine.

typedef void(*TStagePtr)();

namespace Staging{
	void stage_STAGE_INIT();
	void stage_STAGE_PRELAUNCH();
	void stage_STAGE_ARMED();
	void stage_STAGE_ASCENT();
	void stage_STAGE_APOGEE();
	void stage_STAGE_DESCENT();
	void stage_STAGE_RECOVERY();
	void staging();

	extern unsigned long loopCounter;
	extern TStagePtr stagePtr;
	extern unsigned short stage;
	extern bool stageChanged;
	extern unsigned long nextMeasurementTime;
};
