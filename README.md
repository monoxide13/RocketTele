
# RocketTele
Arduino based live rocket telemetry and recording. There are two separate systems that can be used.
 
**DataRec**
...Work in progress...
This is a single on-board system that only records accel and baro data on flash/EEPROM. It is based on a SparkFun 5v Pro Micro Arduino. A switch is used to set to record or replay the flight.
Good for short flights that there is some concern for DSE (Detectable Seismic Event). I use it when testing parachutes and parachute deployment.
After turning on and initializing, there is a 15 second delay, 5 seconds of zeroing sensors, then ready to launch.

Serial Commands:
	* set: Sets config values. Usage: "set [config] [value]"
	* dump: Displays all configs. Usage: "dump"
	* replay: Outputs flight info.

Config values are stored on the arduino. Reprogramming the arduino resets the configs.
Configs:
	* a_start: The minimum positive acceleration triggering start of launch.
	* b_start: The minimum change in barometric pressure that must hit to trigger launch.
If either g_start or b_start are triggered, it will start recording. 0=disabled.

Dump output format:
	[time],a_net,g_x,g_y,g_z\n

![alt text](https://cdn.sparkfun.com/assets/9/c/3/c/4/523a1765757b7f5c6e8b4567.png "SparkFun 5v Pro Micro Arduino Pinout")

MPU6050 calibration data.
XAccel			YAccel				ZAccel			XGyro			YGyro			ZGyro
[-423,-422] --> [-9,8]	[-2703,-2702] --> [-10,5]	[1161,1162] --> [16378,16391]	[-12,-11] --> [0,2]	[-54,-53] --> [0,2]	[20,21] --> [-2,1]
[-423,-422] --> [-8,4]	[-2699,-2698] --> [-12,6]	[1129,1130] --> [16348,16386]	[-17,-16] --> [-4,1]	[-70,-69] --> [0,3]	[29,30] --> [-1,1]
	

I2Cdev and MPU6050 code attributed to jrowberg.

**TeleMax**
...Work in progress...
This is a two part system. Part one is the rocket part, including transmitter. Part two is the reciever.

Optional defines.
- OUTPUT_TELE : Enables use of transmitter/reciever setup.
- OUTPUT_SD : Enables logging to SD card.
- OUTPUT_USB : Enables serial output via USB (used mainly for testing).

System operation is a 8 step process.
1. Check if SD is inserted. If so, initialize and enable logging. If telemetry enabled, initialize and begin transmitting.
2. Delay for 30 seconds to allow for all hatches to be closed and rocket to be stable on launch pad.
	- TODO: Alternative method, wait for telemetry to trigger start-up. Use "#define USE_MANUALSTART". 
3. Start and calibrate sensors. Wait until they are ready.
4. Wait for launch.
5. Rocket on ascent.
6. Apogee
7. Rocket on Descent.
8. Flight over.
	
Telemetry outputs are:
- C[StageNumber]
- S[SensorNumber][SensorStatus]
- H[satelites in use][GPS Accuracy]
- G[LAT][LONG][GPSALT]
- B[baroAlt]
- A[zAccelReferenceToRocket][xGyro][yGyro][zGyro]
- E[rawXAccel][rawYAccel][rawZAccel][rawXGyro][rawYGyro][rawZGyro]
- T[rawBaro][Temp][DewPoint]
- -[Comment]

Sensors are coded as:
<ol start="0">
  <li>barometer</li>
  <li>accelerometer</li>
  <li>GPS</li>
  <li>SD</li>
  <li>Telemetry</li>
</ol>

Serial outputs are all not parsable, and all plain comments.

The rocket part consists of a [Adafruit Feather M0 Logger](https://www.adafruit.com/product/2796), [GPS receiver](https://www.adafruit.com/product/790), [accelerometer](https://www.banggood.com/6DOF-MPU-6050-3-Axis-Gyro-With-Accelerometer-Sensor-Module-For-Arduino-p-80862.html?rmmds=myorder&cur_warehouse=USA), [barometer](https://www.banggood.com/MS5611-GY-63-Atmospheric-Pressure-Sensor-Module-IICSPI-Communication-p-965980.html?rmmds=myorder&cur_warehouse=CN), and tx/rx for telemetry (see below).
![alt text](https://cdn-learn.adafruit.com/assets/assets/000/046/243/original/adafruit_products_Feather_M0_Adalogger_v2.2-1.png? "AdaFruit Feather M0 Logger Pinout")
The base site consists of another serial transmitter and an arduino to control the transmitter and forward the serial log.

Adafruit libraries are both unused currently, and taken from Adafruit's github.
MPU6050 library is taken from [jrowberg](https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050).
 - I did have to make a few changes to the MPU6050 lib. Changes were added to MPU6050.cpp.
 - I added "#define BUFFER_LENGTH SERIAL_BUFFER_SIZE" to convert it from AVR to ARM phrasing.
 - I included the [dtostrf](https://forum.arduino.cc/index.php?topic=632384.0) function.
HC12 library is pieced together from many places, including much of my own work. *No longer used*
RFM9x library is from [mikem](https://airspayce.com/mikem/arduino/RadioHead/).
MS5611 library was written by [keionbis](https://github.com/keionbis/ms5611_SPI), with it being reworked by myself.
UBLOX library was done by [bolderflight](https://github.com/bolderflight/UBLOX).
LCD_I2C library was taken from [joearmstrong980](https://gitlab.com/joearmstrong980/LCD_I2C) on gitlab.
I2Cdev, SD, SPI are all Arduino libraries.

Reference Material:
https://shanetully.com/2016/07/inside-the-construction-of-an-amateur-rocketry-flight-computer/
