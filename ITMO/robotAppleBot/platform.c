#pragma config(Sensor, S2,     sFrontRight,    sensorI2CCustomFastSkipStates9V)
#pragma config(Sensor, S4,     sFront,         sensorSONAR)
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
//short msgCam[3] = {0,0,0};
int vLeft  = 0;
int vRight = 0;
//
int calcEnc = 0;
//
int enc =0;
//#define DEBUG
//
#define DEGREES_360_ROTATION_ENC 2100
#define ROTATION_MAX_360_DEGREE   360
#define M_ROTATION_SPEED_MIN      20
#define M_ROTATION_SPEED_MAX      80
//
#define DIST_START_ROBOT    400
#define DIST_TREE_NORM  		280
#define DIST_BETWEEN_FENCE_TREE  100
#define DIST_FRONT_MIN 			20
#define DEGREES_360_ENC 		4250
#define CM40_ENC 						1950
#define ENC_360_TURN        2150
// the lowest body speed limit
#define M_BODY_SPEED_MIN 20
// the highest body speed limit
#define M_BODY_SPEED_MAX 50
//----------------------------
// semaphore variables
TSemaphore  semParkingRotation;
//----------------------------
//
//function
//int getSpeedByFrontDistance();
void resetMotorsEncoder();
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
bool getCoord(short &p1, short &p2);
bool catchApple();
int getArmMM();
//tasks
task controlMotors();
task parkingRotation();
task BlueToothListener();
//task armListerner();

task main()
{
	sleep(5000);
	InitialyzePipe();
	startTask (controlMotors);
	sleep(1000);

	resetMotorsEncoder();

	int iConnect = 0;
	//startTask (BlueToothListener);
	//startTask(armListerner);
	while ( !sendCommand(CMD_CONNECT, 0, false) ){
		displayTextLine(2, "Connecting %d", iConnect);
		iConnect ++;
		sleep(500);
	}
	displayTextLine(2, "Connected");

	sleep(7000);


	//startTask (armListerner);


	/*	goToTheTree();
	for (int h = 0; h < 10 ; h++)
	sleep(1000);*/
	//----------------------
	int i =0;

	while(i < 4){
		sendCommand(CMD_SET_LANDLE_BY_ARM,0);
		sendCommand(CMD_MOVE_PL,0);
		sendCommand(CMD_LOOK_FOR_APPLE_BY_ARM, 0);
		sleep(1000);
		int accuracy = 25;
		int shiftMM = 0;
		int sum =0;
		short x =0;
		short y =0;
		while((getCoord(y, x)) && ((x < -1 * accuracy ) || (x > accuracy ))){ // hor
			shiftMM = x / 7;
			writeDebugStreamLine("Positionary by horizont: %d", shiftMM);
			if (abs(shiftMM) > 10) {
				sum += shiftMM;
				goAheadMM(shiftMM);
				} else {
				break;
			}
		}
		if (getCoord(y, x)) {
			if (catchApple()) unloading();
			if (abs(sum) > 10) goAheadMM(-1 * sum);
			continue;
		}
		else
			sendCommand(CMD_PARK_ALL,0);

		if(i < 3)goAheadMM(120);
		i++;
	}

	//while(msgCam[2] == 0) sleep(100);
	//sleep(1000);

	sendCommand(CMD_PARK_ALL,0);
	Parking();

	vRight = vLeft = 0;
	stopAllTasks();
}

void goToTheTree(){
	startRobotPos();
#ifdef DEBUG
	sleep(1000);
#endif
	robotAngelCalibration(100);
#ifdef DEBUG
	sleep(1000);
#endif
	findTrees();
}

// gets an apple and confirms the apple in the basket. here are three attempts
bool catchApple(){
	short x =0;
	short y =0;
	int shiftPL = 100;
	const int shiftArm = 50;
	writeDebugStreamLine("Starting catch apple");
	for (int i = 0; i < 3; i++) {
		if(!getCoord(y, x)) break;
		sendCommand(CMD_SHIFT_PL_MM, shiftPL);
		sendCommand(CMD_DOWN_LANDLE, 0);
		sendCommand(CMD_SHIFT_PL_MM, -1 * shiftPL);
		sleep(100);
		if (getCoord(y, x)) return true;
		writeDebugStreamLine("Failed catch apple");
		sendCommand(CMD_SET_LANDLE_BY_ARM, 0);
		shiftPL += 10;
	}
	sendCommand(CMD_SHIFT_ARM_MM, shiftArm); // skip unride apple
	return false;
}

void unloading(){
	writeDebugStreamLine("Starting unload");
	int tmp = getArmMM();
	sendCommand(CMD_MOVE_PL, 0);
	rotatePlatform(-90);
	sendCommand(CMD_UP_ARM, 0);
	sendCommand(CMD_DOWN_LANDLE, 90);
	rotatePlatform(0);
	sendCommand(CMD_UP_ARM, tmp);
	writeDebugStreamLine("Finish unload");
	//sendCommand(CMD_DOWN_LANDLE, 45);
}
/*
task armListerner() {
ubyte idOld = 128;
COMMAND cmd ;

while(true){
if ((inDelivery.Size > 0) && (inDelivery.Status == MSG_STATUS_DELIVERED)){
if (inDelivery.Size >= MSG_HEADER_SIZE + COMMAND_MSG_SIZE){
getCommand(inDelivery.Msg, cmd);
if ((cmd == CMD_CORD) && (idOld != inDelivery.Msg[MSG_HEAD_INDEX_ID])) {
getValue(inDelivery.Msg, msgCam[0], msgCam[1], msgCam[2]);
idOld = inDelivery.Msg[MSG_HEAD_INDEX_ID];
}
}
sleep(200);
}
}
}

*/
/*
task BlueToothListener()
{
while(true) {
if (bQueuedMsgAvailable()) {
msgCam[0] = messageParm[0];
msgCam[1] = messageParm[1];
msgCam[2] = messageParm[2];
ClearMessage();
}
sleep(200);
}
}
*/

int getArmMM(){
	for(int i = 0; i < 3; i++){
		writeDebugStreamLine("Getting Arm mm");
		if(sendCommand(CMD_GET_ARM_MM,0)){
			int tmp =0;
			memcpy(&tmp, outDelivery.Msg[4], 4);
			return tmp;
		}
		sleep(500);
		writeDebugStreamLine("Failed get Arm mm");
	}
	return ARM_MAX_POSITION_270MM;
}

bool getCoord(short &p1, short &p2){
	for(int i = 0; i < 3; i++){
		writeDebugStreamLine("Getting coord");
		if(sendCommand(CMD_GET_COORD, 0)){
			int tmp = 0;
			memcpy(&tmp, outDelivery.Msg[12], 4);

			if((short)tmp == 0) return false; // here isn't an apple
				memcpy(&tmp, outDelivery.Msg[4], 4);
			p1 = (short) tmp;
			memcpy(&tmp, outDelivery.Msg[8], 4);
			p2 = (short) tmp;
			return true;
		}
		writeDebugStreamLine("Failed get coord");
		sleep(500);
	}
	return false;
}

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
		int s = SensorValue(sFront) * 10 ;
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

				robotAngelCalibration(100);
				goAheadMM(-60);
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
	if (dist < 10) return ;
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

//----------------------------------------------------------------------------- - -|
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
