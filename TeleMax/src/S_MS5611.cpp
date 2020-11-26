///// S_MS5611.cpp /////

#include "S_MS5611.hpp"
#include "Logging.hpp"
#include "Staging.hpp"
#include "TeleMax.hpp"


S_MS5611::S_MS5611(){
	Logging::log(3, "-MS5611 in initialization. ");
	baro = new MS5611(MS5611_CS);
	sensorStatus=0;
	sensorReady=false;
	readingInProgress=false;
	readingReady=false;
	cmd=0;
	temp=0;
	pres=0;
	waitingLoops=0;
	Serial.println(String(baro->init()));
	// Add some sort of check to see if sensor is working.
	// If not working, sensors returns temp 20, pressure 0.
	Logging::log(3, "-Initialization complete.\n");
};

S_MS5611::~S_MS5611(){
	delete (baro);
};

short S_MS5611::initialize(){
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
	sensorReady=true;
	return 0;
};

void S_MS5611::tick(){
	if(!(readingInProgress || readingReady) && Staging::nextMeasurementTime<15000+TeleMax::loopTime){
		// Reading times: 256-600us; 512-1170us; 1024-2280us; 2048-4540us; 4096-9040us;
		baro->getPressure(CMD_ADC_4096);
		cmd=CMD_ADC_4096;
		readingInProgress=true;
	}else if(!readingReady && readingInProgress && Staging::nextMeasurementTime<500+TeleMax::loopTime){
		pres = baro->getReading();
		readingInProgress=false;
		readingReady=true;
	}
	if(readingInProgress)
		++waitingLoops;
	if(readingReady)
		++readingLoops;
};

double S_MS5611::getMeasurement(){
	readingReady=false;
	Logging::log(3, "-MS5611: waitingLoops:" + String(waitingLoops) + " readingLoops:" + String(readingLoops) + "\n");
	Logging::log(1, "B:" + String(baro->calculatePressureCompensation(pres, temp)) + "," + String(baro->getAltitude(pres, temp)) + "," + String(temp) + "," + String(pres) +  "\n");
	waitingLoops=0;
	readingLoops=0;
	return 0;
};

