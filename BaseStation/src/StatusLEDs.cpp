/* StatusLEDs.cpp */

#include "StatusLEDs.hpp"
#include <cmath>

using namespace StatusLEDs;

// NOTE: HSV, H goes 0-65535 not 0-360.

long timer=0;
// NeoPixels connected to pin 5.
Adafruit_NeoPixel_ZeroDMA * StatusLEDs::neo = new Adafruit_NeoPixel_ZeroDMA(4, 12, NEO_RGB);
//Adafruit_NeoPixel_ZeroDMA * StatusLEDs::neo;
//Adafruit_NeoPixel_ZeroDMA strip(4, MOSI, NEO_RGB);
bool StatusLEDs::needsUpdate=false;

void StatusLEDs::initialize(){
	if(!neo->begin())
		Serial.println("neo->begin() returned false");
	delay(100);
	neo->setBrightness(32);
	delay(100);
	neo->clear();
	delay(100);
	neo->setBrightness(32);
	neo->show();
	delay(100);
};

void StatusLEDs::test(){

	neo->fill(neo->Color(255, 0, 0));
	neo->show();
	delay(500);
	neo->fill(neo->Color(0, 0, 0));
	neo->show();
	delay(500);
	neo->fill(neo->Color(255, 0, 0));
	neo->show();
	delay(500);
	neo->fill(neo->Color(0, 0, 0));
	neo->show();
	delay(500);

};

void StatusLEDs::tick(){

	if(needsUpdate){
		neo->show();
		needsUpdate=false;
	}
};
