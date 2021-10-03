///// SensorGroup.hpp /////

#pragma once

#include "Sensor.hpp"

struct Sensor_Linked_List {
	Sensor * sensor;
	struct Sensor_Linked_List * next;
};

namespace SensorGroup{
	void addSensor(class Sensor *);
	void removeSensor(class Sensor *);
	void initialize();
	void getMeasurement();
	void tick();
	bool isReady();
	/* Output is in order sensors are added. Mine is accel, baro, gps */
	unsigned short getStatus();
	extern unsigned short sensorCount;
	extern struct Sensor_Linked_List * sensors;
};
