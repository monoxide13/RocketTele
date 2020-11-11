///// TeleMax.cpp /////
#include "Arduino.h"
#include "global.hpp"
#include "TeleMax.hpp"
#include "SensorGroup.hpp"
#include "Logging.hpp"
#include "Staging.hpp"
#include "S_MPU6050.hpp"
#include "S_MS5611.hpp"
#include "S_GPS.hpp"
#include "wiring_private.h" // for pinPeripheral();


using namespace TeleMax;
using namespace Staging;

Uart gpsPort(&sercom1, 10, 11, SercomRXPad::SERCOM_RX_PAD_0, SercomUartTXPad::UART_TX_PAD_2);

bool TeleMax::heartbeat_toggle=false;
unsigned long TeleMax::loopTime=0;
UBLOX gps(gpsPort);
Output TeleMax::debugLed = Output(SYSTEM_LED_PIN);
Output TeleMax::systemLed = Output(SYSTEM2_LED_PIN);
Output TeleMax::buzzer = Output(BUZZER_PIN, true);
unsigned long TeleMax::voltageReadTime=0;
float TeleMax::voltage=0;


void setup(void){
	/*** Configure pins ***/
	pinMode(STAGESEPERATION_PIN, INPUT_PULLUP);
	pinMode(SD_CS_PIN, OUTPUT);
	digitalWrite(SD_CS_PIN, HIGH);
	pinMode(SD_CD_PIN, INPUT_PULLUP);
	pinMode(HC12_SET_PIN, OUTPUT);
	pinMode(MPU6050_INT_PIN, INPUT);
	pinMode(MS5611_CS, OUTPUT);
	digitalWrite(MS5611_CS, HIGH);
	pinMode(VOLTAGE_PIN, INPUT);

	buzzer.turnOff();
	debugLed.blocking({1,0,1,0,1,0,1,0});

	/*** Start the bus ***/
	Wire.begin();

	/*** Start logging ***/
	Logging::init();
	Logging::log(3, "-Adding sensors.\n");
	buzzer.turnOn();

	/*** Add sensor ***/
	SensorGroup::sensors = NULL;
	SensorGroup::sensorCount = 0;
	SensorGroup::addSensor(new S_MPU6050());
	SensorGroup::addSensor(new S_MS5611());
	SensorGroup::addSensor(new S_GPS());
	gpsPort.begin(19200);
	pinPeripheral(10, PIO_SERCOM); // TX
	pinPeripheral(11, PIO_SERCOM); // RX
	

	/*** Set initial values ***/
	Logging::log(3, "-Setting initial values.\n");
	stage=STAGE_INIT;
	stageChange=true;
	loopCounter=0;
	Staging::nextMeasurementTime=0;
	voltageReadTime=0;
	Logging::log(3, "-Init delay(sec): ");
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
	debugLed.blocking({1,1,0,0,1,1,0,0,1,1,0,0});
	
	/*** Output some values for the logs ***/
	Logging::log(3, "-Measurement Rate (Hz): " + String(LOG_RATE) + "\n");
};

void loop(void){
	++loopCounter;
	loopTime = micros();
	if(stageChange){
		switch(stage){
			case STAGE_PRELAUNCH:
				stagePtr = &stage_STAGE_PRELAUNCH;
				break;
			case STAGE_THRUST:
				stagePtr = &stage_STAGE_THRUST;
				break;
			case STAGE_COAST:
				stagePtr = &stage_STAGE_COAST;
				break;
			case STAGE_APOGEE:
				stagePtr = &stage_STAGE_APOGEE;
				break;
			case STAGE_DESCENT:
				stagePtr = &stage_STAGE_DESCENT;
				break;
			case STAGE_RECOVERY:
				stagePtr = &stage_STAGE_RECOVERY;
				break;
			default:
				stagePtr = &stage_STAGE_INIT;
				break;
		}
		Logging::log(3, "-Stage changed: " + String(stage) + "\n");
		stageChange=false;
		loopCounter=0;
	}
	if(voltageReadTime<TeleMax::loopTime){
		Logging::log(1, "C:" + String(TeleMax::loopTime) + "," + String(stage) + ",0,0," + String(analogRead(VOLTAGE_PIN)*6.6/1024) + "\n");
		voltageReadTime=TeleMax::loopTime+10000000; // Every 10 seconds.
	}
	heartbeat();
	stagePtr();
};

inline void TeleMax::heartbeat(){
	SensorGroup::tick();
	debugLed.tick();
	buzzer.tick();
	systemLed.tick();
};

void SERCOM1_Handler(){
	gpsPort.IrqHandler();
};

