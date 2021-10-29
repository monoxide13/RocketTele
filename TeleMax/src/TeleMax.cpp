///// TeleMax.cpp /////
#include "Arduino.h"
#include "global.hpp"
#include "TeleMax.hpp"
#include "SensorGroup.hpp"
#include "Logging.hpp"
#include "Staging.hpp"
#include "S_MPU6050.hpp"
#include "S_BMP388.hpp"
#include "S_GPS_NMEA.hpp"
#include "wiring_private.h" // for pinPeripheral();


using namespace TeleMax;

bool TeleMax::heartbeat_toggle = false;
unsigned long TeleMax::loopTime = 0;
//UBLOX gps(Serial1);
Adafruit_GPS gps(&Serial1);
Output TeleMax::debugLed = Output(SYSTEM_LED_PIN);
Output TeleMax::systemLed = Output(SYSTEM2_LED_PIN);
Output TeleMax::buzzer = Output(BUZZER_PIN, true);
unsigned long TeleMax::voltageReadTime = 0;
float TeleMax::voltage = 0;


void setup(void){
	/*** Configure pins ***/
	// SD
	pinMode(SD_CS_PIN, OUTPUT);
	digitalWrite(SD_CS_PIN, HIGH);
	pinMode(SD_CD_PIN, INPUT_PULLUP);
	// Telemetry
	pinMode(RF95_CS_PIN, OUTPUT);
	digitalWrite(RF95_CS_PIN, HIGH);
	pinMode(RF95_RST_PIN, OUTPUT);
	digitalWrite(RF95_RST_PIN, HIGH);
	pinMode(RF95_IRQ0_PIN, INPUT);
	pinMode(RF95_IRQ1_PIN, INPUT);
	// Accell
	pinMode(MPU6050_INT_PIN, INPUT);
	// Baro
	pinMode(BMP388_CS_PIN, OUTPUT);
	digitalWrite(BMP388_CS_PIN, HIGH);
	digitalWrite(BMP388_INT_PIN, HIGH);
	// GPIO
	pinMode(VOLTAGE_PIN, INPUT);
	pinMode(STAGESEPERATION_PIN, INPUT_PULLUP);

	buzzer.turnOff();
	debugLed.blocking({1,0,1,0,1,0,1,0});

	/*** Start the bus ***/
	Wire.begin();
	SPI.begin();

	/*** Start logging ***/
	Logging::init();
	Logging::log(3, "-Adding sensors.\n");

	buzzer.turnOn();

	/*** Add sensor ***/
	SensorGroup::sensors = NULL;
	SensorGroup::sensorCount = 0;
	SensorGroup::addSensor(new S_MPU6050());
	SensorGroup::addSensor(new S_BMP388());
	//SensorGroup::addSensor(new S_GPS()); // Old UBLOX gps
	SensorGroup::addSensor(new S_GPS_NMEA()); // NMEA
	
	/*** Set initial values ***/
	Logging::log(3, "-Setting initial values.\n");
	Staging::nextMeasurementTime=0;
	voltageReadTime=0;
	Logging::log(1, "-Init delay(sec): ");
	{
		unsigned int x;
		for(x=INIT_DELAY; x>0; --x){
			Logging::log(3, String(x) + " ");
			debugLed.turnOn();
			delay(500);
			debugLed.turnOff();
			delay(500);
		}
	};
	Logging::log(3, ". Delay complete.\n");
	debugLed.repeat({1,1,0});
	
	/*** Output some values for the logs ***/
	Logging::log(2, "-Measurement Rate (Hz): " + String(LOG_RATE) + "\n");
};

void loop(void){
	// Update execution timer
	loopTime = micros();
	// Execute staging and sensor commands
	Staging::staging();
	SensorGroup::tick();
	// Update outputs
	heartbeat();
	// Check for inputs
	#if LOG_USB>0
	if(Serial.available()>0)
		Serial.read();
	#endif
	// Misc.
	if(voltageReadTime<TeleMax::loopTime){
		Logging::telemetryData->data.voltage = analogRead(VOLTAGE_PIN)*330/1024;
		Logging::log(2, "C:" + String(TeleMax::loopTime) + "," + String(Staging::stage) + ",0,0," + String(Logging::telemetryData->data.voltage/50) + "\n");
		voltageReadTime = TeleMax::loopTime+10000000; // Every 10 seconds.
		
	}
};

inline void TeleMax::heartbeat(){
	debugLed.tick();
	buzzer.tick();
	systemLed.tick();
};

