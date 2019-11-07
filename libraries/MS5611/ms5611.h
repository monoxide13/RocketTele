/*
 * ms5611.h
 *
 *  Created on: Jun 26, 2018
 *      Author: kbisland
 */
#define DEBUGSERIAL

#ifndef SRC_MS5611_H_
#define SRC_MS5611_H_
#include <SPI.h>

#define CMD_RESET 0x1E // ADC reset command
#define CMD_ADC_READ 0x00 // ADC read command
#define CMD_ADC_CONV 0x40 // ADC conversion command
#define CMD_ADC_D1 0x00 // ADC D1 conversion
#define CMD_ADC_D2 0x10 // ADC D2 conversion
#define CMD_ADC_256 0x00 // ADC OSR=256
#define CMD_ADC_512 0x02 // ADC OSR=512
#define CMD_ADC_1024 0x04 // ADC OSR=1024
#define CMD_ADC_2048 0x06 // ADC OSR=2056
#define CMD_ADC_4096 0x08 // ADC OSR=4096
#define CMD_PROM_RD 0xA0 // Prom read command
#define MS5611_SPI_CLOCK 16000000 // SPI clock speed


class MS5611 {
private:
	int _cs;
	unsigned int C[8]; // calibration coefficients
	unsigned char n_crc; // crc value of the prom
	unsigned char resetComplete;
	double stationPressure;
public:
	MS5611(int);
	unsigned char init();
	unsigned long cmd_adc(char) ;
	unsigned long getPressure(char);
	unsigned long getTemperature(char);
	void cmd_reset();
	unsigned int cmd_prom(char coef_num);
	unsigned char crc4(unsigned int n_prom[]);
	double getTemperatureCompensated(char);
	double getPressureCompensated(char);
	double getSecondOrderTemperatureCompensated(char);
	double setStationPressure();
	double getAltitude();
	double getAltitude(char);
	double convertMBtoMeters(double);
	double convertMBtoFt(double);
	double convertCtoF(double);
};

#endif /* SRC_MS5611_H_ */
