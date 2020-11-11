///// Filter1.hpp /////

#pragma once

#include "Filter.hpp"

template <typename T>
class Filter1 : public Filter<T>{
	public:
	Filter1();
	~Filter1();
	void addReading(T) override;
	T getReading() override;
	void reset() override;
	private:
	double last_reading;
	double last_average;
	double error;
	bool resetValues;
};
