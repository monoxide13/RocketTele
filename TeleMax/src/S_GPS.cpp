///// S_GPS.cpp /////

#include "S_GPS.hpp"
#include "Logging.hpp"
#include "Staging.hpp"
#include "TeleMax.hpp"


S_GPS::S_GPS(){
	//UBLOX* gps = new UBLOX(gpsPort);
	newData=false;
	sensorStatus=3;
};

S_GPS::~S_GPS(){
	//delete (gps);
};

short S_GPS::initialize(){
	sensorStatus=1;
	Logging::log(3, "-Emptying GPS buffer");
	while(Serial1.available()){
		Serial1.read();
		Logging::log(3,".");
	}
	Logging::log(3, "\n");
	return 0;
};

void S_GPS::tick(){
	// GPS is set to transmit every .5 seconds. Therefore new data is pretty rare comparitively. So when we get data, lets write it out immediately.
	/* Commented out since using NMEA
	if(gps.readSensor()){
		newData=true;
		Logging::log(2,"-GNSS: DT: " + String(gps.getMonth(),2) + "/" + String(gps.getDay(),2) + "/" + String(gps.getYear()) + "-" + String(gps.getHour(),2) + ":" + String(gps.getMin(),2) + ":" + String(gps.getSec(),2));
		Logging::log(2,"-GNSS: VelN:" + String(gps.getNorthVelocity_ms()) + " VelE:" + String(gps.getEastVelocity_ms()) + " VelD:" + String(gps.getDownVelocity_ms()) + "\n");
		Logging::log(1,"G:" + String(gps.getLatitude_deg()) + "," + String(gps.getLongitude_deg()) + "," + String(gps.getEllipsoidHeight_m()) + "," + String(gps.getDownVelocity_ms()) + "," + String(gps.getNumSatellites()) + "," + String(gps.getpDOP()) + "\n");
		if(gps.isGnssFixOk())
			sensorStatus=0;
		else
			sensorStatus=3;
	};
	*/
};

double S_GPS::getMeasurement(){
	if(newData){
		newData=false;
	}
	return 0;
};
