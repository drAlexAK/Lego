#pragma config(Sensor, S2,     sFrontRight,    sensorI2CCustomFastSkipStates9V)
#pragma config(Sensor, S3,     sFront,         sensorSONAR)
#pragma config(Motor,  motorA,          mLeft,         tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          mRotation,     tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
//
#include "mindsensors-irdist.h"
#include "alex-common.h"
#include "shared.h"
#include "nxtPipe.h"
//varieblas
//int vMax = 60;
int vBase = 50;
//int vMin = 0;
int vLeft  = 0;
int vRight = 0;
//
int calcEnc = 0;
//
int enc = 0;
//#define DEBUG
//
#define UP_ARM_BEFORE_CATCH_APPLE_MM 30

#define DEGREES_360_ROTATION_ENC  2100 // Spb competion
#define ROTATION_MAX_360_DEGREE   360
#define M_ROTATION_SPEED_MIN      20
#define M_ROTATION_SPEED_MAX      80
//
#define DIST_START_ROBOT    400
#define DIST_TREE_NORM  		280
#define DIST_BETWEEN_FENCE_TREE  90 // need to calibrate distance between tree and garden
#define ACCURACY_DIST_BETWEEN_FENCE_AND_TREE 20
#define DIST_FRONT_MIN 			20
#define DIST_BETWEEN_VERTICAL_TREE_LINE 115

#define DEGREES_360_ENC 		  4465 // floor at home //4115
//#define DEGREES_360_ENC 		4250 // Spb competion

#define CM40_ENC 						1950
#define ENC_360_TURN        2150
// the lowest body speed limit
#define M_BODY_SPEED_MIN 20
// the highest body speed limit
#define M_BODY_SPEED_MAX 50
#define VERTICAL_ITERATION 6
//----------------------------
// semaphore variables
TSemaphore  semParkingRotation;
//----------------------------
//
int distToTree = 0;
//function
//int getSpeedByFrontDistance();
void resetMotorsEncoder();
void waitArm();
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
void Parking();
void goBackEncCalc();
void goAheadEncCalc();
void unloading();
void rotatePlatform(int deg);
void goToTheTree();
//bool getCoord(short &p1, short &p2);
bool lookForAppleVertical();
//int getArmMM();
bool catchApple();
int moveByHor();
//tasks
task controlMotors();
task parkingRotation();

task main()
{
	InitialyzePipe();
	startTask (controlMotors);

	int iConnect = 0;


	while ( !sendCommand(CMD_CONNECT, 0, false) ){
		displayTextLine(2, "Connecting %d", iConnect);
		iConnect ++;
		sleep(200);
		InitialyzePipe();
	}
	displayTextLine(2, "Connected");

	startTask (BlueToothListener);

	sleep(10000);

	waitArm();

	sleep(5000);

	goToTheTree();

	//for (int h = 0; h < 10 ; h++)
	//sleep(1000);
	//----------------------
	int i =0;
	int sum = 0;
	int falseLookUpAppleIteration = 0;
	const int maxFalseLookUpAppleIteration = 3;

	const int maxGetAppleAttempts = 6;
	int getAppleAttempts = 0;

	for(int k =0 ; k < 2; k++){
		i =0;
		sum =0;
		getAppleAttempts = 0;

		while(i < VERTICAL_ITERATION){
			getAppleAttempts ++;
			falseLookUpAppleIteration = 0;
			while ((lookForAppleVertical()) && (falseLookUpAppleIteration < maxFalseLookUpAppleIteration)) {
				falseLookUpAppleIteration ++;
			}
			sendCommand(CMD_PARK_ALL);
			sleep(500);

			displayTextLine(2, "LookUp apple %d", i);

			if (i < (VERTICAL_ITERATION - 1)) {
				if(k == 0)
					goAheadMM(DIST_BETWEEN_VERTICAL_TREE_LINE);

				if(k == 1)
					goAheadMM(-1 * DIST_BETWEEN_VERTICAL_TREE_LINE);
			}
			i++;
		}
		sleep(500);
		getAppleAttempts = 0;
		i =0;
	}
	sendCommand(CMD_PARK_ALL,0);
	Parking();
	sleep(1000);
	goAheadMM(-300);

	vRight = vLeft = 0;
	sendCommand(CMD_SHUT_DOWN,0, false);
	stopAllTasks();
}

void goToTheTree(){
	//startRobotPos();
#ifdef DEBUG
	sleep(500);
#endif
	robotAngelCalibration(250);
#ifdef DEBUG
	sleep(500);
#endif
	findTrees();
}

