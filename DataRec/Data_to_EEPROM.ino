/* ---------- Data to EEPROM -----------
 *  This program is for the Sparkfun Pro Micro 5v 16Mhz.
 *  It uses an GY-521 for accel and gyro, and GY-63 for barometer. For EEPROM it uses a one-wire DS24B33.
 *  It has a button that when pressed during boot, it will dump the EEPROM via serial.
 *  There is a speaker that will sound constant while booting and calibrating. When ready for launch, it will turn into a siren for locating after launch.
 */

/* Define global variables and values. These must be defined prior to files because some of these link into the includes. */
#define SDEBUG_START
//#define SDEBUG_ACCEL
#define SDEBUG_BARO

/* DMP_FIFO_RATE_DIVISOR defines FIFO rate.
 *  0x01 = 100Hz. 0x07 = 25Hz. 0x09 = 20Hz.
 *  FIFO rate = (200Hz / (1 + value)
 */
#define MPU6050_DMP_FIFO_RATE_DIVISOR 0x09
#define SEALEVELPRESSURE_HPA (1013.25)

/* Define pin locations */
#define CALIBRATION_COUNT 100
#define MPU_INTERRUPT_PIN 7
#define BME_CS 8
#define MEM_CS 10
#define BUZZER_PIN 6
#define BUTTON_PIN 5
#define LED_PIN 9

/* Includes */
#include "I2Cdev.h"
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include "MPU6050_6Axis_MotionApps20.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

bool recordingData=false;
bool stageInit=true;

MPU6050 * mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;
uint8_t mpuStatus;
uint8_t bmeStatus;
uint8_t memStatus;
uint8_t fifoBuffer[64];
uint16_t mpuPacketSize;

Adafruit_BME280 * bme;
float seaLevelPressure;

// 0 prelaunch. 1 ready for launch. 2 acceleration. 3 coast.
// 4 droge descent. 5 main descent. 6 recovery. 7 Dump data. 8 ERROR.
uint8_t rocketStage=0;
float gyroAveX=0, gyroAveY=0, gyroAveZ=0;

