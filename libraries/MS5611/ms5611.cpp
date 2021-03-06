/* 
 * Original code had delay(1) after the digitalWrite(_cs, LOW);
 */

#include "ms5611.h"

MS5611::MS5611(int cs) {
  _cs = cs;
  n_crc = 0;
  stationPressure = 1013.251; // Offset because if initially called at .25 it will return infinite altitude.
  resetComplete = false;
}
unsigned char MS5611::init() {
  pinMode(_cs, OUTPUT);
  digitalWrite(_cs, HIGH);
  if(!resetComplete)
    cmd_reset(); // reset the module after powerup
  for (int i = 0; i < 8; i++) {
    C[i] = cmd_prom(i); // read calibration coefficients
  }
  return (n_crc = crc4(C));
}

unsigned long MS5611::cmd_adc(char cmd) {
  SPI.beginTransaction(SPISettings (MS5611_SPI_CLOCK, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(CMD_ADC_CONV + cmd);
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  return 0;
 };
 /*
  switch (cmd & 0x0f) // wait necessary conversion time
  {
    
    case CMD_ADC_256 : delayMicroseconds(600); break;
    case CMD_ADC_512 : delayMicroseconds(1170); break;
    case CMD_ADC_1024: delayMicroseconds(2280); break;
    case CMD_ADC_2048: delayMicroseconds(4540); break;
    case CMD_ADC_4096: delayMicroseconds(9040); break;
	*/
    /*
    case CMD_ADC_256 : delay(1); break;
    case CMD_ADC_512 : delay(2); break;
    case CMD_ADC_1024: delay(3); break;
    case CMD_ADC_2048: delay(5); break;
    case CMD_ADC_4096: delay(10); break;
	}
    */
unsigned long MS5611::getReading(){
  unsigned long temp;
  unsigned int ret;
  SPI.beginTransaction(SPISettings (MS5611_SPI_CLOCK, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(CMD_ADC_READ);
  ret = SPI.transfer(0x00);
  temp = ret << 8;
  ret = SPI.transfer(0x00);
  temp = temp | ret;
  temp = temp << 8;
  ret = SPI.transfer(0x00);
  temp = temp | ret;
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  return temp;
}

unsigned long MS5611::getPressure(char resolution) {
  return cmd_adc(CMD_ADC_D1 + resolution);
}

double MS5611::calculatePressureCompensation(unsigned long pressure, unsigned long temperature) {
  double P = 0;
  double dT = temperature - C[5] * pow(2, 8);
  double OFF = C[2] * pow(2, 16) + C[4] * dT / pow(2, 7);
  double SENS = C[1] * pow(2, 15) + C[3] * dT / pow(2, 8);
  P = (pressure * SENS / pow(2, 21) - OFF) / pow(2, 15);
  
  return P/100;
}

unsigned long MS5611::getTemperature(char resolution) {
  return cmd_adc(CMD_ADC_D2 + resolution);
}

double MS5611::calculateTemperatureCompensation(unsigned long temperature) {
  double T = 0;
  double dT = temperature - C[5] * pow(2, 8);
  T = 2000 + (dT * C[6]) / pow(2, 23);
  // The following lines are part of temp calculation, but are not used.
  //double OFF = C[2] * pow(2, 16) + dT * C[4] / pow(2, 7);
  //double SENS = C[1] * pow(2, 15) + dT * C[3] / pow(2, 8);
  return T/100;
}

double MS5611::calculateSecondOrderTemperatureCompensation(unsigned long temperature){
  double TEMP, dT, OFF, SENS, T2, OFF2, SENS2 = 0;
  dT = temperature - C[5] * pow(2, 8);
  TEMP = 2000 + dT * C[6] / pow(2, 23);
  OFF = C[2] * pow(2, 16) + dT * C[4] / pow(2, 7);
  SENS = C[1] * pow(2, 15) + dT * C[3] / pow(2, 8);
  if(TEMP<20){
      T2 = pow(dT, 2) / pow(2, 31);
      OFF2 = 5 * pow(TEMP-2000, 2) / pow(2, 1);
      SENS2 = 5 * pow(TEMP-2000, 2) / pow(2, 2);
      if(TEMP<-15){
          OFF2 = OFF2 + 7 * pow(TEMP+1500, 2);
          SENS2 = SENS2 + 11 * pow(TEMP+1500, 2) / pow(2, 1);
      }
  }
    TEMP = TEMP - T2;
    OFF = OFF - OFF2;
    SENS = SENS - SENS2;
    return TEMP/100;
}

void MS5611::cmd_reset()
{   
  SPI.beginTransaction(SPISettings (MS5611_SPI_CLOCK, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(CMD_RESET); // send reset sequence
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  delay(10); // Need delay here to finish reboot!
  resetComplete=true;
}

unsigned int MS5611::cmd_prom(char coef_num)
{
#ifdef DEBUGSERIAL
  Serial.print("PROM "); Serial.print((uint8_t)coef_num, DEC); Serial.print(": ");
#endif
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  digitalWrite(_cs, LOW);
  SPI.transfer(CMD_PROM_RD + ((uint8_t)coef_num) * 2); // send PROM READ command
  unsigned int ret;
  unsigned int rC = 0;
  ret = SPI.transfer(0x00); // send 0 to read the MSB
#ifdef DEBUGSERIAL
  Serial.print(ret, HEX);
#endif
  rC = ret << 8;
  ret = SPI.transfer(0x00); // send 0 to read the LSB
#ifdef DEBUGSERIAL
  Serial.print(ret, HEX);
#endif
  rC = rC | ret;
#ifdef DEBUGSERIAL
  Serial.print(" : ");
  Serial.println(rC);
#endif
  digitalWrite(_cs, HIGH);
  SPI.endTransaction();
  return rC;
}

unsigned char MS5611::crc4(unsigned int n_prom[])
{
  int cnt; // simple counter
  unsigned int n_rem; // crc reminder
  unsigned int crc_read; // original value of the crc
  unsigned char n_bit;
  n_rem = 0x00;
  crc_read = n_prom[7]; //save read CRC
  n_prom[7] = (0xFF00 & (n_prom[7])); //CRC byte is replaced by 0
  for (cnt = 0; cnt < 16; cnt++) // operation is performed on bytes
  { // choose LSB or MSB
    if (cnt % 2 == 1) n_rem ^= (unsigned short) ((n_prom[cnt >> 1]) & 0x00FF);
    else n_rem ^= (unsigned short) (n_prom[cnt >> 1] >> 8);
    for (n_bit = 8; n_bit > 0; n_bit--)
    {
      if (n_rem & (0x8000))
      {
        n_rem = (n_rem << 1) ^ 0x3000;
      }
      else
      {
        n_rem = (n_rem << 1);
      }
    }
  }
  n_rem = (0x000F & (n_rem >> 12)); // // final 4-bit reminder is CRC code
  n_prom[7] = crc_read; // restore the crc_read to its original place
  return (n_rem ^ 0x00);
}

double MS5611::setStationPressure(double pressure){
	// Should be compensated pressure
    return (stationPressure = pressure);
}

double MS5611::convertMBtoMeters(double mb){
     return convertMBtoFt(mb)*.3048;
}
double MS5611::getAltitude(double pressure){
    return (1-pow(pressure/stationPressure, 0.190284))*145366.45;
}
double MS5611::getAltitude(unsigned long pressure, unsigned long temperature){
    return (1-pow(calculatePressureCompensation(pressure, temperature)/stationPressure, 0.190284))*145366.45;
}
double MS5611::convertMBtoFt(double mb){
    return (1-pow(mb/1013.25, 0.190284))*145366.45; // Formula from NOAA
}
double MS5611::convertCtoF(double temp){
    return temp*9/5+32;
}
