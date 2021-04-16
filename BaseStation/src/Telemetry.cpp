/* Telemetry.cpp */

#include "Telemetry.hpp"
#include "global.hpp"

Telemetry::Telemetry(){
	RH_RF95 * downlink;
};

Telemetry::~Telemetry(){
	free(downlink);
};

int Telemetry::init(){
	downlink = new RH_RF95(RF95_CS_PIN, RF95_IRQ0_PIN);
	if(!downlink->init()){
		Serial.println("BaseStation downlink error!");
		return 1;
	}
	downlink->setTxPower(2);
	downlink->setModeRx();
	return 0;
}
bool Telemetry::receive(){
	rxLength = RX_BUFFER_LENGTH;
	downlink->recv((uint8_t*)rxBuffer, (uint8_t*)&rxLength);
	Serial.write(rxBuffer, rxLength);
	return true;
}
