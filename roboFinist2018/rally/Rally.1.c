#pragma config(Sensor, S1,     sDistLeft,      sensorEV3_GenericI2C)
#pragma config(Sensor, S2,     sDistFront,     sensorEV3_GenericI2C)
#pragma config(Sensor, S3,     sDistRight,     sensorEV3_GenericI2C)
#pragma config(Sensor, S4,     sColor,         sensorEV3_Color)
#pragma config(Motor,  motorA,          mRear,         tmotorNXT, openLoop)
#pragma config(Motor,  motorB,          mFront,        tmotorNXT, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
//#pragma config(Motor,  motorD,          mWheel,        tmotorNXT, PIDControl, encoder)
////////////////////////////////////////////////////////////////
//
// rally motors
//
////////////////////////////////////////////////////////////////
// includes
//
//#include "mindsensors-ev3smux.h"
#include "mindsensors-irdist.h"
#include "config1.h"


// variables
int encRat				= 0;
int eDist 				= 0;
int eDistOld			= 0;
int frontDistMin  = INIT_FRONT_DIST_MIN;
int frontDistMax	= INIT_FRONT_DIST_MAX;
int sideDistMin		= INIT_FRONT_DIST_MIN;
int sideDistMax		= INIT_FRONT_DIST_MAX;
int distRight 		= 0;
int distLeft  		= 0;
int distFront 		= 0;
int mSpeed        = 0;

task main()
{

	//selfTest();

	sleep(100);
	startTask(getDist);
	sleep(100);
	startTask(wheelControl);
	sleep(100);
	startTask(motorControl);
	sleep(100);

	clearTimer(T1);

	while (true) {
		mSpeed = 100;
		sleep(1000);
		mSpeed = 50;
		sleep(1000);
		mSpeed = 0;
		sleep(1000);
	}
}

task motorControl(){
	int currentMotorSpeed = 0;
	int diff = 0;
	while (true) {
		diff = mSpeed - currentMotorSpeed;
		if (diff == 0) {
			sleep(MOTOR_SPEED_CHECK_INTERVAL);
		}
		else if (diff > 0) {
			if (diff >= MOTOR_SPEED_UP_STEP)
				currentMotorSpeed += MOTOR_SPEED_UP_STEP;
			else
				currentMotorSpeed += diff;
			sleep(MOTOR_SPEED_UP_SLEEP);
		}
		else {
			if (diff <= MOTOR_SPEED_DOWN_STEP)
				currentMotorSpeed += MOTOR_SPEED_DOWN_STEP;
			else
				currentMotorSpeed += diff;
			sleep(MOTOR_SPEED_DOWN_SLEEP);
		}
		if (currentMotorSpeed > MOTOR_SPEED_MAX) currentMotorSpeed = MOTOR_SPEED_MAX;
		if (currentMotorSpeed < MOTOR_SPEED_MIN) currentMotorSpeed = MOTOR_SPEED_MIN;
		motor[mFront]  = currentMotorSpeed;
		motor[mRear] = currentMotorSpeed;
	}
}

task getDist(){

ubyte address = 0x02;
const int SLEEP_BETWEEN_CHECK_DISTANCE = 15;

string type1 = MSDISTreadModuleType(sDistFront, address);
string type2 = MSDISTreadModuleType(sDistRight, address);
string type3 = MSDISTreadModuleType(sDistLeft,  address);

while(true){
	sleep(SLEEP_BETWEEN_CHECK_DISTANCE);
	distLeft  = normalizeSide(MSDISTreadDist(sDistLeft, address));
	sleep(SLEEP_BETWEEN_CHECK_DISTANCE);
	distRight = normalizeSide(MSDISTreadDist(sDistRight, address));
	sleep(SLEEP_BETWEEN_CHECK_DISTANCE);
	distFront = normalizeFront(MSDISTreadDist(sDistFront, address));
	eDist 		= distLeft - distRight;
	encRat = (eDist * 1 + (eDist - eDistOld));
	eDistOld = eDist;
	displayBigTextLine(4, "%d", encRat);
}
}

int normalizeFront(int dist){
if(dist < frontDistMin) return frontDistMin;
if(dist > frontDistMax) return frontDistMax;
return dist;
}

int normalizeSide(int dist){
if(dist < sideDistMin) return sideDistMin;
if(dist > sideDistMax) return sideDistMax;
return dist;
}

task wheelControl()
{
//int enc 	= 0;
//int fEnc 	= 0;
/*
nMotorEncoder[mWheel] = 0;

while(true){
int targetEncoder = getWheelEncoderByDistErr(encRat);
while ( ! isWheelCurrentEncoderOk( nMotorEncoder[mWheel] , targetEncoder ))
{
motor[mWheel] = -1 * sgn( nMotorEncoder[mWheel] - targetEncoder  ) * WHEEL_SPEED;
targetEncoder = getWheelEncoderByDistErr(encRat);
sleep(3);
}
motor[mWheel] = 0;
sleep(10);
}
*/
}

bool isWheelCurrentEncoderOk(int currentEncoder, int targetEncoder)
{
int t = currentEncoder - targetEncoder;
return (abs(t) <= WHEEL_INACCURACY_ENCODER) ;
}

int getWheelEncoderByDistErr(int err)
{
if (err > WHEEL_DIST_MAX_ERROR) err = WHEEL_DIST_MAX_ERROR;
long t = WHEEL_ENCODER_MAX * err ; // protectes int overflow
return -1 * (t / WHEEL_DIST_MAX_ERROR);
}

//--------------------------
bool selfTest()
{
clearDebugStream();
writeDebugStreamLine("--isWheelCurrentEncoderOk()-- ");

if ( ! isWheelCurrentEncoderOk(10, 0)) setException(1);


return true;

}
