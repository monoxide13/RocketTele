/*
   RocketTele data logger and live telemetry.
   For use on the AdaFruit Feather Logger.
*/

#define SD_CS_PIN 4           // SD Chip select
#define SD_CD_PIN 7           //  SD Card Detect
#define SYSTEM_LED_PIN 8      // External LED indicating status.
#define STAGESEPERATION_PIN 5  // Limit switch detects stage seperation
#define HC12_SET_PIN 5        // HC12 serial TX/RX command mode pin
#define MPU6050_INT_PIN 12    // Accel interrupt
#define MPU6050_ADDRESS 0x68  // Accel I2C address
#define MPU6511_CS 6          // Barometer chip select
#define BUZZER_PIN 13            // Buzzer. 13 on board LED


#include "global.hpp"

#include <I2Cdev.h>
#include <Wire.h>
#include <wiring_private.h> // for pinPeripheral().
#include <SPI.h>
#ifdef OUTPUT_SD
#include <SD.h>
#endif
#ifdef USE_TELE
#include <HC12.hpp>
#endif
#include <MPU6050_6Axis_MotionApps20.h>
#include <helper_3dmath.h>
#include <MPU6050.h>
#include <MS5611.h>
#include <NMEAGPS.h>
#include <GPSport.h>


#define TELE_TX //
#define TELE_SU
#define TELE_ST
#ifdef OUTPUT_USB
#undef TELE_SU
#define TELE_SU(X) Serial.print(X);
#undef TELE_TX
#endif
#ifdef USE_TELE
#undef TELE_ST
#define TELE_ST(X) telemetry.send(X);
#undef TELE_TX
#endif
#ifndef TELE_TX
//#define TELE_TX(X) TELE_SU(X) TELE_ST(X)
#define TELE_TX(X) Serial.print(X);
#endif

struct SenserStruct {
  uint8_t barometer = 0;
  uint8_t accel = 0;
  uint8_t gps = 0;
  uint8_t sd = 0;
} sensors;

uint8_t stage = 0;
bool sdOpen = false;
bool telePresent = false;
bool buzzerState = false;
unsigned long buzzerChangeTime = 0;
Quaternion accel_q;           // [w, x, y, z]         quaternion container
VectorInt16 accel_a;         // [x, y, z]            accel sensor measurements
int16_t accel_history[] = {0, 0, 0};
uint8_t accel_hist_c = 0;
uint8_t accel_buffer[64]; // FIFO storage buffer
uint8_t buzzerCount = 0;
volatile bool accelDataReady = false;     // indicates whether MPU interrupt pin has gone high
unsigned int stageLoopCount = 1; // Start at 1 to keep all stages consistant
uint32_t loopCount = 0;
unsigned long loopTime = 0;
unsigned long updateTime = 0;
#ifdef USE_TELE
HC12 telemetry(&Serial1, HC12_SET_PIN);
#endif
MPU6050 accel(MPU6050_ADDRESS);
MS5611 barometer(MPU6511_CS);
#ifdef OUTPUT_SD
File logFile;
#endif
NMEAGPS gps;
gps_fix gpsFix;

/* Defined in GPSPort.hf
  Uart gpsPort(&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
  void SERCOM1_Handler()
  {
  gpsPort.IrqHandler();
  } */

void accelInterrupt() {
  accelDataReady = true;
}

//***********  Setup  ************//
void setup() {
  // Start I2C bus
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
#endif

  // Configure Input/Output ports.
#ifdef OUTPUT_USB
  Serial.begin(115200);
  while (!Serial.available()) {};
  Serial.println("-Starting...");
#endif
  pinMode(STAGESEPERATION_PIN, INPUT_PULLUP);
  pinMode(SD_CD_PIN, INPUT_PULLUP);
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);
  pinMode(MPU6050_INT_PIN, INPUT);
  pinMode(MPU6511_CS, OUTPUT);

  // Start serial ports.
  gpsPort.begin(9600);
  pinPeripheral(10, PIO_SERCOM); // TX
  pinPeripheral(11, PIO_SERCOM); // RX

  // Begin transmitting to base camp.
#ifdef USE_TELE
  telemetry.init();
#endif
  int x;
  // TODO: Turn buzzer on.
  for (x = 0; x < 3; ++x) {
    // Signal rocket in startup. 3 seconds of solid sound.
#ifdef USE_TELE
    TELE_TX("STRSTR");
#endif
    delay(1000);
  }
  // TODO: Turn buzzer off.

  // Check if SD Card is present.
