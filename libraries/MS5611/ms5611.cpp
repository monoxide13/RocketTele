/* C should be equal to for flat MS5611......
 *  248
 *  49992
 *  53205
 *  30432
 *  28925
 *  34580
 *  27081
 *  1911
 */

#include <MS5611.h>

MS5611::MS5611(int cs) {
  this->_cs = cs;
  this->n_crc = 0;
  resetComplete = false;
}
unsigned char MS5611::init() {
  pinMode(this->_cs, OUTPUT);
  digitalWrite(this->_cs, HIGH);
  if(!resetComplete)
    cmd_reset(); // reset the module after powerup
  for (int i = 0; i < 8; i++) {
    C[i] = cmd_prom(i); // read calibration coefficients
  }
  return (n_crc = crc4(C));
}

unsigned long MS5611::cmd_adc(char cmd) {
  SPISettings settingsA(10000000, MSBFIRST, SPI_MODE0);
  unsigned int ret;
  unsigned long temp;
  SPI.beginTransaction(settingsA);
  digitalWrite(this->_cs, LOW);
  delay(1); // Delay needed here!
  SPI.transfer(CMD_ADC_CONV + cmd);
  digitalWrite(this->_cs, HIGH);
  SPI.endTransaction();
  switch (cmd & 0x0f) // wait necessary conversion time
  {
    
    case CMD_ADC_256 : delayMicroseconds(600); break;
    case CMD_ADC_512 : delayMicroseconds(1170); break;
    case CMD_ADC_1024: delayMicroseconds(2280); break;
    case CMD_ADC_2048: delayMicroseconds(4540); break;
    case CMD_ADC_4096: delayMicroseconds(9040); break;
    /*
    case CMD_ADC_256 : delay(1); break;
    case CMD_ADC_512 : delay(2); break;
    case CMD_ADC_1024: delay(3); break;
    case CMD_ADC_2048: delay(5); break;
    case CMD_ADC_4096: delay(10); break;
    */
  }
SPI.beginTransaction(settingsA);
  digitalWrite(this->_cs, LOW);
  delay(1); // Delay needed here!
  SPI.transfer(CMD_ADC_READ);
  ret = SPI.transfer(0x00);
  temp = ret << 8;
  ret = SPI.transfer(0x00);
  temp = temp | ret;
  temp = temp << 8;
  ret = SPI.transfer(0x00);
  temp = temp | ret;
  digitalWrite(this->_cs, HIGH);
  SPI.endTransaction();
  return temp;
}

unsigned long MS5611::getPressure(char resolution) {
  return cmd_adc(CMD_ADC_D1 + resolution);
}

double MS5611::getPressureCompensated(char resolution) {
   /* long pressure = 0;
	double P = 0;
	pressure = cmd_adc(CMD_ADC_D1+resolution);
	double dT=getTemperature(resolution)-C[5]*pow(2,8);
	double OFF=C[2]*pow(2,17)+dT*C[4]/pow(2,6);
	double SENS=C[1]*pow(2,16)+dT*C[3]/pow(2,7);
	P=(((pressure*SENS)/pow(2,21)-OFF)/pow(2,15))/100;
	return P;
    */
  double P = 0;
  double dT = getTemperature(resolution) - C[5] * pow(2, 8);
  double OFF = C[2] * pow(2, 16) + C[4] * dT / pow(2, 7);
  double SENS = C[1] * pow(2, 15) + C[3] * dT / pow(2, 8);
  P = (getPressure(resolution) * SENS / pow(2, 21) - OFF) / pow(2, 15);
  
  return P/100;
}

unsigned long MS5611::getTemperature(char resolution) {
  return cmd_adc(CMD_ADC_D2 + resolution);
}

double MS5611::getTemperatureCompensated(char resolution) {
  unsigned long temperature = 0;
  double T = 0;
  double dT = getTemperature(resolution) - C[5] * pow(2, 8);
  T = 2000 + (dT * C[6]) / pow(2, 23);
  //double OFF = C[2] * pow(2, 16) + dT * C[4] / pow(2, 7);
  //double SENS = C[1] * pow(2, 15) + dT * C[3] / pow(2, 8);
  return T/100;
}

double MS5611::getSecondOrderTemperatureCompensated(char resolution){
  double TEMP, dT, OFF, SENS, T2, OFF2, SENS2 = 0;
  dT = getTemperature(resolution) - C[5] * pow(2, 8);
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
  digitalWrite(this->_cs, LOW); // pull CSB low to start the command
  delay(1);
  SPI.transfer(CMD_RESET); // send reset sequence
  delay(10); // wait for the reset sequence timing
  digitalWrite(this->_cs, HIGH); // pull CSB high to finish the command
  resetComplete=true;
  Serial.println("Reset complete");
}

unsigned int MS5611::cmd_prom(char coef_num)
{
#ifdef DEBUGSERIAL
  Serial.print("PROM "); Serial.print((uint8_t)coef_num, DEC); Serial.print(": ");
#endif
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  digitalWrite(this->_cs, LOW); // pull CSB low
  delay(2);  // Delay needed here!
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
  digitalWrite(this->_cs, HIGH); // pull CSB high
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

void MS5611::setStationPressure(){
    stationPressure = getPressureCompensated(CMD_ADC_4096);
}

double MS5611::convertMBtoMeters(double mb){
     return convertMBtoFt(mb)*.3048;
}
double MS5611::getAltitude(){
    return getAltitude(CMD_ADC_1024);
}
double MS5611::getAltitude(char resolution){
    return (1-pow(getPressureCompensated(resolution)/stationPressure, 0.190284))*145366.45;
}
double MS5611::convertMBtoFt(double mb){
    return (1-pow(mb/1013.25, 0.190284))*145366.45;
}
double MS5611::convertCtoF(double temp){
    return temp*9/5+32;
}