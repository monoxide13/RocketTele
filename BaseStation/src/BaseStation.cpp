/* BaseStation.cpp */

#include "Arduino.h"
#include "wiring_private.h" // for pinPeripheral();
#include "global.hpp"
#include "BaseStation.hpp"
#include "Output.hpp"


using namespace BaseStation;

Uart telePort(&sercom1, 10, 11, SercomRXPad::SERCOM_RX_PAD_0, SercomUartTXPad::UART_TX_PAD_2);
HC12 rocket = HC12(&telePort, HC12_SET_PIN);

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
	pinMode(VOLTAGE_PIN, INPUT);

	/*** Start the serial ports ***/
	Serial.begin(9600);

	/*** Set initial values ***/
	rocket.init();
	pinPeripheral(10, PIO_SERCOM); // TX
	pinPeripheral(11, PIO_SERCOM); // RX
};

void loop(void){
	loopTime = micros();
	heartbeat();
	rocket.receive();
	if(rocket.readReady){
		Serial.println(rocket.lastRead);
		rocket.readReady=false;
	}
};

inline void BaseStation::heartbeat(){
	readyLed.tick();
	debugLed.tick();
	systemLed.tick();
};

inline void SERCOM1_Handler(){
	telePort.IrqHandler();
};

