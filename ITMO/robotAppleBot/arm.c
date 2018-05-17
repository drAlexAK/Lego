#pragma config(Sensor, S1,     ,               sensorI2CCustomFastSkipStates9V)
#pragma config(Motor,  motorA,          mPl,           tmotorNXT, PIDControl, reversed, encoder)
#pragma config(Motor,  motorB,          mArm,          tmotorNXT, PIDControl, reversed, encoder)
#pragma config(Motor,  motorC,          mLandle,       tmotorNXT, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "alex-common.h"
#include "shared.h"
#include "nxtPipe.h"
#include "mindsensors-irdist.h"
//----------------------------
//
//#define DEBUG
//
#define ARM_270MM_ENCODER 				9340
#define ARM_MAX_POSITION_270MM    270
#define M_ARM_SPEED_MIN           20
#define M_ARM_SPEED_MAX           100
#define LANDLE_11000_ENCODER      2000 // 11000
#define M_LANDLE_SPEED_MIN        10
#define M_LANDLE_SPEED_MAX        100
//----------------------------
#define MAX_CENTER_MM 125
#define MAX_CENTER_ENC 	1810
#define M_PL_SPEED_MIN 10
#define M_PL_SPEED_MAX 80
//----------------------------
// semaphore variables
TSemaphore  semParkingArm;
TSemaphore  semParkingPl;
TSemaphore  semParkingLandle;
//----------------------------
void upArmMM(int posit);
void upArmMMStrongVert(int posit);
void resetMotorsEncoder();
void downLandle(int angel);
void Parking();
task ParkingArm();
task ParkingPl();
task ParkingLandle();
task holdPlPositionByArm();
task BlueToothListener();
task verticalLandlePositionByArm();
void executeCMD(COMMAND cmd,int value);
void InitArmDiffMM();
void InitLandleDiffEnc();
void movePl(int posit);
void lookForAppleByArm();
bool isAppleHere();
int getLandlePositionByArmEnc(int enc);
int getPlPositionByArmEnc(int enc);
int getPlPositionByArmMM(int posit);
int getPlCurrentPositionMM();
//----------------------------

byte armDiffMM[28];
short landleDiffEnc[28];
short msgCam[3] = {0,0,0};

task main()
{
	ubyte id =0;
	COMMAND cmd ;
	int value =0;
	InitialyzePipe();
	while( !sendCommand(CMD_CONNECT,0, false) ){
	sleep(500);
	}

	startTask(BlueToothListener);
	InitArmDiffMM();
	InitLandleDiffEnc();
	resetMotorsEncoder();
	sleep(500);
	//////////////////////////

	//////////////////////////
	//startTask(holdPlPositionByArm);

	//int id =0;

	while(true){
		if((inDelivery.Size > 0) && (inDelivery.Status == MSG_STATUS_DELIVERED)){
			id = inDelivery.Msg[MSG_HEAD_INDEX_ID];
			if (inDelivery.Size >= MSG_HEADER_SIZE + COMMAND_MSG_SIZE ){
				getCommand(inDelivery.Msg, cmd);
				if ( cmd != CMD_CORD) {
					getValue(inDelivery.Msg, value);
					executeCMD(cmd, value);
					if (id == inDelivery.Msg[MSG_HEAD_INDEX_ID]) {
						inDelivery.Status = (MSG_STATUS) MSG_STATUS_COMPLETED;
					}
					SendCompleteReplayMsg(id);
				}
			}
		}
		sleep(200);
	}



	/*sleep(10000);
	downLandle(0);
	sleep(1000);
	upArmMM(0);
	downLandle(90);

	sleep(10000);

	Parking();
*/
	stopAllTasks();
}

task BlueToothListener()
{
	while(true) {
		if (bQueuedMsgAvailable()) {
			msgCam[0] = messageParm[0];
			msgCam[1] = messageParm[1];
			msgCam[2] = messageParm[2];
			ClearMessage();
			//sendCoord(msgCam[0], msgCam[1], msgCam[2]);
		}
		sleep(100);
	}
}

void resetMotorsEncoder() {
	nMotorEncoder[mArm]    = 0;
	nMotorEncoder[mLandle] = 0;
	nMotorEncoder[mPl]     = 0;
}

