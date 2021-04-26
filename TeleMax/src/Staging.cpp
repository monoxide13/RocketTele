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

void Staging::stage_STAGE_INIT(){
	// Initialize the sensors and calibrate
	SensorGroup::initialize();
	nextMeasurementTime = TeleMax::loopTime + 1000000/LOG_RATE;
	Logging::telemetryData->data.stage = stage = STAGE_PRELAUNCH;
	stageChanged=true;
};

void Staging::stage_STAGE_PRELAUNCH(){
	if(TeleMax::loopCounter = 1){ // First loop in this stage
		
	}

	if(TeleMax::loopTime > nextMeasurementTime){
		SensorGroup::getMeasurement();
		//Logging::log(3, "-Staging: Loop counter at measurement time: " + String(loopCounter) + "\n");
		nextMeasurementTime = TeleMax::loopTime + 1000000/LOG_RATE;
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
		Logging::log(3, "-StageChanged:" + String(stage) + "\n");
		Logging::telemetryData->data.stage = stage;
		stageChanged=false;
		TeleMax::loopCounter=0;
	}
	stagePtr;
};
