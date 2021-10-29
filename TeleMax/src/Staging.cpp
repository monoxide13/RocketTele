///// Staging.cpp /////

#include "Staging.hpp"
#include "Logging.hpp"
#include "SensorGroup.hpp"
#include "TeleMax.hpp"

using namespace Staging;

TStagePtr Staging::stagePtr = &stage_STAGE_INIT;
unsigned short Staging::stage = STAGE_INIT;
bool Staging::stageChanged = true;
unsigned long Staging::nextMeasurementTime;
unsigned long Staging::loopCounter = 0;

void Staging::stage_STAGE_INIT(){
	// Initialize the sensors and calibrate
	SensorGroup::initialize();
	Logging::telemetryData->data.sensorStatus = SensorGroup::getStatus();
	nextMeasurementTime = TeleMax::loopTime + 1000000/LOG_RATE;
	stage = STAGE_PRELAUNCH;
	stageChanged=true;
};

void Staging::stage_STAGE_PRELAUNCH(){
	if(loopCounter == 0){ // First loop in this stage
		Logging::log(3,"First run of Stage_Prelaunch\n");
	}

	if(TeleMax::loopTime > nextMeasurementTime){
		Logging::telemetryData->data.timer = TeleMax::loopTime;
		Logging::telemetryData->data.sensorStatus = SensorGroup::getStatus();
		SensorGroup::getMeasurement();
		Logging::log(2, "-Staging: Loop counter at measurement time: " + String(loopCounter) + "\n");
		Logging::telemetryData->data.crc = calculateCRC(Logging::telemetryData);
		Logging::sendTelemetry();
		nextMeasurementTime = TeleMax::loopTime + 1000000/LOG_RATE;
		loopCounter=0;
	}
};

void Staging::stage_STAGE_ARMED(){
};

void Staging::stage_STAGE_ASCENT(){
};

void Staging::stage_STAGE_APOGEE(){
	stage=STAGE_DESCENT;
	stageChanged=true;
};

void Staging::stage_STAGE_DESCENT(){
};

void Staging::stage_STAGE_RECOVERY(){
};

void Staging::staging(){
	++loopCounter;
    if(stageChanged){
        switch(stage){
            case STAGE_PRELAUNCH:
                stagePtr = &stage_STAGE_PRELAUNCH;
                break;
            case STAGE_ARMED:
                stagePtr = &stage_STAGE_ARMED;
                break;
            case STAGE_ASCENT:
                stagePtr = &stage_STAGE_ASCENT;
                break;
            case STAGE_APOGEE:
                stagePtr = &stage_STAGE_APOGEE;
                break;
            case STAGE_DESCENT:
                stagePtr = &stage_STAGE_DESCENT;
                break;
            case STAGE_RECOVERY:
                stagePtr = &stage_STAGE_RECOVERY;
                break;
            default:
                stagePtr = &stage_STAGE_INIT;
                break;
		}
		Logging::log(3, "-StageChanged. Going to: " + String(stage) + "\n");
		Logging::telemetryData->data.stage = stage;
		loopCounter=0;
		stageChanged=false;
	}
	stagePtr();
};