void executeCMD(COMMAND cmd, int value){
	switch (cmd)
	{
	case CMD_UP_ARM:
		upArmMM(value);
		break;
	case CMD_UP_ARM_STR_VERT:
		upArmMMStrongVert(value);
		break;
	case CMD_MOVE_PL:
		movePl(value);
		break;
	case CMD_DOWN_LANDLE:
		downLandle(value);
		break;
	case CMD_LOOK_FOR_APPLE_BY_ARM:
		lookForAppleByArm();
		break;
		case CMD_MOVE_PL_10MM:
		int mm = nMotorEncoder[mPl] / (MAX_CENTER_ENC / MAX_CENTER_MM);
	movePl(100+mm);
		break;
	case CMD_PARK_ALL:
	default:
		Parking();
		break;
	}
}

void Parking(){
	semaphoreInitialize(semParkingArm);
	semaphoreInitialize(semParkingPl);
	semaphoreInitialize(semParkingLandle);

	startTask(ParkingArm);
	sleep(100);
	startTask(ParkingPl);
	sleep(100);
	startTask(ParkingLandle);

	sleep(100);
	semaphoreLock( semParkingArm );
	semaphoreLock( semParkingPl );
	semaphoreLock( semParkingLandle );

	if (bDoesTaskOwnSemaphore(semParkingArm)) semaphoreUnlock(semParkingArm);
	if (bDoesTaskOwnSemaphore(semParkingPl)) semaphoreUnlock(semParkingPl);
	if (bDoesTaskOwnSemaphore(semParkingLandle)) semaphoreUnlock(semParkingLandle);
}

task ParkingArm(){
	semaphoreLock( semParkingArm );
	upArmMM(0);
	if (bDoesTaskOwnSemaphore(semParkingArm)) semaphoreUnlock(semParkingArm);
}
task ParkingLandle(){
	semaphoreLock( semParkingLandle );
	downLandle(0);
	if (bDoesTaskOwnSemaphore(semParkingLandle)) semaphoreUnlock(semParkingLandle);
}
task ParkingPl(){
	semaphoreLock( semParkingPl );
	movePl(0);
	if (bDoesTaskOwnSemaphore(semParkingPl)) semaphoreUnlock(semParkingPl);
}

void downLandle(int angel)
{
	if (angel < 0)  angel  = 0;
	if (angel > 90) angel = 90;
	int	targetEnc = LANDLE_11000_ENCODER / 90 * angel;
	int startEnc = nMotorEncoder[mLandle];
	int speed = 0 ;
	if((targetEnc - startEnc) > 0){
		while(nMotorEncoder[mLandle] < targetEnc){
			speed = getLimitSpeed(M_LANDLE_SPEED_MIN, M_LANDLE_SPEED_MAX, startEnc, nMotorEncoder[mLandle], targetEnc);
			motor[mLandle] = speed;
		}
		} else {
		while(nMotorEncoder[mLandle] > targetEnc){
			speed = getLimitSpeed(M_LANDLE_SPEED_MIN, M_LANDLE_SPEED_MAX, startEnc, nMotorEncoder[mLandle], targetEnc);
			motor[mLandle]= speed;
		}
	}
	motor[mLandle] = 0;
}

void lookForAppleByArm() {

	startTask(holdPlPositionByArm);
	startTask(verticalLandlePositionByArm);
	sleep(250);

	int posit = ARM_MAX_POSITION_270MM;
	int speedMax = 0;
	int	targetEnc = (ARM_270MM_ENCODER * posit) / ARM_MAX_POSITION_270MM;
	int startEnc  = nMotorEncoder[mArm];
	int speed = 0 ;
	if ((targetEnc - startEnc) > 0){
		while(nMotorEncoder[mArm] < targetEnc){
			speedMax = (msgCam[2] == 1 ? M_ARM_SPEED_MAX / 2 : M_ARM_SPEED_MAX); // if cam sees an apple motor will slow down
			speed = getLimitSpeed(M_ARM_SPEED_MIN, speedMax, startEnc, nMotorEncoder[mArm], targetEnc);
			if (isAppleHere()) break;
			motor[mArm] = speed;
		}
		} else {
		while(nMotorEncoder[mArm] > targetEnc){
			speedMax = (msgCam[2] == 1 ? M_ARM_SPEED_MAX / 2 : M_ARM_SPEED_MAX); // if cam sees an apple motor will slow down
			speed = getLimitSpeed(M_ARM_SPEED_MIN, speedMax, startEnc, nMotorEncoder[mArm], targetEnc);
			if (isAppleHere()) break;
			motor[mArm] = speed;
		}
	}
	motor[mArm] = 0;
	sleep(500);
	motor[mPl]=0;
	motor[mLandle]=0;
	stopTask(holdPlPositionByArm);
	stopTask(verticalLandlePositionByArm);
}

