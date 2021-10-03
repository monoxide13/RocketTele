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

#define VOLTAGE_PIN	A7			// Battery voltage. 2v fully charged. aR()*6.6/1024 
#define PACKET_LED_PIN 13		// Also onboard LED
#define READY_LED_PIN 13		// Also onboard LED
#define SYSTEM_LED_PIN 13		// Also onboard LED
#define SYSTEM2_LED_PIN 13		// Second led onboard. No physical pin.
#define RF95_CS_PIN 8
#define RF95_RST_PIN 4
#define RF95_IRQ0_PIN 3
//#define RF95_IRQ1_PIN 3
//#define RF95_IRQ2_PIN 3


/*----- ROCKET STAGES OF FLIGHT -----*/
#define STAGE_INIT 0
#define STAGE_PRELAUNCH 1
#define STAGE_ARMED 2
#define STAGE_ASCENT 3
#define STAGE_APOGEE 4
#define STAGE_DESCENT 5
#define STAGE_RECOVERY 6