#ifdef OUTPUT_SD
  if (digitalRead(SD_CD_PIN)) {
#ifdef USE_TELE
    TELE_TX("S33S33");
#endif
#ifdef USE_USB
    Serial.println("SD card present");
#endif
    sensors.sd = 3;
    if (SD.begin(SD_CS_PIN)) {
#ifdef USE_TELE
      TELE_TX("S34S34");
#endif
#ifdef USE_USB
      Serial.println("Opening SD");
#endif
      sensors.sd = 4;
      unsigned short trialCount = 0;
      char fileName[] = "FILENAME_PREFIX000";
      do {
        if (!SD.exists(fileName)) {
          sprintf(fileName, "FILENAME_PREFIX%03d\n", trialCount);
        }
      } while (logFile == NULL);
      logFile = SD.open(fileName, FILE_WRITE);
      if (logFile != NULL) {
#ifdef USE_TELE
        TELE_TX("S35S35");
#endif
#ifdef USE_USB
        Serial.print("-File opened as: ");
        Serial.println(fileName);
#endif
        sensors.sd = 5;
        logFile.write("File Opened");
        sdOpen = true;
      } else {
#ifdef USE_USB
        Serial.println("Error opening SD file!");
#endif
        sensors.sd = 6;
#ifdef USE_TELE
        TELE_TX("S36S36");
#endif
      }
    } else { // SD.begin()
#ifdef USE_TELE
      TELE_TX("S36S36"); // Send SD not present.
#endif
#ifdef OUTPUT_USB
      Serial.println("-SD Card not able to start.");
#endif
      sensors.sd = 6;
    } // end SD.begin
  } else { // if SD not present
#ifdef USE_TELE
    TELE_TX("S31S31"); // Send SD not present.
#endif
#ifdef OUTPUT_USB
    Serial.println("-SD Card not present.");
#endif
    sensors.sd = 1;
  } // end SD present
#else // If SD card not used
#ifdef USE_TELE
  TELE_TX("S31S31"); // Send SD not present/used.
#endif
#ifdef OUTPUT_USB
  Serial.println("-SD Card disabled.");
#endif
#endif // End SD card init
  delay(1000);
  stageLoopCount = 1;
  stage = 1;
  buzzerCount = 0;
}

