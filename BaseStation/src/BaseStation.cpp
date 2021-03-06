/* BaseStation.cpp */

#include "Arduino.h"
#include "wiring_private.h" // for pinPeripheral();
#include "global.hpp"
#include "BaseStation.hpp"
#include "Output.hpp"
#include "Telemetry.hpp"
#include "LiquidCrystal_I2C.h"

using namespace BaseStation;

unsigned long BaseStation::loopTime=0;
Output BaseStation::packetLed = Output(PACKET_LED_PIN);
Output BaseStation::readyLed = Output(READY_LED_PIN);
Output BaseStation::debugLed = Output(SYSTEM_LED_PIN);
Output BaseStation::systemLed = Output(SYSTEM2_LED_PIN);
Telemetry BaseStation::telemetry; 
LiquidCrystal_I2C lcd(0x20, 16, 2);

namespace {
	unsigned long long snrUpdate;
}

void setup(void){
	/*** Configure pins ***/
	pinMode(RF95_CS_PIN, OUTPUT);
	digitalWrite(RF95_CS_PIN, HIGH);
	pinMode(RF95_RST_PIN, OUTPUT);
	digitalWrite(RF95_RST_PIN, HIGH);
	pinMode(VOLTAGE_PIN, INPUT);
	
	/*** Start the bus ***/
	Wire.begin();
	SPI.begin();

	/*** Start the serial ports ***/
	Serial.begin(192000);
	while(!Serial.available()){};
	Serial.println("+Starting...");
	pinPeripheral(10, PIO_SERCOM); // TX
	pinPeripheral(11, PIO_SERCOM); // RX

	
	/*** Set initial values ***/
	lcd.init();
	lcd.backlight();
	readyLed.turnOff();
	debugLed.turnOff();
	systemLed.repeat({1,0});
	Serial.println("+Starting Telemetry");
	if(!telemetry.init()){
		Serial.println("+Downlink OK");
	}else{
		Serial.println("+Downlink BAD");
	}
	lcd.clear();
	lcd.print("Ready");
	delay(5000);
	snrUpdate=micros();
};

void loop(void){
	loopTime = micros();
	telemetry.receive();
	if(Serial.available()){
		Serial.read();
	}
	if(snrUpdate<loopTime){
		lcd.clear();
		lcd.print(loopTime);
		lcd.setCursor(0,1);
		lcd.print(telemetry.getSNR());
		snrUpdate=loopTime+1000000;
	}
	heartbeat();
};

inline void BaseStation::heartbeat(){
	readyLed.tick();
	debugLed.tick();
	systemLed.tick();
};

