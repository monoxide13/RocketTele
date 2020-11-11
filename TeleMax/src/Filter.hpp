///// Filter.hpp /////

#pragma once

template <typename T>
class Filter{
	public:
	Filter();
	~Filter();
	void addReading(T);
	T getReading();
	protected:
	T lastReading;
	void reset();
	private:

};
