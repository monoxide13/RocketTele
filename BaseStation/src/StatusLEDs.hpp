/* StatusLEDs.hpp */

// This class controls the 4 WS2812s that display status and info.

/* Vertical Velocity ** UNUSED ** GPS Status ** RX Strength */

#pragma once

#define ADAFRUIT_FEATHER_M0 1
#include "Adafruit_NeoPixel_ZeroDMA.h"

namespace StatusLEDs{
	extern bool needsUpdate;
	extern Adafruit_NeoPixel_ZeroDMA * neo;
	void initialize();
	void test();
	void tick();
	namespace {
		float previousRX;
		float previousGPS;
		float previousVV;
	}
#define VV_MAX 100
#define VV_MIN 1
	inline void setVV(float value){
	// Using HSV values. 0 is Red, 240 is Blue. S and V at max.
		if(value>VV_MAX)
			value=VV_MAX;
		else if(value<VV_MIN)
			value=VV_MIN;
		if(value==previousVV)
			return;
		neo->setPixelColor(0, neo->gamma32( \
			neo->ColorHSV(43690 - round((value-VV_MIN)/(VV_MAX-VV_MIN)*43690), 255, 255)));
	};
#define GPS_MAX 20
#define GPS_MIN 1
	inline void setGPS(float value){
		if(value>GPS_MAX)
			value=GPS_MAX;
		if(value==previousGPS)
			return;
		if(value==-1) // No fix!
			neo->setPixelColor(2, 0); // Off
		else{ // Value based on HDOP
			if(value<GPS_MIN)
				value=GPS_MIN;
			if(value>GPS_MAX)
				value=GPS_MAX;
		neo->setPixelColor(1, neo->gamma32( \
			neo->ColorHSV(round((value-GPS_MIN)/(GPS_MAX-GPS_MIN)*45510), 255, 255)));
		}
		needsUpdate=true;
	};
#define RX_MAX 15
#define RX_MIN -10
	inline void setRX(float value){
		if(value==-100){ // No packet in a while, turn off LED.
			neo->setPixelColor(3, 0);
			previousRX=-100;
			needsUpdate=true;
			return;
		}
	// Using HSV values. 0 is Red, 240 is Blue. S and V at max.
		if(value>RX_MAX)
			value=RX_MAX;
		else if(value<RX_MIN)
			value=RX_MIN;
		if(value==previousRX)
			return;
		neo->setPixelColor(3, neo->gamma32( \
			neo->ColorHSV(round((value-RX_MIN)/(RX_MAX-RX_MIN)*43690), 255, 255)));
		needsUpdate=true;
	};
};
