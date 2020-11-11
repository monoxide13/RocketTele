///// Filter.cpp /////

#include "Filter.hpp"

template <typename T>
Filter<T>::Filter(){

};

template <typename T>
Filter<T>::~Filter(){};

template <typename T>
void Filter<T>::addReading(T value){
	lastReading = value;
};

template <typename T>
T Filter<T>::getReading(){
	return lastReading;
};

template <typename T>
void Filter<T>::reset(){
	lastReading=0;
};
