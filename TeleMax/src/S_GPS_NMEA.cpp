///// S_GPS_NMEA.cpp /////

#include "S_GPS_NMEA.hpp"
#include "Logging.hpp"
#include "Staging.hpp"
#include "TeleMax.hpp"

#define STATUS_OFFSET 0

S_GPS_NMEA::S_GPS_NMEA(){
	newData=false;
	sensorStatus=1;
	gps.begin(9600);
	Logging::log(3, "-GPS Added\n");
	gps.sendCommand(PMTK_SET_NMEA_OUTPUT_GGAONLY);
	hasReceivedNMEA=false;
};

S_GPS_NMEA::~S_GPS_NMEA(){
};


short S_GPS_NMEA::initialize(){
	Logging::log(3, "-Emptying GPS buffer");
	while(Serial1.available()){
		Serial1.read();
		Logging::log(3,".");
	}
	Logging::log(3, "\n");
	gps.sendCommand(PMTK_SET_BAUD_57600);
	Serial1.flush();
	delay(1000);
	Serial1.end();
	delay(1000);
	gps.begin(57600);
	delay(1000);
	//gps.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
	gps.sendCommand(PMTK_SET_NMEA_UPDATE_2HZ);
	sensorStatus=2;
	return 0;
};

void S_GPS_NMEA::tick(){
	gps.read();
	if(gps.newNMEAreceived()){
		Logging::log(3, "-NMEA " + String(gps.lastNMEA()) + "\n");
		sensorStatus=0;
		if(gps.parse(gps.lastNMEA())){
			newData=true;
			Logging::log(1, "-GPS Fix:" + String(gps.fix)+", Quality:" + String(gps.fixquality) + ", Sats:" + String(gps.satellites) + ", HDOP:" + String(gps.HDOP) + "\n");
			if(gps.fix){
				Logging::log(2, "-GPS Lat:" + String(gps.latitude) + String(gps.lat) + ", Lon:" + String(gps.longitude) + String(gps.lon) + ", Alt:" + String(gps.altitude) + "\n");
			}
			switch(gps.fixquality){
				case 2:
					sensorStatus=0;
					break;
				case 1:
					sensorStatus=2;
					break;
				default:
					sensorStatus=1;
					break;
			}
		}
	};
};

double S_GPS_NMEA::getMeasurement(){
	if(newData){
		newData=false;
	}
	Logging::telemetryData->data.fixqual = 0;
	Logging::telemetryData->data.fixqual = gps.fixquality << 6 | gps.satellites;
	Logging::telemetryData->data.lat = gps.latitude;
	Logging::telemetryData->data.lon = gps.longitude;
	Logging::telemetryData->data.galt = gps.altitude;
	Logging::telemetryData->data.hdop = gps.HDOP;
	return 0;
};

unsigned char S_GPS_NMEA::getStatus(){
	return sensorStatus << STATUS_OFFSET;
}
