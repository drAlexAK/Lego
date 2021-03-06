#pragma config(Sensor, S4,     sFront,         sensorSONAR)
#pragma config(Sensor, S2,     sFrontRight,    sensorI2CCustomFastSkipStates9V))
#pragma config(Motor,  motorA,          mLeft,         tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          mPl,           tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
//
#include "mindsensors-irdist.h"
#include "alex-common.h"
#include "shared.h"
//varieblas
//int vMax = 60;
int vBase = 50;
//int vMin = 0;
int vLeft  = 50;
int vRight = 50;
//
//#define DEBUG
//
#define DIST_START_ROBOT    400
#define DIST_TREE_NORM  		280
#define DIST_BETWEEN_FENCE_TREE  100
#define DIST_FRONT_MIN 			20
#define DEGREES_360_ENC 		4250
#define CM40_ENC 						1950
#define ENC_360_TURN        2100
// the lowest body speed limit
#define M_BODY_SPEED_MIN 17
// the highest body speed limit
#define M_BODY_SPEED_MAX 50

//function
//int getSpeedByFrontDistance();
void findTrees();
int getDistRightMedian();
int getDistRightAverage();
void goAheadMM(int dist);
void startRobotPos();
void turnRobotDegree(int deg);
int getAngelDeviationDegree(int errorDist, int testLenght);
void robotAngelCalibration(int lenght);
void goToTree(int dist);

void goAheadEncoder(int enc);
int convertEncoderAheadToMM(int encoder);
void goAheadEncoderArm(int enc, int m);
//tasks
task controlMotors();
//

task main()
{
	sleep(7000);
startTask(controlMotors);
goAheadEncoderArm(-ENC_360_TURN,1);
sleep(3000);
goAheadEncoderArm(500,2);
/*startRobotPos();
#ifdef DEBUG
	sleep(300);
#endif
	robotAngelCalibration(100);
#ifdef DEBUG
	sleep(300);
#endif
	findTrees();
#ifdef DEBUG
	sleep(300);
#endif*/
	vRight = vLeft = 0;
	stopAllTasks();
}

task controlMotors()
{
	while(1){
		int s =SensorValue(sFront) * 10 ;
		if((s< DIST_FRONT_MIN) && ((vLeft > 0) && (vRight > 0))){
			motor[mLeft] = 0;
			motor[mRight] = 0;
			}else{
			motor[mLeft] = vLeft;
			motor[mRight] = vRight;
		}
	}
}

void findTreesOld()
{
	int eNorm = getDistRightMedian();
	if (eNorm < DIST_TREE_NORM) eNorm = DIST_TREE_NORM;
	int e =0;
	int u =0;
	int eOld =e;
	int v =0;
	int i =0;
	bool treeIsFind = false;
	int encPos = 0;
	int distBeginTree = 0;
	int distEndTree = 0;

	while(true){
		e = eNorm - MSDISTreadDist(sFrontRight);
		// error must be great then different distance between fence and tree
		while(((e >= DIST_BETWEEN_FENCE_TREE) && (!treeIsFind)) ||
			((abs(e) >= DIST_BETWEEN_FENCE_TREE) && (treeIsFind))){
			i++;
			if(i > 2 ){
				vLeft = vRight = 0;
				playSound(soundBeepBeep);
				sleep(300);
				if( !treeIsFind ){
					int dist = MSDISTreadDist(sFrontRight) - DIST_TREE_NORM;
					goToTree(dist);
					nMotorEncoder[mLeft] =0;
					nMotorEncoder[mRight] =0;
					i = 0;
					eNorm = MSDISTreadDist(sFrontRight);
					distBeginTree = eNorm;
					treeIsFind = true;
					break;
					}else{
					encPos = nMotorEncoder[mLeft];
					goAheadMM(-100);
					int lenght = convertEncoderAheadToMM(encPos) - 100;
					distEndTree = getDistRightMedian();
					int errPosition = distEndTree - distBeginTree;
					int deg = sgn(errPosition) * getAngelDeviationDegree(abs(errPosition), lenght);//mm
					if (abs(deg) > 3 ) 	turnRobotDegree(deg);
					return;
				}
			}
			sleep(30);
			e = eNorm - MSDISTreadDist(sFrontRight);
		}
		if( treeIsFind ){
			encPos = nMotorEncoder[mLeft];
			distEndTree = eNorm - e;
		}
		i = 0;
		u = (e  + ((e - eOld) / 4 )) / 3;
		v = vBase - (u * 0.5);
		vLeft =  v ;// - u;
		vRight = v ;// + u;
		eOld = e;
		sleep(30);
	}
}

void findTrees()
{
	int eNorm = getDistRightMedian();
	//if (eNorm < DIST_TREE_NORM) eNorm = DIST_TREE_NORM;
	int e =0;
	int i =0;

	vLeft =  vRight = vBase ;

	while(true){
		e = eNorm - MSDISTreadDist(sFrontRight);
		// error must be great then different distance between fence and tree
		while(e >= DIST_BETWEEN_FENCE_TREE){
			vLeft =  vRight = M_BODY_SPEED_MIN;
			i++;
			if(i > 2 ){
				vLeft = vRight = 0;
#ifdef DEBUG
				playSound(soundBeepBeep);
				sleep(300);
#endif
				int dist = MSDISTreadDist(sFrontRight) - DIST_TREE_NORM;
				if (abs(dist) > 10)
				{
					goAheadMM(-50);
					goToTree(dist);
					goAheadMM(50);
				}
				robotAngelCalibration(80);
				return;
			}
			sleep(30);
			e = eNorm - MSDISTreadDist(sFrontRight);
		}
		i = 0;
		sleep(30);
	}
}


