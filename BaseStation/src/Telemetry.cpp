/* Telemetry.cpp */

#include "Telemetry.hpp"
#include "global.hpp"

Telemetry::Telemetry(){
	RH_RF95 * downlink;
	for(snrIter=0; snrIter<SNR_HYSTERESIS; ++snrIter){
		snrArray[snrIter]=0;
	}
	snrIter=0;
};

Telemetry::~Telemetry(){
	free(downlink);
};

int Telemetry::init(){
	downlink = new RH_RF95(RF95_CS_PIN, RF95_IRQ0_PIN);
	if(!downlink->init()){
		Serial.println("+BaseStation downlink error!");
		return 1;
	}
	Serial.print("+BaseStation downlink version:");
	Serial.println((uint8_t)downlink->getDeviceVersion());
    downlink->setModemConfig(RH_RF95::Bw500Cr45Sf128);
	downlink->setTxPower(2);
	downlink->setModeRx();
	return 0;
};

bool Telemetry::receive(){
	rxLength = RX_BUFFER_LENGTH;
	if(downlink->recv((uint8_t*)rxBuffer, (uint8_t*)&rxLength)){
    	Serial.write(rxBuffer, rxLength);
    }
	snrArray[snrIter]=downlink->lastSNR();
	if(++snrIter>=SNR_HYSTERESIS)
		snrIter=0;
	return true;
};

int Telemetry::getSNR(){
	int x, sum=0;
	for(x=0; x<SNR_HYSTERESIS; ++x){
		sum+=snrArray[x];
	}
	return floor(sum/SNR_HYSTERESIS);
};
