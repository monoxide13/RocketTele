# RocketTele
Arduino based live rocket telemetry and recording.

Short Term Goals:
	Store accel/gyro and baro data to DS24B33 eeprom. Trigger launch with accel.
	
Pinout... A better pdf can be found from [Sparkfun](https://cdn.sparkfun.com/assets/9/c/3/c/4/523a1765757b7f5c6e8b4567.png)
![alt text](https://cdn.sparkfun.com/assets/9/c/3/c/4/523a1765757b7f5c6e8b4567.png "SparkFun 5v Pro Micro Arduino Pinout")
USB Connection on top
| Connection | Arduino Pin | Arduino Pin | Connection |
|---|---|---|---|
| NC | D1/TX1 | RAW 6v-16v | 6.6v LiPo |
| NC | D0/RX1 | GND | GND |
| NC | GND | RST | NC |
| SPKR- | GND | Vcc | NC |
| SDA | SDA/D2 | A3 | NC |
| SCL | SCL/D3 | A2 | NC |
| BTTN | D4/A6 | A1 | NC |
| EEPROM | D5 | A0 | NC |
| SPKR+ | D6/A7 | D15 | SCK |
| ACCEL INT | D7/INT6 | D14 | MISO |
| CS PRES | D8/A8 | D16 | MOSI |
| LED | D9/A9 | D10/A10 | CS MEM |

	
	
MPU6050 calibration data.
XAccel			YAccel				ZAccel			XGyro			YGyro			ZGyro
[-423,-422] --> [-9,8]	[-2703,-2702] --> [-10,5]	[1161,1162] --> [16378,16391]	[-12,-11] --> [0,2]	[-54,-53] --> [0,2]	[20,21] --> [-2,1]
[-423,-422] --> [-8,4]	[-2699,-2698] --> [-12,6]	[1129,1130] --> [16348,16386]	[-17,-16] --> [-4,1]	[-70,-69] --> [0,3]	[29,30] --> [-1,1]
	
Long Term Goals:
Logging frequency 10Hz to SD card:
	Acceleration and Gyro data.
	Baro pressure.
	GPS.
TX via serial to ground RX 
	Altitude.
	GPS.

I2Cdev and MPU6050 code attributed to jrowberg.

Reference Material:
https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
https://shanetully.com/2016/07/inside-the-construction-of-an-amateur-rocketry-flight-computer/
