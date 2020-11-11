///// SensorGroup.cpp /////

#include "global.hpp"
#include "SensorGroup.hpp"
#include <cstddef>

struct Sensor_Linked_List * SensorGroup::sensors;

using namespace SensorGroup;
unsigned short SensorGroup::sensorCount;

void SensorGroup::addSensor(class Sensor * newSensor){
	if(sensors == NULL){
		sensors = new struct Sensor_Linked_List;
		sensors->sensor = newSensor;
		sensors->next = NULL;
	}else{
		struct Sensor_Linked_List* current = sensors;
		while(current->next != NULL){
			current = current->next;
		}
		current->next = new struct Sensor_Linked_List;
		current->next->sensor = newSensor;
		current->next->next = NULL;
	}
	sensorCount++;
};

void SensorGroup::removeSensor(class Sensor * oldSensor){
	// Yeah I think I'll just do this later...
};

void SensorGroup::initialize(){
	struct Sensor_Linked_List* current = sensors;
	while(current!=NULL){
		current->sensor->initialize();
		current = current->next;
	}
};

void SensorGroup::getMeasurement(){
	struct Sensor_Linked_List* current = sensors;
	while(current!=NULL){
		current->sensor->getMeasurement();
		current = current->next;
	}
};

void SensorGroup::tick(){
	struct Sensor_Linked_List* current = sensors;
	while(current!=NULL){
		current->sensor->tick();
		current = current->next;
	}
};

bool SensorGroup::isReady(){
	bool ready=true;
	struct Sensor_Linked_List* current = sensors;
	while(current!=NULL){
		if(!current->sensor->sensorReady)
			ready=false;
		current = current->next;
	}
	return ready;
};
