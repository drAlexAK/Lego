
#include "nxtPipe.h"

#define COMMAND_MSG_SIZE 5

typedef enum COMMAND {
	CMD_ROTATE_PLATFORM = 1,
	CMD_UP_ARM					= 2,
	CMD_UP_LANDLE				= 3,
	CMD_PARK_ALL				= 4
} COMMAND;

typedef char commandMsg[5];

bool sendCommand(COMMAND cmd, int value){
	commandMsg msg;
	msg[0] = cmd;
	memcpy(&msg[1], &value, sizeof(int) * 1);
	return SendMsg(&msg[0], COMMAND_MSG_SIZE, true, 3, 15000);
}

int getLimitSpeed(const int speedMin, int speedMax, int currentEnc, int targetEnc);

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
