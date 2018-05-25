
#include "nxtPipe.h"

#define COMMAND_MSG_SIZE 4
#define ARM_MAX_POSITION_270MM    270

typedef enum COMMAND {
	CMD_MOVE_PL								= 1,
	CMD_UP_ARM								= 2,
	CMD_UP_ARM_STR_VERT				= 3,
	CMD_DOWN_LANDLE		  			= 4,
	CMD_PARK_ALL							= 5,
	CMD_LOOK_FOR_APPLE_BY_ARM = 6,
	CMD_SHIFT_PL_MM					  = 7,
	CMD_CONNECT								= 8,
	CMD_GET_COORD							= 9,
	CMD_SET_LANDLE_BY_ARM			= 10,
	CMD_SHIFT_ARM_MM 					= 11,
	CMD_GET_ARM_MM						= 12,
	CMD_SAVE_ARM_MM						= 13,
	CMD_RESTORE_ARM_MM				= 14
} COMMAND;

short msgCam[3] = {0,0,0};

//typedef char commandMsg[COMMAND_MSG_SIZE];
//----------------------------------------
bool sendCommand(COMMAND cmd);
bool sendCommand(COMMAND cmd, int value);
bool sendCommand(COMMAND cmd, int value, bool waitComplete);
void getMsgCoord(char *body, short v1, short v2, short v3);
void getCommand(char *msg, COMMAND &cmd);
void getValue(char *msg, int &value);
void getValue(char *msg, short &v1, short &v2, short &v3);
int getLimitSpeed(const int speedMin, int speedMax, int startEnc, int currentEnc, int targetEnc);
task BlueToothListener();

task BlueToothListener()
{
	while(true) {
		if (bQueuedMsgAvailable()) {
			msgCam[0] = messageParm[0];
			msgCam[1] = messageParm[1];
			msgCam[2] = messageParm[2];
			ClearMessage();
		}
		sleep(50);
	}
}

//-----------------------------------------
bool sendCommand(COMMAND cmd, int value, bool waitComplete){
	const int messageSize = 8;
	char msg[messageSize];
	memcpy(&msg[0], &cmd, sizeof(int));
	memcpy(&msg[sizeof(int)], &value, sizeof(int));
	return SendMsg(&msg[0], messageSize, waitComplete, 3, 3000);
}

bool sendCommand(COMMAND cmd, int value){
	return sendCommand(cmd, value, true);
}

bool sendCommand(COMMAND cmd){
	return sendCommand(cmd, 0, true);
}

void getMsgCoord(char *body, short v1, short v2, short v3){
	memcpy(&body[0], &v1, sizeof(short));
	memcpy(&body[sizeof(int)*1], &v2, sizeof(short));
	memcpy(&body[sizeof(int)*2], &v3, sizeof(short));
}

void getIntAsArray(char *body, int v){
	memcpy(&body[0], &v, sizeof(int));
}

void getCommand(char *msg, COMMAND &cmd){
	memcpy(&cmd, &msg[MSG_HEADER_SIZE], sizeof(int));
}

void getValue(char *msg, int &value){
	memcpy(&value, &msg[MSG_HEADER_SIZE + sizeof(int)], sizeof(int));
}

void getValue(char *msg, short &v1, short &v2, short &v3){
	memcpy(&v1, &msg[MSG_HEADER_SIZE + sizeof(int)],     sizeof(short));
	memcpy(&v2, &msg[MSG_HEADER_SIZE + sizeof(int) * 2], sizeof(short));
	memcpy(&v3, &msg[MSG_HEADER_SIZE + sizeof(int) * 3], sizeof(short));
}
// speed limiter
int getLimitSpeed(const int speedMin,
				  const int speedMax,
				  const int startEnc,
				  const int currentEnc,
				  const int targetEnc){

					  const int maxEnc = 360; //540
					  int dir = sgn(targetEnc - currentEnc); // direction
					  int complPercent = 0;

					  if(dir * (targetEnc) < dir * (currentEnc)) return dir * speedMin; // current encoder longer than target
					  if ((dir * (currentEnc - startEnc  - dir * maxEnc) > 0) &&
						  (dir * (currentEnc - targetEnc + dir * maxEnc) < 0)) return dir * speedMax; // current enc longer than maxEncoder and shorter than target enc - maxEncoder

					  if (abs(targetEnc - startEnc) < maxEnc * 2){
						  if( abs(currentEnc - startEnc) < abs((targetEnc - startEnc) / 2))
							  complPercent = abs ((currentEnc - startEnc) * 100 / maxEnc);
						  else
							  complPercent = (abs(targetEnc - startEnc) * 100 / maxEnc - (abs(currentEnc - startEnc) * 100 / maxEnc));
								complPercent =  abs(complPercent);
					  }
					  else if (dir * (currentEnc - startEnc  - dir * maxEnc) <= 0)  {
						  complPercent = abs((currentEnc - startEnc) * 100 / (maxEnc));
					  } else {
						  complPercent = 100 - abs((currentEnc - (targetEnc - dir * maxEnc)) * 100 / maxEnc);
					  }

					return (dir * (speedMin + (speedMax - speedMin) * complPercent / 100));
}
