///// S_MPU6050.cpp /////

/* The FIFO puts out about 200 readings per second */

#include "S_MPU6050.hpp"
#include "Logging.hpp"
#define STATUS_OFFSET 2

S_MPU6050::S_MPU6050(){
	accel = new MPU6050(MPU6050_ADDRESS);
	sensorStatus=3;
	S_MPU6050_int::dataReady=false;
	accel->initialize();
	Logging::log(2, "-Testing accel MPU6050 connection... ");
	if(!accel->testConnection()){
		Logging::log(1, "-MPU6050 Connection Failed\n");
		return;
	}
	Logging::log(2, "-Connection Successful\n");
	Logging::log(3, "-Initializing DMP... ");
	devStatus = accel->dmpInitialize();
	if(devStatus){
		Logging::log(2, "-DMP Failure: " + String(devStatus) + "\n");
		return;
	}
	accel->setXGyroOffset(-67);
	accel->setYGyroOffset(-102);
	accel->setZGyroOffset(-6);
	accel->setXAccelOffset(1267);
	accel->setYAccelOffset(1147);
	accel->setZAccelOffset(1534);
	packetSize = accel->dmpGetFIFOPacketSize();
	Logging::log(2, "-MPU6050 DMP Initialized. Ready for calibration.\n");
	counter=0;
	sensorStatus=1;
};

S_MPU6050::~S_MPU6050(){
	delete (accel);
};

short S_MPU6050::initialize(){
	if(sensorStatus==3){
		Logging::log(1, "-MPU6050 in error. Unable to initialize.\n");
		return -1;
	}
	Logging::log(3,"-MPU6050 starting calibration.\n");
	Logging::log(3,"-> = Starting loop.\n");
    Logging::log(3,"-* = Error too great, retrying different values.\n");
    Logging::log(3,"-. = Values successful. Advancing to next loop.\n-");
	accel->CalibrateAccel(6); // For real use, use 6 for calibration
	accel->CalibrateGyro(6);
	Logging::log(3, "\n-MPU6050 calibration complete.\n");
	accel->setFullScaleAccelRange(3);
	accel->setDMPEnabled(true);
	attachInterrupt(digitalPinToInterrupt(MPU6050_INT_PIN), S_MPU6050_int::callback, RISING);
	dmpReady=true;
	sensorStatus=0;
	return 0;
};

float ypr[3];

void S_MPU6050::tick(){
	if(sensorStatus!=0)
		return;
	if(S_MPU6050_int::dataReady){
		accel->dmpGetCurrentFIFOPacket(fifoBuffer);
		// Do stuff below only if actually going to print every fifo out. Otherwise it's wasted cycles.
		/*
		accel->dmpGetQuaternion(&q, fifoBuffer);
		accel->dmpGetAccel(&aa, fifoBuffer);
		accel->dmpGetGravity(&gravity, &q);
		accel->dmpGetLinearAccel(&aaReal, &aa, &gravity);
		accel->dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
		Logging::log(2, "A:" + String(q.w) + "," + String(q.x) + "," + String(q.y) + "," + String(q.z) + "," + String(aa.x) + "," + String(aa.y) + "," + String(aa.z) + "\n");
		// */
		S_MPU6050_int::dataReady=false;
		++counter;
	}
};

double S_MPU6050::getMeasurement(){
	if(sensorStatus!=0)
		return 0;
	// If not printing out every loop above, at least print one out when we're sending a measurement
	// /*
	accel->dmpGetQuaternion(&q, fifoBuffer);
	accel->dmpGetAccel(&aa, fifoBuffer);
	accel->dmpGetGravity(&gravity, &q);
	accel->dmpGetLinearAccel(&aaReal, &aa, &gravity);
	accel->dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
	Logging::log(2, "A:" + String(q.w) + "," + String(q.x) + "," + String(q.y) + "," + String(q.z) + "," + String(aa.x) + "," + String(aa.y) + "," + String(aa.z) + "\n");
	// */
// Output quarternion and real world accel (acceleration in reference to observer)
	/*Logging::telemetryData->data.qw = q.w;
	Logging::telemetryData->data.qx = q.x;
	Logging::telemetryData->data.qy = q.y;
	Logging::telemetryData->data.qz = q.z;
	*/
	Logging::log(2, "-A: measurements taken: " + String(counter) + "\n");
	counter=0;
	accel->dmpGetEuler(ypr, &q);
	Logging::telemetryData->data.qx = ypr[0];
	Logging::telemetryData->data.qy = ypr[1];
	Logging::telemetryData->data.qz = ypr[2];

	Logging::telemetryData->data.ax = aaReal.x;
	Logging::telemetryData->data.ay = aaReal.y;
	Logging::telemetryData->data.az = aaReal.z;
	return 0;
};

unsigned char S_MPU6050::getStatus(){
	return sensorStatus << STATUS_OFFSET;
}

volatile bool S_MPU6050_int::dataReady;
void S_MPU6050_int::callback(){
	S_MPU6050_int::dataReady=true;
};
