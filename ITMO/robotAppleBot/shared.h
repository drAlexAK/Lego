
#include "nxtPipe.h"

#define COMMAND_MSG_SIZE 8

typedef enum COMMAND {
	CMD_MOVE_PL					= 1,
	CMD_UP_ARM					= 2,
	CMD_UP_ARM_STR_VERT	= 3,
	CMD_UP_LANDLE				= 4,
	CMD_PARK_ALL				= 5
} COMMAND;

typedef char commandMsg[COMMAND_MSG_SIZE];
//----------------------------------------
bool sendCommand(COMMAND cmd, int value);
void getCommand(char *msg, COMMAND &cmd, int &value);
int getLimitSpeed(const int speedMin, int speedMax, int startEnc, int currentEnc, int targetEnc);


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
int getLimitSpeed(const int speedMin,
const int speedMax,
const int startEnc,
const int currentEnc,
const int targetEnc){

	const int maxEnc = 360; // 540
	int dir = sgn(targetEnc - currentEnc); // direction
	int complPercent = 0;

	if(dir * (targetEnc) < dir * (currentEnc)) return dir * speedMin; // current encoder longer than target
		if ((dir * (currentEnc - startEnc  - dir * maxEnc) > 0) &&
		(dir * (currentEnc - targetEnc + dir * maxEnc) < 0)) return dir * speedMax; // current enc longer than maxEncoder and shorter than target enc - maxEncoder

	if (abs(targetEnc - startEnc) < maxEnc * 2){
		if( currentEnc < abs((targetEnc - startEnc) / 2))
			complPercent = abs((currentEnc - startEnc) * 100 / maxEnc);
		else
		{
			complPercent = ((targetEnc - startEnc) - abs(currentEnc - startEnc)) * 100 / maxEnc;
			complPercent = abs(complPercent);
		}
	}
	else if (dir * (currentEnc - startEnc  - dir * maxEnc) <= 0)  {
		complPercent = abs((currentEnc - startEnc) * 100 / (maxEnc));
		} else {
		complPercent = 100 - abs((currentEnc - (targetEnc - dir * maxEnc)) * 100 / maxEnc);
	}

	return (dir * (speedMin + (speedMax - speedMin) * complPercent / 100));
}