//***********  Loop  ************//
void loop() {
  loopTime = millis();
  switch (stage) {
    case 1:
      if (stageLoopCount == 1) {
#ifdef OUTPUT_USB
        Serial.println("-Stage 1 start");
#endif
#ifdef USE_TELE
        TELE_TX("C10C10");
#endif
        updateTime = 0;
        buzzerCount = 0;
      }
      // Buzzer is on for 1 second, off for 2.
      if (updateTime <= loopTime) {
        if (!buzzerState) {
          buzzerState = true;
          digitalWrite(BUZZER_PIN, HIGH);
          updateTime=loopTime+1000;
        } else {
          buzzerState = false;
          digitalWrite(BUZZER_PIN, LOW);
          updateTime=loopTime+2000;
          ++buzzerCount;
        }
        if (buzzerCount > INITDELAY/3) {
          stage = 1;
          stageLoopCount = 0;
        }
      }
      break;
    /* Case 2
        Start and calibrate sensors. Barometer, Accel, GPS.
        Barometer is sensor S0.
        Accel is sensor S1.
        GPS is sensor S2.
        SD is sensor S3.
        Sx0 means unknown.
        Sx1 means not present.
        Sx3 means sensor start.
        Sx4 means sensor in calibration or looking(gps).
        Sx5 means sensor rdy.
        Sx6 means problem.
    */
    case 2:
      static unsigned short sensorCount;
      if (stageLoopCount == 1) {
        sensorCount = 0;
        sensors.barometer = sensors.accel = sensors.gps = 0;
#ifdef OUTPUT_USB
        Serial.println("-Stage 2 start");
#endif
#ifdef USE_TELE
        TELE_TX("C20C20"); // Start initialization.
#endif
      }
      if (sensorCount == 0) { // Every time sensorCount is 0, send progress report to base.
        int x, y = 0;
        char sensorChar[8] = "S00S00";
        sensorChar[7] = '\0';
        for (x = 0; x < 4; x++) {
          sprintf(sensorChar, "S%d%dS%d%d", x, *(((uint8_t *) &sensors) + x), x, *(((uint8_t *) &sensors) + x));
          //sensorChar[1] = sensorChar[4] =  // Needs to be converted from int to char.
          //sensorChar[2] = sensorChar[5] = '2';
#ifdef USE_TELE
          TELE_TX(sensorChar);
#endif
#ifdef OUTPUT_USB
          Serial.println(sensorChar);
#endif
        }
        Serial.println("Checking if sensors ready");
        if ( ((sensors.barometer - 1) % 4 == 0) && ((sensors.accel - 1) % 4 == 0) && \
             ((sensors.gps - 1) % 4 == 0) && ((sensors.sd - 1) % 4 == 0)) {
          Serial.println("Sensors ready");
          stage = 3;
          stageLoopCount = 0;
          sensorCount = 5; // Sensors are ready, skip the case loop;
        }
        if (stageLoopCount > 100) {
#ifdef OUTPUT_USB
          Serial.println("ERROR! Sensors not sure if ready, moving forwards anyways.");
#endif
#ifdef USE_TELE
          TELE_TX("C39C39");
#endif
          stage = 3;
          stageLoopCount = 0;
          sensorCount = 5; // Sensors are ready, skip the case loop;
        }
      }
      switch (sensorCount) {
        case 0:
          switch (sensors.barometer) {
            case 0: // Reset sensor and set to startup.
              barometer.cmd_reset(); // Trigger reboot.
              sensors.barometer = 3;
              break;
            case 3: // Sensor in startup.
              if (barometer.init() == 0) {
                sensors.barometer = 1; // Error.
#ifdef OUTPUT_USB
                Serial.println("Error starting barometer");
#endif
              } else {
                sensors.barometer = 4;
              }
              break;
            case 4: // Sensor in calibration. Set ground level.
              barometer.setStationPressure();
              break;
          }
          break;
        case 1:
          switch (sensors.accel) {
            case 0: // Check if sensor is present;
              sensors.accel = ( accel.testConnection() ? 3 : 1 ); // Check if sensor can be reached. If true set to startup '3';
#ifdef OUTPUT_USB
              Serial.println("Unable to start accel");
#endif
              break;
            case 3: // Start sensor and init
              accel.initialize();
              accel.setXGyroOffset(45);
              accel.setYGyroOffset(-30);
              accel.setZGyroOffset(24);
              accel.setXAccelOffset(-250);
              accel.setYAccelOffset(-2678);
              accel.setZAccelOffset(1179);
              sensors.accel = ((0 == accel.dmpInitialize()) ? 4 : 1);
              break;
            case 4: // TODO: CALIBRATE SENSOR AND GRAVITY.
              accel.setDMPEnabled(true);
              sensors.accel = 5;
              break;
          }
          break;
        case 2:
          switch (sensors.gps) {
            // TODO: IMPLEMENT GPS
            case 0:
              if (gpsFix.valid.status) {
                sensors.gps = 3;
#ifdef OUTPUT_USB
                Serial.println("Valid GPS data found.");
#endif
              }
              else
              {
                sensors.gps = 1;
#ifdef OUTPUT_USB
                Serial.println("No valid GPS data found. Not using.");
#endif
              }
              break;
            case 3:
              /* Check if getting valid packets from GPS. If so 4 */
              if (gpsFix.status == gps_fix::STATUS_STD) {
                sensors.gps = 4;
#ifdef OUTPUT_USB
                Serial.println("GPS fix status standard");
#endif
              }
              break;
            case 4:
              /* Looking for GPS. If locked on set 5. */
              if (gpsFix.valid.location) {
                sensors.gps = 5;
#ifdef OUTPUT_USB
                Serial.println("GPS Location found:");
                Serial.printf("%l, %l\nAlt: %f, SatsInUse:%d\n",
                              gpsFix.latitudeL(), gpsFix.longitudeL(), gpsFix.altitude(), gpsFix.satellites);
#endif
              }
              break;
          }
          break;
        default:
          break;
      }
      if (++sensorCount > 3) {
        sensorCount = 0;
      }
      ++stageLoopCount;
      break;
    /* Case 3
      Rocket should be ready. Varify all checks are complete.
      If not send error and send back to sensor phase or reset.
    */
    case 3:
      if (stageLoopCount == 1) {
#ifdef OUTPUT_USB
        Serial.println(" - Stage 3 starting");
#endif
      }
      if (stageLoopCount == 1000) {
        delay(5);
        stageLoopCount = 0;
        stage = 4;
      }
      break;
    /* Case 4
      Rocket has been approved for launch. It polls only accel to detect launch.
    */
    case 4:
      if (stageLoopCount == 1) {
        // Turn on accel interrupt.
#ifdef OUTPUT_USB
        Serial.println(" - Stage 4 starting");
#endif
        accel_hist_c = 0;
        // Clear accel overflow flag and empty buffer since we've been ignoring the interrupt.
        accel.resetFIFO();
        attachInterrupt(digitalPinToInterrupt(MPU6050_INT_PIN), accelInterrupt, RISING);
        accelDataReady = false;
#ifdef USE_TELE
        TELE_TX("C40C40"); // Awaiting launch!
#endif
        updateTime = millis();
      }
      if (updateTime <= millis()) {
        buzzerState = !buzzerState;
#ifdef OUTPUT_SD
        if (sdOpen) {
          logFile.print("t: ");
          logFile.println(millis());
        }
#else
        updateTime = millis() + 1000;
#endif
      }
      if (accelDataReady) {
        accelDataReady = false;
        accel.getIntStatus();
        accel.getFIFOBytes(accel_buffer, accel.dmpGetFIFOPacketSize());
        accel.dmpGetQuaternion(&accel_q, accel_buffer);
        accel.dmpGetAccel(&accel_a, accel_buffer);
#ifdef OUTPUT_SD
        if (sdOpen) {
          logFile.print("a: \t");
          logFile.print(accel_q.w);
          logFile.print("\t");
          logFile.print(accel_q.x);
          logFile.print("\t");
          logFile.print(accel_q.y);
          logFile.print("\t");
          logFile.println(accel_q.z);
          logFile.print("areal\t");
          logFile.print(accel_a.x);
          logFile.print("\t");
          logFile.print(accel_a.y);
          logFile.print("\t");
          logFile.println(accel_a.z);
        }
#endif
#ifdef OUTPUT_USB
        Serial.print("Y Accel: ");
        Serial.println((float)accel_a.y / 8196);
#endif
        accel_history[accel_hist_c] = accel_a.y;
        if (++accel_hist_c >= 3)
          accel_hist_c = 0;
        if ((accel_history[0] + accel_history[1] + accel_history[2]) / 3 <= -12294) {
          stage = 5;
          stageLoopCount = 0;
        }
#ifdef OUTPUT_USB
        Serial.println((accel_history[0] + accel_history[1] + accel_history[2]) / 3);
#endif
      }
      break;
    /* Case 5
      Rocket is on ascent. Log pressure, temp, accel, GPS, Transmit only altitude.
    */
    case 5:
      while (1) {
        if (accelDataReady) {
          accelDataReady = false;
          accel.getIntStatus();
          accel.getFIFOBytes(accel_buffer, accel.dmpGetFIFOPacketSize());
          accel.dmpGetQuaternion(&accel_q, accel_buffer);
          accel.dmpGetAccel(&accel_a, accel_buffer);
#ifdef OUTPUT_SD
          if (sdOpen) {
            logFile.print("a: \t");
            logFile.print(accel_q.w);
            logFile.print("\t");
            logFile.print(accel_q.x);
            logFile.print("\t");
            logFile.print(accel_q.y);
            logFile.print("\t");
            logFile.println(accel_q.z);
            logFile.print("areal\t");
            logFile.print(accel_a.x);
            logFile.print("\t");
            logFile.print(accel_a.y);
            logFile.print("\t");
            logFile.println(accel_a.z);
          }
#endif
#ifdef OUTPUT_USB
          Serial.print("Y Accel: ");
          Serial.println(accel_a.y);
#endif
          accel_history[accel_hist_c++] = accel_a.y;
          if (accel_hist_c == 3)
            accel_hist_c = 0;
        }
      };
      break;
    /* Case 6
      Rocket is on descent. Log pressure, temp, GPS. Transmit altitude and GPS.
    */
    case 6:
      break;
      /*
        Rocket has landed. Finish writes to SD card and close. Transmit GPS position.
      */
  } // End switch(stage)
  while (gps.available(gpsPort)) {
    gpsFix = gps.read();
    Serial.print("GPS DATA: ");
    Serial.println(gpsFix.valid.status);
  }
  ++stageLoopCount;
} // End loop
