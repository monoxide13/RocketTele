///// Logging.hpp /////


#pragma once

#include "global.hpp"
#include "Stream.h"
#include "packet_def.h"

namespace Logging{
	void init();
	void openFile();
	void closeFile();
	void log(unsigned char, String);
	void log(unsigned char, char*, unsigned short);
	bool loggingReady();
	void flush();

	extern union Telemetry_Packet * telemetryData;

	// Private: Defined in Logging.cpp
		/*	char loggingStatus
			2 bits per device. unused - Downlink - SD - Serial port
			00 means not used/disabled, not preset. 01 means not initialized or closed.
			10 means error. 11 means normal. */

};