bool isAppleHere() {
	const int accuracy = -50;
	const int shiftPosition = -120;
	if (msgCam[2] == 1) { //apple here
		if (((msgCam[0] - shiftPosition) < 0) && ((msgCam[0] - shiftPosition) > accuracy)) return true;
	}
	return false;
}

void upArmMM(int posit){
	if (posit < 0) posit = 0;
	if (posit > ARM_MAX_POSITION_270MM) posit = ARM_MAX_POSITION_270MM;

	int	targetEnc = (ARM_270MM_ENCODER * posit) / ARM_MAX_POSITION_270MM;
	int startEnc  = nMotorEncoder[mArm];
	int speed = 0 ;
	if ((targetEnc - startEnc) > 0){
		while(nMotorEncoder[mArm] < targetEnc){
			speed = getLimitSpeed(M_ARM_SPEED_MIN, M_ARM_SPEED_MAX, startEnc, nMotorEncoder[mArm], targetEnc);
			motor[mArm] = speed;
		}
		} else {
		while(nMotorEncoder[mArm] > targetEnc){
			speed = getLimitSpeed(M_ARM_SPEED_MIN, M_ARM_SPEED_MAX, startEnc, nMotorEncoder[mArm], targetEnc);
			motor[mArm] = speed;
		}
	}
	motor[mArm] = 0;
}

int getPlPositionByArmEnc(int enc){
	int armPositMM = ARM_MAX_POSITION_270MM  * enc / ARM_270MM_ENCODER;
	return getPlPositionByArmMM(armPositMM);
	//return (MAX_CENTER_ENC * platformPositionMM) / MAX_CENTER_MM ;
}

int getPlCurrentPositionMM(){
	return nMotorEncoder[mArm] * MAX_CENTER_MM / MAX_CENTER_ENC;
}

int getPlPositionByArmMM(int posit){
	posit = abs(posit);
	if (posit > ARM_MAX_POSITION_270MM) posit = ARM_MAX_POSITION_270MM;
	if (posit % 10 == 0) return armDiffMM[posit/10];
	int a = posit / 10;
	int b = a + 1;
	if (( b > 27 ) || ( a> 27 )) return armDiffMM[27];
	return armDiffMM[a] + ((armDiffMM[b] - armDiffMM[a]) * (posit - a * 10) / (b * 10 - a * 10));
}


void movePl(int posit){
	if (abs(posit) > MAX_CENTER_MM) posit = sgn(posit) * MAX_CENTER_MM;
	int	targetEnc = (MAX_CENTER_ENC * posit) / MAX_CENTER_MM;
	int startEnc = nMotorEncoder[mPl];
	int speed = 0;
	if (abs(targetEnc - startEnc) < 10) return;
	if((targetEnc - startEnc) > 0){
		while(nMotorEncoder[mPl] < targetEnc){
			speed = getLimitSpeed(M_PL_SPEED_MIN, M_PL_SPEED_MAX, startEnc, nMotorEncoder[mPl], targetEnc);
			motor[mPl] = speed;
		}
		} else {
		while(nMotorEncoder[mPl] > targetEnc){
			speed = getLimitSpeed(M_PL_SPEED_MIN, M_PL_SPEED_MAX, startEnc, nMotorEncoder[mPl], targetEnc);
			motor[mPl]= speed;
		}
	}
	motor[mPl]=0;
}

task verticalLandlePositionByArm()
{
	const int accuracy = 3;
	int targetEnc = 0;
	int startEnc = 0;
	int speed = 0;

	while(true){
		targetEnc = getLandlePositionByArmEnc(nMotorEncoder[mArm]);
		startEnc = nMotorEncoder[mLandle];
		if((targetEnc - startEnc) > 0){
			while(((nMotorEncoder[mLandle] - accuracy) < targetEnc) && ((nMotorEncoder[mLandle] + accuracy) < targetEnc)){
				speed = getLimitSpeed(M_PL_SPEED_MIN, M_PL_SPEED_MAX, startEnc, nMotorEncoder[mLandle], targetEnc);
				motor[mLandle] = speed;
			}
			} else {
			while(((nMotorEncoder[mLandle] - accuracy) > targetEnc) && ((nMotorEncoder[mLandle] + accuracy) > targetEnc)) {
				speed = getLimitSpeed(M_PL_SPEED_MIN, M_PL_SPEED_MAX, startEnc, nMotorEncoder[mLandle], targetEnc);
				motor[mLandle]= speed;
			}
		}
		motor[mLandle]=0;
		sleep(10);
	}
}


