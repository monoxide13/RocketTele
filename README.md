
# RocketTele
Arduino based live rocket telemetry and recording. There are two separate systems that can be used.
 
**System one**
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

**System two**
...Work in progress...
This is a two part system. Part one is the rocket part, including transmitter. Part two is the reciever.
If you don't want to use the transmitter/reciever setup, comment out "#define OUTPUT_TELE".
If you don't want to use SD cards, comment out "#define OUTPUT_SD".
If you don't want to use serial usb output (used mainly for testing), comment out "#define OUTPUT_USB".

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
	

The rocket part consists of a [Adafruit Feather M0 Logger](https://www.adafruit.com/product/2796 "Adafruit Feather M0 Logger"), [GPS receiver](https://www.banggood.com/GPS-Module-with-Ceramic-Antenna-GPS-Receiver-TTL9600-Ublox-Module-for-Multirotor-p-1100984.html?rmmds=myorder&cur_warehouse=CN), [accelerometer](https://www.banggood.com/6DOF-MPU-6050-3-Axis-Gyro-With-Accelerometer-Sensor-Module-For-Arduino-p-80862.html?rmmds=myorder&cur_warehouse=USA), [barometer](https://www.banggood.com/MS5611-GY-63-Atmospheric-Pressure-Sensor-Module-IICSPI-Communication-p-965980.html?rmmds=myorder&cur_warehouse=CN), and [serial transmitter](https://www.banggood.com/HC-12-433-SI4463-Wireless-Serial-Module-Remote-1000M-With-Antenna-p-973522.html?rmmds=myorder&cur_warehouse=USA).
![alt text](https://cdn-learn.adafruit.com/assets/assets/000/046/243/original/adafruit_products_Feather_M0_Adalogger_v2.2-1.png? "AdaFruit Feather M0 Logger Pinout")
The base site consists of another serial transmitter and an arduino to control the transmitter and forward the serial log.


Reference Material:
https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
https://shanetully.com/2016/07/inside-the-construction-of-an-amateur-rocketry-flight-computer/
