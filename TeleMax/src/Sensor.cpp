///// Sensor.cpp /////

#include "Sensor.hpp"

short Sensor::initialize(){
	sensorStatus=1;
	return 1;
};

void Sensor::tick(){
};

double Sensor::getMeasurement(){
	return 0;
};

unsigned char Sensor::getStatus(){
	return sensorStatus;
};
