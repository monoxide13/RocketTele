///// S_MS5611.cpp /////

#include "S_MS5611.hpp"
#include "Logging.hpp"
#include "Staging.hpp"
#include "TeleMax.hpp"


S_MS5611::S_MS5611(){
	Logging::log(3, "-MS5611 in startup.\n");
	baro=new MS5611(MS5611_CS);
	sensorStatus=3;
	tempReadInProgress=false;
	tempReadTime=0;
	readingInProgress=false;
	readingReady=false;
	cmd=0;
	temp=0;
	pres=0;
	readingLoops=0;
	readyLoops=0;
	baro->init();
	// For me, if sensor is not connected init will return 0.
	if(baro->init()){
		sensorStatus=1; // Sensor good, in start up.
	}else{
		Logging::log(1, "-MS5611 unable to startup.\n");
	}
	Logging::log(3, "-MS5611 Startup complete.\n");
};

S_MS5611::~S_MS5611(){
	delete (baro);
};

short S_MS5611::initialize(){
	if(sensorStatus==3){
		Logging::log(2, "-MS5611 error. Unable to initialize.\n");
		return -1;
	}
	Logging::log(3, "-MS5611 starting calibration.\n");
	// Take 10 temperature readings, and average
	int x;
	temp=0;
	for(x=0; x<10; ++x){
		baro->getTemperature(CMD_ADC_4096);
		delay(100);
		temp += baro->getReading();
		delay(100);
	}
	temp = (unsigned long)temp/10;
	Logging::log(2, "-MS5611 reporting temperature: " + String(baro->calculateSecondOrderTemperatureCompensation(temp)) + "c\n");

	// Take 5 pressure readings, and average
	pres=0;
	for(x=0; x<10; ++x){
		baro->getPressure(CMD_ADC_4096);
		delay(100);
		pres += baro->getReading();
		delay(100);
	}
	pres=(unsigned long) pres/10;
	Logging::log(2, "-MS5611 station pressure:" + \
		String(baro->setStationPressure(baro->calculatePressureCompensation(pres, temp))) + \
		"mbar\n");
	tempReadTime = TeleMax::loopTime+20000;
	sensorStatus=0;
	return 0;
};

void S_MS5611::tick(){
	if(readingInProgress){
		++readingLoops;
		if(Staging::nextMeasurementTime<1000 + TeleMax::loopTime){
			pres=baro->getReading();
			readingInProgress=false;
			readingReady=true;
		}
	}else if(readingReady){
		++readyLoops;
	/* We have completed a reading, so the sensor is not needed right away. Let's check the temp */
	}else if(tempReadInProgress){
		if(TeleMax::loopTime>tempReadTime){ /* Having this in separate if blocks pressure command from being sent in the event we're not done reading */
			//temp=baro->getReading();
			tempReadInProgress=false;
			tempReadTime = Staging::nextMeasurementTime;
		}
	}else if(Staging::nextMeasurementTime<20000+TeleMax::loopTime){
		// Reading times: 256-600us; 512-1170us; 1024-2280us; 2048-4540us; 4096-9040us;
		baro->getPressure(CMD_ADC_4096);
		readingInProgress=true;
	}else if(TeleMax::loopTime>tempReadTime){
		/* In theory at the max planned log rate of 20Hz, that's 50ms between reads.
		 * There should be enough time to read temp and pres each time in the window. */
		//baro->getTemperature(CMD_ADC_4096);
		tempReadTime = TeleMax::loopTime + 9040;
		tempReadInProgress=true;
	}
};

double S_MS5611::getMeasurement(){
	readingReady=false;
	Logging::log(3, "-MS5611: readingLoops:" + String(readingLoops) + " readyLoops:" + String(readyLoops) + "\n");
	// I'm logging the first order temp compensation. The second could be used for more accuracy, but I'm wanting to save a few cpu cycles.
	Logging::log(1, "B:" + String(baro->getAltitude(pres, temp)) + "," + String(baro->calculateTemperatureCompensation(temp)) + "," + String(baro->calculatePressureCompensation(pres, temp)) +  "\n");
	readingLoops=0;
	readyLoops=0;
	return 0;
};

