/*
   RocketTele data logger and live telemetry.
   For use on the AdaFruit Feather Logger.


   PINS AS FOLLOWS:
   SDA+SCL = I2C to barometer and gyro. Needs pullups.
   TX1+RX0 = Serial to transmitter.
   A5/19 = Digital write to transmitter set pin.
   GPIO10+11 = Serial to GPS using SERCOM.
   GPIO5 = Lower stage seperation.
   GPIO6 = Barometer chip select.
   GPIO12 = MCU6050 interrupt.
   MISO+MOSI+SCK = SPI for SD Card and Barometer.


*/

#include <wiring_private.h> // for pinPeripheral().
#include <SPI.h>
#include <SD.h>
#include <I2Cdev.h>
#include <Wire.h>
#include <HC12.hpp>
#include <helper_3dmath.h>
#include <MPU6050.h>



#define SD_CS_PIN 4 // SPI Chip select
#define SD_CD_PIN 7 //  SD Card Detect
#define SYSTEM_LED_PIN 8
#define STAGESEPERATION_PIN 19
#define HC12_SET_PIN 6
#define MPU6050_INT_PIN 12
#define FILENAME_PREFIX LAUNCH_

struct SenserStruct {
  uint8_t barometer = 0;
  uint8_t accel = 0;
  uint8_t gps = 0;
  uint8_t sd = 0;
} sensors;

uint8_t stage = 0;
bool sdPresent = false;
bool telePresent = false;
unsigned int stageCounter = 0;
uint32_t loopCount = 0;
unsigned long updateTime = 0;
HC12 telemetry(&Serial1, HC12_SET_PIN);
MPU6050 accel(0x68);
File logFile;

Uart GPSSerial(&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
void SERCOM1_Handler()
{
  GPSSerial.IrqHandler();
}

void mpu6050_zero() {
  // Use gravity to determine where down is and set initial gyro values accordingly.
  
}

void setup() {

  // Configure Input/Output ports.
  pinMode(STAGESEPERATION_PIN, INPUT_PULLUP);
  pinMode(SD_CD_PIN, INPUT_PULLUP);
  pinMode(SD_CS_PIN, OUTPUT);
  digitalWrite(SD_CS_PIN, HIGH);

  // Start serial ports.
  GPSSerial.begin(115200);
  pinPeripheral(10, PIO_SERCOM); // TX
  pinPeripheral(11, PIO_SERCOM); // RX

  // Begin transmitting to base camp.
  telemetry.init();
  telemetry.send("STLSTL");

  // Check if SD Card is present.
  sdPresent = digitalRead(SD_CD_PIN);
  if (sdPresent) {
    telemetry.send("S33S33");
    sensors.sd = 3;
    if (SD.begin(SD_CS_PIN)) {
      telemetry.send("S34S34");
      sensors.sd = 4;
      unsigned short trialCount=0;
      char fileName[]="FILENAME_PREFIX000";
      do{
        if(!SD.exists(fileName)){
          sprintf(fileName, "FILENAME_PREFIX%03d", trialCount);
        }    
      }while(logFile==NULL);
      logFile = SD.open(fileName);
      telemetry.send("S35S35");
      telemetry.send("R: FILE OPENED AS... ");
      telemetry.send(fileName);
      sensors.sd = 5;
      SD.print("File Opened");
    } else {
      sensors.sd = 6;
      telemetry.send("S36S36");
      sdPresent=false;
    }
  } else {
    telemetry.send("S31S31"); // Send SD not present.
    sensors.sd = 1;
  }
}

void loop() {
  /* Read commands from HC12 and store them to be used.
  */
  telemetry.receive();
  if (telemetry.stageChange(false) != 0) {
    stage = telemetry.stageChange(true);
  }
  switch (stage) {
    /* Case 0
       Wait for connection from HC12 to initiate start. No user input required.
       Base station will send HBTHBT to rocket. Rocket will send "C00C00: Heartbeat" to base station.
    */
    case 0:
      telemetry.send("C00C00");
      if (telemetry.lastHeartbeat != 0) {
        stage = 1;
        stageCounter = 0;
      }else{
        if(stageCounter==0){
          updateTime=millis();
        }
        ++stageCounter;
      }
      break;
    /* Case 1
       Preflight. Wait for initialize signal from base station (INTINT)
    */
    case 1:
      if (updateTime + 3000 < millis()) {
        updateTime = millis();
        telemetry.send("C10C10"); // Waiting to start initialization.
      }
      if (telemetry.lastRead == "INTINT") {
        stage = 2;
        stageCounter = 0;
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
      if (stageCounter == 1) {
        telemetry.changeBaud(54000); // TODO: Check if correct.
        sensors.barometer = sensors.accel = sensors.gps = 0;
        sensorCount = 0;
        int x;
        char sensorChar[8] = "S00S00";
        sensorChar[7] = '\0';
        for (x = 0; x < 4; x++) {
          sensorChar[1] = sensorChar[4] = x; // Needs to be converted from int to char.
          sensorChar[2] = sensorChar[5] = '2';
          telemetry.send(sensorChar);
        }
      }
      switch (sensorCount) {
        case 0:
          switch (sensors.barometer) {
            case 0:
              break;
          }
          break;
        case 1:
          switch (sensors.accel) {
            case 0: // Check if sensor is present;
              sensors.accel = ( accel.testConnection() ? 3 : 1 ); // Check if sensor can be reached. If true set to startup '3';
              break;
            case 3: // Start sensor and init
              accel.initialize();

              break;
            case 4:

              break;
          }
          break;
        case 2:
          switch (sensors.gps) {
            case 0:
              /* Check if sensor is present, if so 3, if not 1. */
              sensors.gps = 3;
              break;
            case 3:
              /* Check if getting valid packets from GPS. If so 4 */
              break;
            case 4:
              /* Looking for GPS. If locked on set 5. */
              break;
          }
          break;
      }
      if (++sensorCount > 3) {
        sensorCount = 0;
      }
      // TODO: Check if all sensors are either not present or ready.
      break;
    /* Case 3
       Rocket ready. Wait for go/no-go signal from base station (RGORGO).
        If rocket not ready, it can be sent back to calibration or reset.
    */
    case 3:
      if (stageCounter == 1) {
        telemetry.changeBaud(2400);
      }
      if (updateTime + 3000 < millis()) {
        updateTime = millis();
        telemetry.send("C30C30"); //  Waiting for go/no-go.
      }
      if (telemetry.lastRead.equals("RGORGO")) {
        stageCounter = 0;
        stage = 4;
      }
      break;
    /* Case 4
       Rocket has been approved for launch. It polls only accel to detect launch.
    */
    case 4:
      if (updateTime + 3000 < millis()) {
        updateTime = millis();
        telemetry.send("C40C40"); // Awaiting launch!
      }
      
      break;
    /* Case 5
       Rocket is on ascent. Log pressure, temp, accel, GPS, Transmit only altitude.
    */
    case 5:
      while (1) {

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
  ++stageCounter;
} // End loop