task holdPlPositionByArm()
{
	const int accuracy = 3;
	int HoldPositionPlEnc = nMotorEncoder[mPl];
	int targetEnc = 0;
	int startEnc = 0;
	int speed = 0;

	while(true){
		targetEnc = HoldPositionPlEnc - (MAX_CENTER_ENC * getPlPositionByArmEnc(nMotorEncoder[mArm])) / MAX_CENTER_MM;
		if (abs(targetEnc) > MAX_CENTER_ENC) targetEnc = sgn(targetEnc) * MAX_CENTER_ENC;
		startEnc = nMotorEncoder[mPl];
		if((targetEnc - startEnc) > 0){
			while(((nMotorEncoder[mPl] - accuracy) < targetEnc) && ((nMotorEncoder[mPl] + accuracy) < targetEnc)){
				speed = getLimitSpeed(M_PL_SPEED_MIN, M_PL_SPEED_MAX, startEnc, nMotorEncoder[mPl], targetEnc);
				motor[mPl] = speed;

			}
			} else {
			while(((nMotorEncoder[mPl] - accuracy) > targetEnc) && ((nMotorEncoder[mPl] + accuracy) > targetEnc)) {
				speed = getLimitSpeed(M_PL_SPEED_MIN, M_PL_SPEED_MAX, startEnc, nMotorEncoder[mPl], targetEnc);
				motor[mPl]= speed;
			}
		}
		motor[mPl]=0;
		sleep(10);
	}
}

void upArmMMStrongVert(int posit){
	int startPos = nMotorEncoder[mArm] * ARM_MAX_POSITION_270MM / ARM_270MM_ENCODER;
	if (posit<0) posit = 0;
	if (posit>ARM_MAX_POSITION_270MM) posit = ARM_MAX_POSITION_270MM;
	nMotorEncoder[mArm] =0;
	int	enc = (ARM_270MM_ENCODER * posit) / ARM_MAX_POSITION_270MM - nMotorEncoder[mArm];

	int currentEnc = nMotorEncoder[mArm];
	int speed = 0 ;
	int pp = 0;
	if(enc > 0){
		while(currentEnc < enc){
			pp = getPlPositionByArmEnc(abs(currentEnc)) - getPlCurrentPositionMM();
			movePl(pp+10);
			speed = getLimitSpeed(M_ARM_SPEED_MIN, M_ARM_SPEED_MAX,startPos, currentEnc, enc);
			motor[mArm] = speed;
			currentEnc = nMotorEncoder[mArm];
		}
		} else {
		while(currentEnc > enc){
			pp = getPlPositionByArmEnc(abs(currentEnc)) - getPlCurrentPositionMM();
			movePl(pp-10);
			speed = getLimitSpeed(M_ARM_SPEED_MIN, M_ARM_SPEED_MAX,startPos, currentEnc, enc);
			motor[mArm]= -1 * speed;
			currentEnc = nMotorEncoder[mArm];
		}
	}
	motor[mArm]=0;
}

int getLandlePositionByArmEnc(int enc){
	const int armMM2Enc = 325;
	int index = abs(enc / armMM2Enc);
	if (index > 27) index = 27;
	if (index < 0) index = 0;
	return landleDiffEnc[index];

}

