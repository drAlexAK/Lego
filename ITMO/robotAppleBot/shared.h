
#include "nxtPipe.h"

#define COMMAND_MSG_SIZE 8

typedef enum COMMAND {
	CMD_MOVE_PL					= 1,
	CMD_UP_ARM					= 2,
	CMD_UP_LANDLE				= 3,
	CMD_PARK_ALL				= 4
} COMMAND;

typedef char commandMsg[COMMAND_MSG_SIZE];
//----------------------------------------
bool sendCommand(COMMAND cmd, int value);
void getCommand(char *msg, COMMAND &cmd, int &value);
int getLimitSpeed(const int speedMin, int speedMax, int currentEnc, int targetEnc);


//-----------------------------------------
bool sendCommand(COMMAND cmd, int value){
	commandMsg msg;
	memcpy(&msg[0], &cmd, sizeof(int));
	memcpy(&msg[sizeof(int)], &value, sizeof(int));
	return SendMsg(&msg[0], COMMAND_MSG_SIZE, true, 3, 15000);
}

void getCommand(char *msg, COMMAND &cmd, int &value){

		memcpy(&cmd, &msg[MSG_HEADER_SIZE], sizeof(int));
		memcpy(&value, &msg[MSG_HEADER_SIZE + sizeof(int)], sizeof(int));
}
// speed limiter
int getLimitSpeed(const int speedMin,const int speedMax, const int currentEnc, const int targetEnc){
	const int maxEnc = 360; //540
	//int direction = sgn(targetEnc);
	currentEnc = abs(currentEnc);
	targetEnc = abs (targetEnc);

	if(targetEnc < currentEnc) currentEnc = targetEnc;
	if((currentEnc >= maxEnc) && (currentEnc <= targetEnc - maxEnc)) return speedMax;

	if(targetEnc < (maxEnc * 2)){
		int accel = (speedMax - speedMin) / maxEnc;
		speedMax = ((targetEnc / 2) * accel) + speedMin;
		} else {
		if (currentEnc > maxEnc) currentEnc -= (targetEnc - (maxEnc*2));
		targetEnc = (maxEnc * 2);
	}

	int percentComplete = (currentEnc * 100) / targetEnc;

	if(percentComplete < 50){
		return (speedMin + ((speedMax - speedMin) * percentComplete) / 100);
		}else{
		return (speedMin + (speedMax - speedMin) * (100 - percentComplete) / 100);
	}
}
