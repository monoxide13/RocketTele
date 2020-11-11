///// Output.cpp /////

#include "Output.hpp"
#include "Arduino.h"
#include "TeleMax.hpp"

Output::Output(unsigned short ipin, bool iinverted){
	//pin=ipin;
	//inverted=iinverted;
	state=false;
	pinMode(pin=ipin, OUTPUT);
	digitalWrite(pin, inverted=iinverted);
	nextTickTime=0;
	repeating=false;
};

Output::~Output(){

};

void Output::tick(){
	if(commands.size() && TeleMax::loopTime>nextTickTime){
		digitalWrite(pin, !inverted != !commands.front());
		state=commands.front();
		if(repeating){
			commands.push_back(commands.front());
			commands.pop_front();
		}
	nextTickTime = TeleMax::loopTime + OUTPUT_SWITCH_TIME;
	}

};

void Output::nonBlocking(std::list<bool> input){
	repeating=false;
	commands=input;
	nextTickTime=TeleMax::loopTime;
};

void Output::blocking(std::list<bool> input){
	while(!input.empty()){
		digitalWrite(pin, !inverted != !input.front());
		state = input.front();
		input.pop_front();
		delay(OUTPUT_SWITCH_TIME);
	}
	nextTickTime = TeleMax::loopTime + OUTPUT_SWITCH_TIME;
};
void Output::repeat(std::list<bool> input){
	repeating=true;
	commands.clear();
	//commands.splice(commands.begin(), input);
	commands=input;
};
void Output::turnOn(){
	state=true;
	digitalWrite(pin, !inverted);
	repeating=false;
	commands.clear();
};
void Output::turnOff(){
	state=false;
	digitalWrite(pin, inverted);
	repeating=false;
	commands.clear();
}
