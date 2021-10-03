///// S_BMP388.cpp /////

#include "S_BMP388.hpp"
#include "Logging.hpp"
#include "Staging.hpp"
#include "TeleMax.hpp"
#define STATUS_OFFSET 4


S_BMP388::S_BMP388() {
	unsigned long counter=0;
	Logging::log(2, "-BMP388 in startup.\n");
	S_BMP388_int::dataReady=false;
	baro=new BMP388_DEV(BMP388_CS_PIN);
	temp=pres=altitude=0;
	// If sensor is not connected or wrong ID, begin() returns 0. Success=1.
	if(baro->begin(SLEEP_MODE)){ //Set mode to forced/one shot. 0x01
		sensorStatus=2; // Sensor good, in start up.
		Logging::log(1, "-BMP388 Startup complete.\n");
	}else{
		sensorStatus=1; // Sensor no good.
		Logging::log(1, "-BMP388 unable to startup.\n");
	}
};

S_BMP388::~S_BMP388(){
	delete (baro);
};

short S_BMP388::initialize(){
	if(sensorStatus==1){
		Logging::log(1, "-BMP388 in error. Unable to initialize.\n");
		return -1;
	}
	Logging::log(3, "-BMP388 starting calibration.\n");
	// Take 10 readings, and average
	int x;
	temp=pres=0;
	altitude=0;
	baro->setPresOversampling(OVERSAMPLING_X32);
	baro->setTempOversampling(OVERSAMPLING_X2);
	baro->setIIRFilter(IIR_FILTER_OFF);
	for(x=0; x<5; ++x){
		baro->startForcedConversion();
		float tTemp, tPres, tAlt;
		delay(100);
		if(!(baro->getMeasurements(tTemp, tPres, tAlt))){
			--x;
			continue;
		}
		temp += tTemp;
		pres += tPres;
		altitude += tAlt;
		delay(200);
	}
	temp = round(temp/10);
	pres = pres/10;
	Logging::log(2, "-BMP388 reporting temperature: " + String(temp) + "c\n");
	Logging::log(2, "-BMP388 station pressure:" + String(pres) + "mbar\n");
	Logging::log(2, "-BMP388 starting altitude:" + String(altitude) + "m\n");
	// Change to normal settings, enable interrupt
	baro->setPresOversampling(OVERSAMPLING_X2);
	baro->setTempOversampling(OVERSAMPLING_SKIP);
	baro->setIIRFilter(IIR_FILTER_OFF);
	baro->setTimeStandby(TIME_STANDBY_40MS);
	attachInterrupt(digitalPinToInterrupt(BMP388_INT_PIN), S_BMP388_int::callback, RISING);
	baro->enableInterrupt(PUSH_PULL, ACTIVE_HIGH, UNLATCHED);
	baro->startNormalConversion();
	sensorStatus=0;
	return 0;
};

void S_BMP388::tick(){
	if(sensorStatus!=0)
		return;
	if(S_BMP388_int::dataReady){
		++counter;
		// TODO: Read measurement.
		if(!(baro->getMeasurements(temp, pres, altitude))){
			Logging::log(3, "-BMP388. Reading ready but unable to read");
		}
		Logging::log(2, "B:" + String(altitude) + ',' + String(temp) + ',' + String(pres) + '\n');
		S_BMP388_int::dataReady=false;
	}

};

double S_BMP388::getMeasurement(){
	if(sensorStatus!=0)
		return 0;
	Logging::log(3, "-B: measurements taken: " + String(counter) + '\n');
	Logging::telemetryData->data.balt = altitude;
	Logging::telemetryData->data.temp = temp;;
	counter=0;
	return 0;
};

unsigned char S_BMP388::getStatus(){
	return sensorStatus << STATUS_OFFSET;
}

volatile bool S_BMP388_int::dataReady;
void S_BMP388_int::callback(){
	S_BMP388_int::dataReady=true;
};
