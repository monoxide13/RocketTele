/* BaseStation.cpp */

#include "Arduino.h"
#include "wiring_private.h" // for pinPeripheral();
#include "global.hpp"
#include "BaseStation.hpp"
#include "Output.hpp"

using namespace BaseStation;

HC12 rocket = HC12(&Serial1, HC12_SET_PIN);

unsigned long BaseStation::loopTime=0;
Output BaseStation::packetLed = Output(PACKET_LED_PIN);
Output BaseStation::readyLed = Output(READY_LED_PIN);
Output BaseStation::debugLed = Output(SYSTEM_LED_PIN);
Output BaseStation::systemLed = Output(SYSTEM2_LED_PIN);

void setup(void){
	/*** Configure pins ***/
	pinMode(SD_CS_PIN, OUTPUT);
	digitalWrite(SD_CS_PIN, HIGH);
	pinMode(SD_CD_PIN, INPUT_PULLUP);
	pinMode(HC12_SET_PIN, OUTPUT);
	digitalWrite(HC12_SET_PIN, HIGH);
	pinMode(VOLTAGE_PIN, INPUT);

	/*** Start the serial ports ***/
	Serial.begin(9600);
	while(!Serial.available()){};
	Serial.println("+Starting...");
	rocket.init();
	pinPeripheral(10, PIO_SERCOM); // TX
	pinPeripheral(11, PIO_SERCOM); // RX
	
	/*** Set initial values ***/
	rocket.setDefaults();
	Serial.println("+Defaults set");
	if(rocket.testModule()){
		Serial.println("+Module OK");
	}else{
		Serial.println("+Module not OK");
	}
	readyLed.turnOff();
	debugLed.turnOff();
	systemLed.repeat({1,0});
};

void loop(void){
	loopTime = micros();
	heartbeat();
	rocket.receive();
	if(rocket.readReady){
		packetLed.turnOn();
		systemLed.turnOn();
		Serial.print(rocket.lastRead + "\n");
		rocket.readReady=false;
		/* Check messages for sensor command to determine if rocket is ready for launch */
		if(rocket.lastRead.startsWith("C:")){
			int val = rocket.lastRead.indexOf(rocket.lastRead.indexOf(',')+1);
			if(val == STAGE_PRELAUNCH)
				readyLed.turnOn();
		}
		systemLed.turnOff();
	}
};

inline void BaseStation::heartbeat(){
	readyLed.tick();
	debugLed.tick();
	systemLed.tick();
};

