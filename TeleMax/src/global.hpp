// Global header file
// Define the chip for the CMSIS lib. Needed here? Unknown.

#pragma once
#define __SAMD21G18__ 
#define EXPANDSTRING(x) STRINGIFY(x)
#define STRINGIFY(x) #x

#include "packet_def.h"

/*----- LOGGING AND TELEMETRY -----*/
/* For logs and telemetry...
* 0 - No output. Will not include in build.
* 1 - Minimal. Output only what's necessary and important.
* 2 - Normal.
* 3 - Maximum. Use only for debugging.
*/
// Output telemetry over downlink port
#define LOG_DOWNLINK 1
// Output telemetry to SD card
#define LOG_SD 0
// Prefix filename for SD recording. 6 char max.
#define FILENAME_PREFIX LAUNCH
// Output telemetry to onboard USB serial port
#define LOG_USB 1
// Logging rate, in Hz.
#define LOG_RATE 1

/*----- SENSOR CONFIGURATION -----*/
// Delay in seconds after turning on before starting sensor initialization and zeroing.
// Should be long enough to have to the rocket closed up and on the pad.
#define INIT_DELAY 3

/*----- ARDUINO PINS AND ADDRESSING -----*/ 
// MOSI							// SERCOM4.2
// MISO							// SERCOM0+2.3
// SCK							// SERCOM4.3
// TXD							// SERCOM5.2
// RXD							// SERCOM5.3
// GPS RX 11					// SERCOM1+3.0
// GPS TX 10					// SERCOM1+3.2
// D9, AIN 7					// SERCOM0.3 Battery voltage. 2v fully charge.
#define SD_CS_PIN 4				// SERCOM0+2.0 SD Chip select
#define SD_CD_PIN 7				// SERCOM3+5.3 SD Card Detect
#define MPU6050_INT_PIN 12		// Accel interrupt
#define MPU6050_ADDRESS 0x68	// Accel I2C address
#define BMP388_CS_PIN 6			// Barometer chip select
#define BMP388_INT_PIN A4		// Barometer interrupt
#define VOLTAGE_PIN	A7			// Battery voltage. 2v fully charged. aR()*6.6/1024 
#define SYSTEM_LED_PIN 13		// Also onboard LED
#define BUZZER_PIN 18			// Buzzer. A4 on feather
#define STAGESEPERATION_PIN 17	// Detection of staging. A3 on feather
#define SYSTEM2_LED_PIN 8		// Second led onboard. No real pin.
#define RF95_CS_PIN 5
#define RF95_RST_PIN 19
#define RF95_IRQ0_PIN 11
#define RF95_IRQ1_PIN 10


/*----- ROCKET STAGES OF FLIGHT -----*/
#define STAGE_INIT 0
#define STAGE_PRELAUNCH 1
#define STAGE_ARMED 2
#define STAGE_ASCENT 3
#define STAGE_APOGEE 4
#define STAGE_DESCENT 5
#define STAGE_RECOVERY 6