void InitLandleDiffEnc() {
	// arm encoder multiplication x345
	landleDiffEnc[0] = 1208;
	landleDiffEnc[1] = 1233;
	landleDiffEnc[2] = 1259;
	landleDiffEnc[3] = 1308;
	landleDiffEnc[4] = 1383;
	landleDiffEnc[5] = 1444;
	landleDiffEnc[6] = 1494;
	landleDiffEnc[7] = 1532;
	landleDiffEnc[8] = 1603;
	landleDiffEnc[9] = 1640;
	landleDiffEnc[10] = 1638;
	landleDiffEnc[11] = 1715;
	landleDiffEnc[12] = 1750;
	landleDiffEnc[13] = 1750;
	landleDiffEnc[14] = 1792;
	landleDiffEnc[15] = 1826;
	landleDiffEnc[16] = 1832;
	landleDiffEnc[17] = 1883;
	landleDiffEnc[18] = 1910;
	landleDiffEnc[19] = 1920;
	landleDiffEnc[20] = 1920;
	landleDiffEnc[21] = 1960;
	landleDiffEnc[22] = 1960;
	landleDiffEnc[23] = 1960;
	landleDiffEnc[24] = 1960;
	landleDiffEnc[25] = 1960;
	landleDiffEnc[26] = 1960;
	landleDiffEnc[27] = 1960;
}

void InitArmDiffMM() {
	/*
	armDiffMM[0] = 0; //5;
	armDiffMM[1] = 3; //8;
	armDiffMM[2] = 5; //13;
	armDiffMM[3] = 7;
	armDiffMM[4] = 9;
	armDiffMM[5] = 10;
	armDiffMM[6] = 10;
	armDiffMM[7] = 11;
	armDiffMM[8] = 11;
	armDiffMM[9] = 10;
	armDiffMM[10] = 7;
	armDiffMM[11] = 6;
	armDiffMM[12] = 3;
	armDiffMM[13] = 2;
	armDiffMM[14] = 3;
	armDiffMM[15] = 0;
	armDiffMM[16] = -2;
	armDiffMM[17] = -3;
	armDiffMM[18] = -6;
	armDiffMM[19] = -9;
	armDiffMM[20] = -10;
	armDiffMM[21] = -10; //-15;
	armDiffMM[22] = -10; //-18;
	armDiffMM[23] = -10; //-24;
	armDiffMM[24] = -10; //-31;
	armDiffMM[25] = -10; //-40;
	armDiffMM[26] = -10; //-47;
	armDiffMM[27] = -10; //-58;
	*/
	armDiffMM[0] = 0; //5;
	armDiffMM[1] = 7; //8;
	armDiffMM[2] = 12; //13;
	armDiffMM[3] = 17;
	armDiffMM[4] = 21;
	armDiffMM[5] = 24;
	armDiffMM[6] = 27;
	armDiffMM[7] = 31;
	armDiffMM[8] = 34;
	armDiffMM[9] = 38;
	armDiffMM[10] = 40;
	armDiffMM[11] = 43;
	armDiffMM[12] = 45;
	armDiffMM[13] = 47;
	armDiffMM[14] = 49;
	armDiffMM[15] = 50;
	armDiffMM[16] = 50;
	armDiffMM[17] = 52;
	armDiffMM[18] = 51;
	armDiffMM[19] = 52;
	armDiffMM[20] = 48;
	armDiffMM[21] = 45; //-15;
	armDiffMM[22] = 44; //-18;
	armDiffMM[23] = 41; //-24;
	armDiffMM[24] = 37; //-31;
	armDiffMM[25] = 31; //-40;
	armDiffMM[26] = 27; //-47;
	armDiffMM[27] = 20; //-58;
}


/*
void getArrayDistFromLadleDependsOnArm(){

TFileHandle   hFileHandle;              // will keep track of our file
TFileIOResult nIOResult;                // will store our IO results
string        sFileName = "Landle.txt";   // the name of our file
short         nFileSize = 1000;          // will store our file size
string result = "";

byte CR = 0x13;   // define CR (carriage return)
byte LF = 0x10;   // define LF (line feed)

OpenWrite( hFileHandle, nIOResult, sFileName, nFileSize);    // open the file for writing (creates the file if it does not exist)
int d = MSDISTreadDist(S1);
int a[7];
for(int i = 0; i <= 270; i += 10){
upArmMM(i);
sleep(3000);
for(int j =0; j <7;j++){
a[j] = MSDISTreadDist(S1);
sleep(500);
}
sprintf(result, "%d %d\n\r", i , GetMedian(a,7));
WriteText(hFileHandle, nIOResult, result);         // write 'sMessageToWrite' to the file
//WriteByte(hFileHandle, nIOResult, CR);                      // write 'CR' to the file (carriage return)
//WriteByte(hFileHandle, nIOResult, LF);                      // write 'LF' to the file (line feed)
}
Close(hFileHandle, nIOResult);                              // close our file (DON'T FORGET THIS STEP!)
}*/
