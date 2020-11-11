///// Filter1.cpp /////

#include "Filter1.hpp"

template <typename T>
Filter1<T>::Filter1(){
	reset();
};

template <typename T>
Filter1<T>::~Filter1(){};

template <typename T>
void Filter1<T>::reset(){
	resetValues=true;
};

template <typename T>
void Filter1<T>::addReading(T readValue){
};

template <typename T>
T Filter1<T>::getReading(){
	T value=0;
	return value;
};