void setup() {
  // Start functions and declarations independent of events.
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif
  // Set up the pins
  pinMode(BUTTON_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  #ifdef SDEBUG_START
  Serial.begin(115200);
  #endif
  
  
  // Start up delay for rocket to get assembled and parts to warm up.
  delay(3000); // 30 seconds
  
  /* Initialize gyro/accelerometer */
  mpu = new MPU6050();
  // Check switch to see if we're in flight mode or dump mode.
  /** TODO: Check what mode we're in**/
  if(rocketStage==7){ // Dump mode.
    #ifndef SDEBUG_ACCEL
    Serial.begin(115200);
    #endif
    return; // Return from setup function.
  }
  // Check EEPROM to see if rocket is set for a certain stage, possibly from power loss.
  delay(2000); // 2 second delay for rocket to stop moving from being touched so sensors can calibrate.
}

void loop() {
  switch (rocketStage){
    case 0:
      if(stageInit){
        #ifdef SDEBUG_ACCEL
        Serial.println(F("Initializing MPU"));
        #endif
        mpuStart();
      
        /* Initialize barometer */
        bme = new Adafruit_BME280(BME_CS);
        bme->setSampling(Adafruit_BME280::MODE_NORMAL,
                          Adafruit_BME280::SAMPLING_NONE,   // temperature
                          Adafruit_BME280::SAMPLING_X16,   // pressure
                          Adafruit_BME280::SAMPLING_NONE, // humidity
                          Adafruit_BME280::FILTER_X16,
                          Adafruit_BME280::STANDBY_MS_10 );
                          
        /* Start interrupt and FIFO buffer */
        #ifdef SDEBUG_ACCEL
        Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
        Serial.print(digitalPinToInterrupt(MPU_INTERRUPT_PIN));
        Serial.println(F(")..."));
        #endif
        seaLevelPressure = bme->seaLevelForAltitude(0, SEALEVELPRESSURE_HPA);
        attachInterrupt(digitalPinToInterrupt(MPU_INTERRUPT_PIN), mpuInterrupt, RISING);
        // if ockets ready, go to next stage.
        rocketStage=1;
        stageInit=true;
      }
      break;
    case 1:
    
      mpu->getFIFOBytes(fifoBuffer, mpuPacketSize); 
      break;
    }
  while(recordingData){ // Launching, record info.
    if(mpu->getFIFOCount()>=mpuPacketSize){
      mpu->getFIFOBytes(fifoBuffer, mpuPacketSize);
    }
    mpu->getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    #ifdef SDEBUG_ACCEL
    Serial.print(F("a/g:\t"));
    Serial.print((float)ax/4096); Serial.print(F("\t"));
    Serial.print((float)ay/4096); Serial.print(F("\t"));
    Serial.print((float)az/4096); Serial.print(F("\t"));
    Serial.print(sqrt( sq((float)az/4096)) + sq((float)ay/4096) + sq((float)ax/4096) ); Serial.print(F("\t"));
    Serial.print((float)(gx-gyroAveX)/32.8); Serial.print(F("\t"));
    Serial.print((float)(gy-gyroAveY)/32.8); Serial.print(F("\t"));
    Serial.println((float)(gz-gyroAveZ)/32.8);
    Serial.print((float)(gx)/32.8); Serial.print(F("\t"));
    Serial.print((float)(gy)/32.8); Serial.print(F("\t"));
    Serial.println((float)(gz)/32.8);
    #endif

  }
  while(true){ // Landed, stop recording. 
    
  }
}

void mpuStart(){
  mpu->initialize();
  // Test if we are connected to the MPU6050
  if(mpu->testConnection()){
    #ifdef SDEBUG_ACCEL
    Serial.print(F("Sensor connected. DevID: "));
    Serial.println(mpu->getDeviceID());
    #endif
    mpu->setFullScaleAccelRange(MPU6050_ACCEL_FS_8); // Set for 8g max detection. 4096 LSB/g. Divide raw value by 4096 to get g's.
    mpu->setFullScaleGyroRange(MPU6050_GYRO_FS_1000); // Set to 1000 degrees per second of rotation. Divide number by 32.8 to get angular velocity of 1 degree per second.
    pinMode(LED_BUILTIN, OUTPUT);
    mpu->setXGyroOffsetTC(-11);
    mpu->setYGyroOffsetTC(-53);
    mpu->setZGyroOffsetTC(20);
    mpu->setXAccelOffset(-422);
    mpu->setYAccelOffset(-2703);
    mpu->setZAccelOffset(1162);
    if(mpu->dmpInitialize()!=0){
      #ifdef SDEBUG_ACCEL
      Serial.println(F("Error initializing MPU DMP."));
      #endif
        mpu->getIntStatus();
    }
    mpuPacketSize = mpu->dmpGetFIFOPacketSize();
  }else{
    mpu=NULL;
    #ifdef SDEBUG_ACCEL
    Serial.println(F("Accel Sensor not found! ERROR."));
    Serial.flush();
    #endif
  }
}

void mpuInterrupt(){
  /* Interrupt that we have data. Save it.*/
  /* This will get called at FIFO rate. 20Hz */
  
  static uint16_t fifoCount; 
  if(bmeStatus==0){
    #ifdef SDEBUG_ACCEL
    Serial.print(F("Temperature = "));
    Serial.print(bme->readTemperature());
    Serial.println(F(" *C"));
    Serial.print(F("Pressure = "));
    Serial.print(bme->readPressure() / 100.0F);
    Serial.println(F(" hPa"));
    Serial.print(F("Approx. Altitude = "));
    Serial.print(bme->readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(F(" m"));
    Serial.print(F("Humidity = "));
    Serial.print(bme->readHumidity());
    Serial.println(F(" %"));
    #endif
  }
  while (fifoCount < mpuPacketSize) fifoCount = mpu->getFIFOCount(); // Wait until the FIFO has all the data
  mpu->getFIFOBytes(NULL, mpuPacketSize);
}


void calibrateGyro(){
  int gyroErrorX[CALIBRATION_COUNT], gyroErrorY[CALIBRATION_COUNT], gyroErrorZ[CALIBRATION_COUNT];
  #ifdef SDEBUG_ACCEL
  Serial.print(F("Calibrating gyro: "));
  #endif
  for(int i=0; i<CALIBRATION_COUNT; ++i){
    #ifdef SDEBUG_ACCEL
    Serial.print(F("."));
    #endif
    mpu->getMotion6(&ax, &ay, &az, &gx, &gy, &gz); // Changed, should be ax first.
    gyroErrorX[i]=gx;
    gyroAveX+=gx;
    gyroErrorY[i]=gy;
    gyroAveY+=gy;
    gyroErrorZ[i]=gz;
    gyroAveZ+=gz;
    delay(20);
  }
  gyroAveX=gyroAveX/CALIBRATION_COUNT;
  gyroAveY=gyroAveY/CALIBRATION_COUNT;
  gyroAveZ=gyroAveZ/CALIBRATION_COUNT;
  float xSum=0,ySum=0,zSum=0;
  for(int i=0; i<CALIBRATION_COUNT; ++i){
    Serial.print(F("."));
    xSum=sq(abs((gyroErrorX[i]-gyroAveX)));
    ySum=sq(abs((gyroErrorY[i]-gyroAveY)));
    zSum=sq(abs((gyroErrorZ[i]-gyroAveZ)));
  }
  xSum=sqrt(xSum);
  ySum=sqrt(ySum);
  zSum=sqrt(zSum);
  Serial.println("\nCalibration complete.");
  Serial.print(F("x:\t")); Serial.print(gyroAveX); Serial.print(F("\t")); Serial.println(xSum);
  Serial.print(F(" y:\t")); Serial.print(gyroAveY); Serial.print(F("\t")); Serial.println(ySum);
  Serial.print(F(" z:\t")); Serial.print(gyroAveZ); Serial.print(F("\t")); Serial.println(zSum);
}