bool lookForAppleVertical() {
	//short y, x;
	int sum = 0;
	sendCommand(CMD_SET_LANDLE_BY_ARM, 0);
	sendCommand(CMD_MOVE_PL,0);
	sendCommand(CMD_LOOK_FOR_APPLE_BY_ARM);
	sleep(100);
	//ReinitPipe();
	InitialyzePipe(); //reinit pipe
	//if (getCoord(y, x)) {
	if (camStatus.Apple == false) sleep(500); // protect mistake

	if (camStatus.Apple) {
		sum = moveByHor();
		if (catchApple()) unloading();
		if (abs(sum) > 10) goAheadMM(-1 * sum);
		return true;
	}
	return false;
}


// gets an apple and confirms the apple in the basket. here are three attempts
bool catchApple(){
	//short x =0;
	//short y =0;
	int shiftPL = 75 + (distToTree - DIST_TREE_NORM ); // 75
	const int shiftArm = 50;
	//const int shiftPlatformAfterArmUp = 10;

	writeDebugStreamLine("Starting catch apple");
	for (int i = 0; i < 3; i++) {
		//if(!getCoord(y, x)) break;
		if (camStatus.Apple == false) break;
		sendCommand(CMD_SHIFT_PL_MM, shiftPL);
		// here----------------------------------------------------------------------------------------
		//sendCommand(CMD_SAVE_ARM_MM);
		//sendCommand(CMD_RESTORE_ARM_MM, UP_ARM_BEFORE_CATCH_APPLE_MM);
		//sendCommand(CMD_SHIFT_ARM_STR_VERT_MM, UP_ARM_BEFORE_CATCH_APPLE_MM);
		//sendCommand(CMD_SHIFT_PL_MM, shiftPlatformAfterArmUp);
		sendCommand(CMD_SOFTLY_GET_APPLE, UP_ARM_BEFORE_CATCH_APPLE_MM);
		//---------------------------------------------------------------------------------------------
		//sendCommand(CMD_DOWN_LANDLE, 0);
		sendCommand(CMD_SHIFT_PL_MM, -1 * shiftPL);
		//sendCommand(CMD_SHIFT_PL_MM, -1 * shiftPL - shiftPlatformAfterArmUp);
		// here----------------------------------------------------------------------------------------
		//sendCommand(CMD_SAVE_ARM_MM);
		//sendCommand(CMD_RESTORE_ARM_MM, -1 * UP_ARM_BEFORE_CATCH_APPLE_MM);
		//sendCommand(CMD_SHIFT_ARM_STR_VERT_MM, -1 * UP_ARM_BEFORE_CATCH_APPLE_MM);
		//---------------------------------------------------------------------------------------------

		sleep(100);
		//if (getCoord(y, x)) return true;
		if (isAppleInLadle()) return true;
		rotatePlatform(30);

		//if (getCoord(y, x)){
		if (isAppleInLadle()) {
			rotatePlatform(0);
			return true;
		}

		rotatePlatform(-30);

		//if (getCoord(y, x)){
		if (isAppleInLadle())	{
			rotatePlatform(0);
			return true;
		}

		rotatePlatform(0);
		if (isAppleInLadle()) return true;

		writeDebugStreamLine("Failed catch apple");
		sendCommand(CMD_SET_LANDLE_BY_ARM);
		shiftPL += 5; //10
	}
	sendCommand(CMD_SAVE_ARM_MM);
	sendCommand(CMD_RESTORE_ARM_MM, shiftArm); // skip unride apple
	return false;
}

void unloading(){
	writeDebugStreamLine("Starting unload");
	sendCommand(CMD_SAVE_ARM_MM);
	//int tmp = getArmMM();
	sendCommand(CMD_MOVE_PL, 0);
	rotatePlatform(-90);
	sendCommand(CMD_UP_ARM, 120);
	while(SensorValue(sFront) > 30) sleep(100);
	sendCommand(CMD_DOWN_LANDLE, 130);
	sleep(1000);
	sendCommand(CMD_DOWN_LANDLE, 0);
	rotatePlatform(0);
	sendCommand(CMD_RESTORE_ARM_MM, 0);
	//sendCommand(CMD_UP_ARM, tmp);
	writeDebugStreamLine("Finish unload");
	//sendCommand(CMD_DOWN_LANDLE, 45);
}
/*
int getArmMM(){
for(int i = 0; i < 3; i++){
writeDebugStreamLine("Getting Arm mm");
if(sendCommand(CMD_GET_ARM_MM,0)){
int tmp =0;
memcpy(&tmp, outDelivery.Msg[4], 4);
return tmp;
}
sleep(100);
writeDebugStreamLine("Failed get Arm mm");
}
return ARM_MAX_POSITION_270MM;
}
*/
/*
bool getCoord(short &p1, short &p2){

p1 = camStatus.Y;
p2 = camStatus.X;
return (camStatus.Apple);
}
*/
void goAheadEncCalc(){
	int encNorm = getDistRightMedian();
	int i=0;
	int e=0;
	while(i < 5){
		i++;
		e = encNorm - getDistRightMedian();
		while(abs(e) < 100){
			e = encNorm - getDistRightMedian();
			vLeft = 20;
			vRight = 20;
			i = 0;
		}
	}
	vLeft = vRight =0;
	calcEnc = nMotorEncoder[mLeft];
}