int getDistRightAverage(){
	int d = MSDISTreadDist(sFrontRight);
	int sum =0;
	int attempt = 5;

	for(int i =0; i< attempt; i++){
		sleep(20);
		d = MSDISTreadDist(sFrontRight);
		if (( d > 1000 ) || ( d < 50 )) {
			attempt--;
			sleep(40);
			} else {
			sum += d;
		}
	}
	return sum / attempt;
}

int getDistRightMedian(){
	int d = MSDISTreadDist(sFrontRight);
	const int attempt = 5;
	int a[attempt];

	for(int i =0; i< attempt; i++){
		sleep(20);
		a[i]= MSDISTreadDist(sFrontRight);
	}
	return GetMedian(a, attempt);
}

// if argument is positive angel, robot will turn clockwise, otherwise - anticlockwise.
void turnRobotDegree(int deg){
	nMotorEncoder[mLeft] =0;
	int	enc = (DEGREES_360_ENC * deg) / 360;
	int currentEnc = nMotorEncoder[mLeft];
	int speed = 0 ;
	if(enc > 0){
		while(currentEnc < enc){
			speed = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			vLeft = speed;
			vRight = -1 * speed;
			currentEnc = nMotorEncoder[mLeft];
		}
		} else {
		while(currentEnc > enc){
			speed = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			vLeft = -1 * speed;
			vRight = speed;
			currentEnc = nMotorEncoder[mLeft];
		}
	}
	vLeft = 0;
	vRight = 0;
}

int convertEncoderAheadToMM(int encoder) {
	return (400 * CM40_ENC) / encoder;
}

void goAheadMM(int dist){ //MM
	goAheadEncoder((CM40_ENC * dist) / 400);
}

void goAheadEncoder(int enc){ //encoder
	nMotorEncoder[mLeft] =0;
	int currentEnc = nMotorEncoder[mLeft];
	int speed = 0 ;
	if (enc > 0) {
		while(currentEnc < enc){
			vLeft = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			vRight = vLeft;
			currentEnc = nMotorEncoder[mLeft];
		}
		}else{
		while(currentEnc > enc){
			vLeft = -1 * getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			vRight = vLeft;
			currentEnc = nMotorEncoder[mLeft];
		}
	}
	vLeft = 0;
	vRight = 0;
}

void startRobotPos(){
	int d = getDistRightMedian();
	if( d < DIST_START_ROBOT ){
		turnRobotDegree(-90);
#ifdef DEBUG
		sleep(300);
#endif
		goAheadMM(DIST_START_ROBOT - abs(d));
#ifdef DEBUG
		sleep(300);
#endif
		turnRobotDegree(90);
	}
}
// returns degree of diviation by error and lenght distances
int getAngelDeviationDegree(int errorDist, int testLenght){
	return radiansToDegrees( atan2(errorDist, testLenght));
}

void robotAngelCalibration(int lenght){
	int d1 = getDistRightMedian();
	goAheadMM(lenght);
	int d2 = getDistRightMedian();
	int deg = sgn(d2 - d1) * getAngelDeviationDegree(abs(d2 - d1), lenght);//mm
	if (abs(deg) > 3 ) 	turnRobotDegree(deg);
}


void goToTree(int dist){
	turnRobotDegree(sgn(dist) * 90);
#ifdef DEBUG
	sleep(300);
#endif
	goAheadMM(abs(dist));
#ifdef DEBUG
	sleep(300);
#endif
	turnRobotDegree(-1 * sgn(dist) * 90);
}
//----------------------------------------------------------------------------------
void rotatePlatform(int deg){

	nMotorEncoder[mLeft] =0;
	int	enc = (DEGREES_360_ENC * deg) / 360;
	int currentEnc = nMotorEncoder[mLeft];
	int speed = 0 ;
	if(enc > 0){
		while(currentEnc < enc){
			speed = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			vLeft = speed;
			vRight = -1 * speed;
			currentEnc = nMotorEncoder[mLeft];
		}
		} else {
		while(currentEnc > enc){
			speed = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			vLeft = -1 * speed;
			vRight = speed;
			currentEnc = nMotorEncoder[mLeft];
		}
	}
	vLeft = 0;
	vRight = 0;
}

//----------------------------------------------------------------------------------
void goAheadEncoderArm(int enc,int m){ //encoder
	nMotorEncoder[mLeft] =0;
	int currentEnc = nMotorEncoder[mLeft];
	int speed = 0 ;
	if (enc > 0) {
		while(currentEnc < enc){
			if(m == 1)vLeft = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			else vRight = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			//vRight = vLeft;
			currentEnc = nMotorEncoder[mLeft];
		}
		}else{
		while(currentEnc > enc){
			if(m == 2)vRight = -1 * getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			else vLeft = -1 * getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, currentEnc, enc);
			//vRight = vLeft;
			currentEnc = nMotorEncoder[mLeft];
		}
	}
	vLeft = 0;
	vRight = 0;
}
