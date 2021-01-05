///// S_MPU6050.cpp /////

/* The FIFO puts out about 200 readings per second */

#include "S_MPU6050.hpp"
#include "Logging.hpp"

S_MPU6050::S_MPU6050(){
	accel = new MPU6050(MPU6050_ADDRESS);
	sensorStatus=3;
	TeleMax::dataReady=false;
	accel->initialize();
	Logging::log(3, "Testing accel MPU6050 connection... ");
	Logging::log(3, accel->testConnection() ? ("Connection Successful\n") : ("Connection Failed\n"));
	Logging::log(3, "Initializing DMP... ");
	devStatus = accel->dmpInitialize();
	if(devStatus){
		Logging::log(3, "DMP Failure: " + String(devStatus) + "\n");
		return;
	}
	accel->setXGyroOffset(-67);
	accel->setYGyroOffset(-102);
	accel->setZGyroOffset(-6);
	accel->setXAccelOffset(1267);
	accel->setYAccelOffset(1147);
	accel->setZAccelOffset(1534);
	packetSize = accel->dmpGetFIFOPacketSize();
	Logging::log(3, "DMP Initialized. Ready for calibration.\n");
	counter=0;
	sensorStatus=1;
};

S_MPU6050::~S_MPU6050(){
	delete (accel);
};

short S_MPU6050::initialize(){
	if(sensorStatus==3){
		Logging::log(2, "-MPU6050 init error\n");
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
	attachInterrupt(digitalPinToInterrupt(MPU6050_INT_PIN), TeleMax::dmpDataReady, RISING);
	dmpReady=true;
	sensorStatus=0;
	return 0;
};

void S_MPU6050::tick(){
	if(TeleMax::dataReady){
		accel->dmpGetCurrentFIFOPacket(fifoBuffer);
		TeleMax::dataReady=false;
		//Logging::log(3,"FIFO read\n");
		++counter;
	}
};


double S_MPU6050::getMeasurement(){
// Output quarternion and real world accel (acceleration in reference to observer)
	accel->dmpGetQuaternion(&q, fifoBuffer);
	accel->dmpGetAccel(&aa, fifoBuffer);
	accel->dmpGetGravity(&gravity, &q);
	accel->dmpGetLinearAccel(&aaReal, &aa, &gravity);
	accel->dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
	Logging::log(1, "A:" + String(q.w) + "," + String(q.x) + "," + String(q.y) + "," + String(q.z) + "," + String(aaWorld.x) + "," + String(aaWorld.y) + "," + String(aaWorld.z) + "\n");
	Logging::log(2, "-FIFO measurements: " + String(counter) + "\n");
	counter=0;
	return 0;
};

volatile bool TeleMax::dataReady;
void TeleMax::dmpDataReady(){
	TeleMax::dataReady=true;
};
