/* Output.hpp */

#pragma once

#include <list>

#define OUTPUT_SWITCH_TIME 500 // In milliseconds

class Output{
	public:
	Output(unsigned short, bool iinverted=false);
	~Output();
	void tick();
	void blocking(std::list<bool>);
	void nonBlocking(std::list<bool>);
	void repeat(std::list<bool>);
	void turnOn();
	void turnOff();
	private:
	bool state;
	const bool inverted;
	bool repeating;
	const unsigned short pin;
	unsigned long nextTickTime;
	std::list<bool> commands;
};
