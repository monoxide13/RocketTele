// Global header file
// Define the chip for the CMSIS lib. Needed here? Unknown.

#pragma once

#define __SAMD21G18__ 

/*----- ARDUINO PINS AND ADDRESSING -----*/ 
// MOSI							// SERCOM4.2
// MISO							// SERCOM0+2.3
// SCK							// SERCOM4.3
// TXD							// SERCOM5.2
// RXD							// SERCOM5.3
// GPS RX 11					// SERCOM1+3.0
// GPS TX 10					// SERCOM1+3.2
// D9, AIN 7				// SERCOM0.3 Battery voltage. 2v fully charge.

#define SD_CS_PIN 4				// SERCOM0+2.0 SD Chip select
#define SD_CD_PIN 7				// SERCOM3+5.3 SD Card Detect
#define HC12_SET_PIN 5			// HC12 serial TX/RX command mode pin.
#define VOLTAGE_PIN	A7			// Battery voltage. 2v fully charged. aR()*6.6/1024 
#define PACKET_LED_PIN 13		// Also onboard LED
#define READY_LED_PIN 13		// Also onboard LED
#define SYSTEM_LED_PIN 13		// Also onboard LED
#define SYSTEM2_LED_PIN 8		// Second led onboard. No physical pin.


/*----- ROCKET STAGES OF FLIGHT -----*/
#define STAGE_INIT 0
#define STAGE_PRELAUNCH 1
#define STAGE_THRUST 2
#define STAGE_COAST 3
#define STAGE_APOGEE 4
#define STAGE_DESCENT 5
#define STAGE_RECOVERY 6
