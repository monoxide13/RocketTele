///// Sensor.cpp /////

#include "Sensor.hpp"

short Sensor::initialize(){
	sensorReady=false;
	return 1;
};

void Sensor::tick(){
};

double Sensor::getMeasurement(){
	return 0;
};
