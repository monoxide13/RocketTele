/* Telemetry.cpp */

#include "Telemetry.hpp"
#include "global.hpp"
#include "BaseStation.hpp"
#include "packet_def.h"
#include "StatusLEDs.hpp"

#define TX_FREQ 434.3 // In MHz

uint32_t Ptimer;
uint16_t Pbalt;

Telemetry::Telemetry(){
	RH_RF95 * downlink;
	for(snrIter=0; snrIter<SNR_HYSTERESIS; ++snrIter){
		snrArray[snrIter]=0;
	}
	snrIter=0;
	offset=0;
	lastGoodTime=0;
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
	downlink->setFrequency(TX_FREQ);
    downlink->setModemConfig(RH_RF95::Bw500Cr45Sf128);
	downlink->setSpreadingFactor(10);
	downlink->setSignalBandwidth(125000);
	downlink->setCodingRate4(5);
	downlink->setTxPower(2);
	downlink->setModeRx();
	return 0;
};

bool Telemetry::receive(){
	rxLength = RX_BUFFER_LENGTH-offset;
	// offset is the first open writable slot.
	// rxLength is new bytes read.
	if(downlink->available() && downlink->recv((uint8_t*)(rxBuffer+offset), &rxLength) && rxLength>0){
		offset+=rxLength;
		bool keepLooping;
		unsigned short x;
		do{
			keepLooping = false;
			if(offset >= TELEMETRY_PACKET_LENGTH+2){
				if(rxBuffer[0]=='T' && rxBuffer[1]==':'){
					keepLooping = true;
					if(checkPacket((Telemetry_Packet *)rxBuffer)){
						lastGoodTime = millis();
						// Packet is valid, process and shift.
						processPacket((Telemetry_Packet *)rxBuffer);
		   				Serial.write((uint8_t*)rxBuffer, TELEMETRY_PACKET_LENGTH+2);
				/*		 // Prints out packet in HEX.
						int x;
						Serial.flush();
						for(x=0; x<TELEMETRY_PACKET_LENGTH+2; ++x){
							if(rxBuffer[x]<0x10)
								Serial.print('0');
							Serial.print(rxBuffer[x], HEX);
							Serial.print(" ");
						}
						// */
						offset-=TELEMETRY_PACKET_LENGTH+2;
						memmove(rxBuffer, rxBuffer+TELEMETRY_PACKET_LENGTH+2, offset);
					}else{
						// Packet is invalid, destroy the identifier so we can purge it next loop.
						Serial.println("Destroying packet");
						rxBuffer[0]='\0';
					}
				}else{
					// Packet is long enough, but doesn't begin with T: so go to next T.
					for(x=1; x<offset; ++x){
						if(rxBuffer[x]='T'){
							keepLooping = true;
		   					Serial.write((uint8_t*)rxBuffer, x);
			/*				 // Prints out packet in HEX.
							Serial.println("2");
						Serial.flush();
							int y;
							for(y=0; y<TELEMETRY_PACKET_LENGTH+2; ++y){
								if(rxBuffer[y]<0x10)
									Serial.print('0');
								Serial.print(rxBuffer[y], HEX);
								Serial.print(" ");
							}
							// */
							offset-=x;
							memmove(rxBuffer, rxBuffer+x, offset);
							break;
						}
					}
				}
			}
		}while(keepLooping);

		//RH_RF95::printBuffer("Buffer:", (uint8_t*)rxBuffer, test);
		//Serial.println("Length: " + String(test));
		snrArray[snrIter]=downlink->lastSNR();
		Serial.print("R:");
		Serial.println(snrArray[snrIter]);
		StatusLEDs::setRX(getSNR());
		if(++snrIter>=SNR_HYSTERESIS)
			snrIter=0;
		BaseStation::debugText = String(downlink->rxGood()) + ":" + String(downlink->rxBad());
	}
	return true;
};

float Telemetry::getSNR(){
	int x, sum=0;
	for(x=0; x<SNR_HYSTERESIS; ++x){
		sum+=snrArray[x];
	}
	return sum/SNR_HYSTERESIS;
};

bool Telemetry::checkPacket(Telemetry_Packet * ptr){
	// TODO: Validate CRC.
	return true;
}

void Telemetry::processPacket(Telemetry_Packet * ptr){
	if(ptr->data.fixqual && 0b11000000 == 0b00000000) // noFix
		StatusLEDs::setGPS(-1);
	else
		StatusLEDs::setGPS(ptr->data.hdop);
	// Calculate Vertical velocitiy in m/s
	float change = (ptr->data.timer-Ptimer) / (ptr->data.balt-Pbalt) * 10; // *10 to convert 1/10sec to secs.
	Serial.print("+Vertical Velocity in m/s:");
	Serial.println(change);
}
