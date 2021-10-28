/* BaseStation.cpp */

#include "Arduino.h"
#include "wiring_private.h" // for pinPeripheral();
#include "global.hpp"
#include "BaseStation.hpp"
#include "Output.hpp"
#include "Telemetry.hpp"
#include "LiquidCrystal_I2C.h"
#include "StatusLEDs.hpp"

using namespace BaseStation;

unsigned long BaseStation::loopTime=0;
Output BaseStation::packetLed = Output(PACKET_LED_PIN);
Output BaseStation::readyLed = Output(READY_LED_PIN);
Output BaseStation::debugLed = Output(SYSTEM_LED_PIN);
Output BaseStation::systemLed = Output(SYSTEM2_LED_PIN);
String BaseStation::debugText = "";
Telemetry BaseStation::telemetry; 
LiquidCrystal_I2C lcd(0x20, 16, 2);

unsigned long long displayUpdate;

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
	//while(!Serial.available()){};
	Serial.println("+Starting...");

	StatusLEDs::initialize();
	
	/*** Set initial values ***/
	readyLed.turnOff();
	debugLed.turnOff();
	lcd.init();
	lcd.clear();
	lcd.backlight();
	lcd.print("Ready");
	StatusLEDs::test();
	Serial.println("+Starting Telemetry");
	if(!telemetry.init()){
		Serial.println("+Downlink OK");
	}else{
		Serial.println("+Downlink BAD");
	}
	lcd.clear();
	systemLed.repeat({1,0});
	displayUpdate=0;
	lcd.print("TSP: ");
};

void loop(void){
	loopTime = micros();
	telemetry.receive();
	if(Serial.available()){
		Serial.read();
	}
	if(displayUpdate<loopTime){
		//lcd.clear();
		float time = (millis() - telemetry.lastGoodTime)/(float)1000;
		lcd.setCursor(5,0);
		if(time<100)
			lcd.print(String(time, 1) + "   ");
		else
			lcd.print(String(time, 0) + "   ");
		lcd.setCursor(0,1);
		lcd.print(String(telemetry.getSNR()) + "  ");
		lcd.setCursor(7,1);
		lcd.print(String(telemetry.downlink->rxGood()) + ":" + String(telemetry.downlink->rxBad()));
		displayUpdate=loopTime+100000; // 10 times per second
	}
	heartbeat();
};

inline void BaseStation::heartbeat(){
	StatusLEDs::tick();
	readyLed.tick();
	debugLed.tick();
	systemLed.tick();
};