void goBackEncCalc(){
	int encNorm = getDistRightMedian();
	int i=0;
	int e=0;
	while(i < 5){
		i++;
		e = encNorm - getDistRightMedian();
		while(abs(e) < 100){
			e = encNorm - getDistRightMedian();
			vLeft = -20;
			vRight = -20;
			i = 0;
		}
	}
	vLeft = vRight =0;
	nMotorEncoder[mLeft] =0;
}

void resetMotorsEncoder() {
	nMotorEncoder[mLeft]  		= 0;
	nMotorEncoder[mRight] 		= 0;
	nMotorEncoder[mRotation] 	= 0;
}

task controlMotors()
{
	while(1){

		motor[mLeft] = vLeft;
		motor[mRight] = vRight;
		sleep(5);
		/*
		int s = SensorValue(sFront) * 10 ;
		if((s < DIST_FRONT_MIN) && ((vLeft > 0) && (vRight > 0))){
		motor[mLeft] = 0;
		motor[mRight] = 0;
		}else{
		motor[mLeft] = vLeft;
		motor[mRight] = vRight;
		}
		*/
	}
}

void findTrees()
{

	sleep(300);
	int eNorm = getDistRightMedian();
	//if (eNorm < DIST_TREE_NORM) eNorm = DIST_TREE_NORM;
	int e =0;
	int i =0;

	vLeft =  vRight = vBase ;

	while(true){
		e = eNorm - MSDISTreadDist(sFrontRight);
		// error must be great then different distance between fence and tree
		while(e >= (DIST_BETWEEN_FENCE_TREE - ACCURACY_DIST_BETWEEN_FENCE_AND_TREE)){
			vLeft =  vRight = M_BODY_SPEED_MIN;

			i++;
			if(i > 2 ){
				vLeft = vRight = 0;
				goAheadMM(50); // move forward before check distance to tree
				sleep(100);
#ifdef DEBUG
				//playSound(soundBeepBeep);
				//sleep(300);
#endif
				distToTree = getDistRightMedian();
				int dist =  distToTree - DIST_TREE_NORM;
				//goAheadMM(-40); // retrack
				/*if (abs(dist) > 30)
				{
					goAheadMM(-50);
					goToTree(dist);
					goAheadMM(50);
				}*/
				//goAheadMM(-20);
				//robotAngelCalibration(70); // unfortunately doesn't work because distance too short
				sleep(500);
				// save dist to a tree
				//goAheadMM(-40);
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

	for(int i = 0; i < attempt; i++){
		sleep(60);
		a[i] = MSDISTreadDist(sFrontRight);
	}
	return GetMedian(a, attempt);
}

// if argument is positive angel, robot will turn clockwise, otherwise - anticlockwise.
void turnRobotDegree(int deg){
	nMotorEncoder[mLeft] = 0;
	int	enc = (DEGREES_360_ENC * deg) / 360;
	int currentEnc = 0;
	int speed = 0 ;
	if(enc > 0){
		while(currentEnc < enc){
			speed = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, 0, currentEnc, enc);
			vLeft =  speed;
			vRight = -1 * speed;
			currentEnc = nMotorEncoder[mLeft];
		}
		} else {
		while(currentEnc > enc){
			speed = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, 0, currentEnc, enc);
			vLeft = speed;
			vRight = -1 * speed;
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
	if (abs(dist) < 10) return;
	goAheadEncoder((CM40_ENC * dist) / 400);
}

void goAheadEncoder(int enc){ //encoder
	nMotorEncoder[mLeft] =0;
	int currentEnc = nMotorEncoder[mLeft];
	if (enc > 0) {
		while(currentEnc < enc){
			vLeft = getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, 0, currentEnc, enc);
			vRight = vLeft;
			currentEnc = nMotorEncoder[mLeft];
		}
		}else{
		while(currentEnc > enc){
			vLeft =  getLimitSpeed(M_BODY_SPEED_MIN, M_BODY_SPEED_MAX, 0, currentEnc, enc);
			vRight = vLeft;
			currentEnc = nMotorEncoder[mLeft];
		}
	}
	vLeft = 0;
	vRight = 0;
}

void startRobotPos(){

	int dist =  getDistRightMedian() - (DIST_TREE_NORM + DIST_BETWEEN_FENCE_TREE);
	if (abs(dist) > 20) goToTree(dist);

}
// returns degree of diviation by error and lenght distances
int getAngelDeviationDegree(int errorDist, int testLenght){
	return radiansToDegrees( atan2(errorDist, testLenght));
}

void robotAngelCalibration(const int lenght){
	sleep(1000);
	const int backLenght = -20;
	int d1 = getDistRightMedian();
	goAheadMM(lenght);
	sleep(1000);
	int d2 = getDistRightMedian();
	int deg = sgn(d2 - d1) * getAngelDeviationDegree(abs(d2 - d1), lenght);//mm
	if (abs(deg) > 45) { // noise protection, go back and recalc
		goAheadMM(backLenght);
		sleep(1000);
		d2 = getDistRightMedian();
		deg = sgn(d2 - d1) * getAngelDeviationDegree(abs(d2 - d1), lenght + backLenght);//mm
		goAheadMM(abs(backLenght));
	}
	if (abs(deg) > 2 ) turnRobotDegree(deg);
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

//----------------------------------------------------------------------------- - -|
void rotatePlatform(int deg){
	if (abs(deg) > ROTATION_MAX_360_DEGREE) deg = sgn(deg) * ROTATION_MAX_360_DEGREE;
	int	enc = (DEGREES_360_ROTATION_ENC * deg) / ROTATION_MAX_360_DEGREE;
	int startEnc = nMotorEncoder[mRotation];
	int speed = 0 ;
	if((enc - startEnc ) > 0){
		while(nMotorEncoder[mRotation] < enc){
			speed = getLimitSpeed(M_ROTATION_SPEED_MIN, M_ROTATION_SPEED_MAX, startEnc, nMotorEncoder[mRotation], enc);
			motor[mRotation] = speed;
		}
		} else {
		while(nMotorEncoder[mRotation] > enc){
			speed = getLimitSpeed(M_ROTATION_SPEED_MIN, M_ROTATION_SPEED_MAX, startEnc, nMotorEncoder[mRotation], enc);
			motor[mRotation]= speed;
		}
	}
	motor[mRotation]=0;
}
//----------------------------------------------------------------------------- - -|
void Parking(){
	semaphoreInitialize(semParkingRotation);
	startTask(parkingRotation);
	sleep(100);
	semaphoreLock( semParkingRotation);
	if (bDoesTaskOwnSemaphore(semParkingRotation)) semaphoreUnlock(semParkingRotation);
}

task parkingRotation(){
	semaphoreLock( semParkingRotation );
	rotatePlatform(0);
	if (bDoesTaskOwnSemaphore(semParkingRotation)) semaphoreUnlock(semParkingRotation);
}

int moveByHor(){
	int accuracy = 25;
	int shiftMM = 0;
	//short x =0, y =0;
	int sum =0;
	//bool appleHere = getCoord(y, x);
	//while ((appleHere) && ((x < -1 * accuracy ) || (x > accuracy ))){ // hor
	while ((camStatus.Apple) && ((camStatus.X < -1 * accuracy ) || (camStatus.X > accuracy ))){
		shiftMM = camStatus.X / 15;
		//writeDebugStreamLine("Positionary by horizont: %d", shiftMM);
		if (abs(shiftMM) > 10) {
			sum += shiftMM;
			goAheadMM(shiftMM);
		}
		else
		{
			break;
		}
		sleep(200);
		//appleHere = getCoord(y, x);
	}
	//if ( ! getCoord(y, x)) goAheadMM( -1 * shiftMM); // if we lost the apple we will move back
	if ( camStatus.Apple == false) goAheadMM( -1 * shiftMM); // if we lost the apple we will move back
		return sum;
}

void waitArm(){

	while(SensorValue(sFront) > 20)
		sleep(100);

	while(SensorValue(sFront) < 20)
	sleep(100);

	}
