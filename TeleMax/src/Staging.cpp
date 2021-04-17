///// Staging.cpp /////

#include "Staging.hpp"
#include "Logging.hpp"
#include "SensorGroup.hpp"
#include "TeleMax.hpp"

using namespace Staging;

unsigned long Staging::loopCounter;
TStagePtr Staging::stagePtr;
unsigned short Staging::stage;
bool Staging::stageChange;
unsigned long Staging::nextMeasurementTime;

void Staging::stage_STAGE_INIT(){
	// Initialize the sensors and calibrate
	SensorGroup::initialize();
	nextMeasurementTime = TeleMax::loopTime + 1000000/LOG_RATE;
	stage=STAGE_PRELAUNCH;
	stageChange=true;
};

void Staging::stage_STAGE_PRELAUNCH(){
	// Check accel, if > 0, advance

	// if accel <= 0, reset altitude to match gps

	if(TeleMax::loopTime > nextMeasurementTime){
		// Clock, stageNum, trigger, calculatedAlt, voltage
		//Logging::log(1, "clock
		SensorGroup::getMeasurement();
		//Logging::log(3, "-Staging: Loop counter at measurement time: " + String(loopCounter) + "\n");
		nextMeasurementTime = TeleMax::loopTime + 1000000/LOG_RATE;
		loopCounter=1;
	}
};

void Staging::stage_STAGE_THRUST(){
	// if accel<=0, rocket is coasting, advance
};

void Staging::stage_STAGE_COAST(){
};

void Staging::stage_STAGE_APOGEE(){
	stage=STAGE_DESCENT;
	stageChange=true;
};

void Staging::stage_STAGE_DESCENT(){
};

void Staging::stage_STAGE_RECOVERY(){
};
