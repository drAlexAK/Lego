#pragma config(Sensor, S4,     sLightLeft,     sensorI2CCustom9V)
#pragma config(Sensor, S1,     sLightRight,    sensorI2CCustom9V)
#pragma config(Sensor, S3,     sTouch,         sensorTouch)
#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "mindsensors-lineleader.h"

// time: 	    08:20 - 08:40 (left) 8:60 - 8:80 (right) fast
// direction: left
// motors: 	  2
// ger: 	    direct
// tire:      \__/ #61481 + #56145c04

#define RELEASE
//#define DEBUG
//--------------------
typedef struct power{
	byte left;
	byte right;
}power;
//--------------------
int getRWRight(tByteArray rawLight);
int getRWLeft(tByteArray rawLight);
void calibrate ();
void waitTouchRelease();
int getRec(power *p);
//--------------------
int vBase 			= 70;
int const vMax  = 100;
int const vMin	= 10;
int const maxI	= 10;
float const k 	= 35;
int iAlert			= 0;
bool leftAlert  = false;
bool rightAlert = false;
//--------------------
bool playOn      = false;
power p[300];
//--------------------
task speedUp()
{
	int vFinish        = vBase;
	int const vStart   = 30;
	int const tSpeedUp = 600;
	int tSleep = tSpeedUp / ( vFinish - vStart );

	for ( int i = vStart ; i <= vFinish ; i++ )
	{
		vBase = i ;
		sleep(tSleep);
	}
}

//--------------------
task main()
{

	LLinit(sLightLeft); 	// Set up Line Leader sensor type
	LLinit(sLightRight); 	// Set up Line Leader sensor type
	_lineLeader_cmd(sLightLeft, 'E'); // European frequency compensation
	_lineLeader_cmd(sLightRight, 'E'); // European frequency compensation

	int recToPlay = getRec(p);
	calibrate();
	waitTouchRelease();
	startTask(speedUp);

	tByteArray rawLightLeft;
	tByteArray rawLightRight;

	int e       		= 0;
	int fps 				= 0;
	int eOld 				= e;
	int rwLeft  		= getRWLeft(rawLightLeft);
	int rwRight 		= getRWRight(rawLightRight);
	int es 					= rwLeft - rwRight;
	float i 				= 0;
	int 	v 				= 0 ;
	int   u					= 0 ;
	int vLeft 			= 0;
	int vRight 			= 0;
	long iSpeed     = 0;
	int iRec        = 0;
	leftAlert  			= false;
	rightAlert 			= false;
	tByteArray rawLightLeft;
	tByteArray rawLightRight;

	clearTimer(T1);

	while(true)
	{
		rwLeft = getRWLeft(rawLightLeft);
		rwRight = getRWRight(rawLightRight);
		//-------------
		if ((rightAlert == false) && (rawLightLeft[7] < 20) && ((rawLightLeft[1] > 40) || (rawLightLeft[0] > 40))) leftAlert = true;
		if (leftAlert && (iAlert > 0)) leftAlert = ((rawLightLeft[4] > 20) &&
			(rawLightLeft[3] > 20) &&
		(rawLightLeft[2] > 20) &&
		(rawLightLeft[1] > 20) &&
		(rawLightLeft[0] > 20) &&
		(rawLightRight[0] > 20) &&
		(rawLightRight[1] > 20) &&
		(rawLightRight[2] > 20) &&
		(rawLightRight[3] > 20) &&
		(rawLightRight[4] > 20) &&
		(rawLightRight[5] > 20) &&
		(rawLightRight[6] > 20) &&
		(rawLightRight[7] > 20));
		if (leftAlert) rightAlert = false;

		if ((leftAlert == false) && (rawLightRight[0] < 20) && ((rawLightRight[6] > 40) || (rawLightRight[7] > 40))) rightAlert = true;
		if (rightAlert && (iAlert > 0)) rightAlert = ((rawLightRight[3] > 20) &&
			(rawLightRight[4] > 20) &&
		(rawLightRight[5] > 20) &&
		(rawLightRight[6] > 20) &&
		(rawLightRight[7] > 20) &&
		(rawLightLeft[0] > 20) &&
		(rawLightLeft[1] > 20) &&
		(rawLightLeft[2] > 20) &&
		(rawLightLeft[3] > 20) &&
		(rawLightLeft[4] > 20) &&
		(rawLightLeft[5] > 20) &&
		(rawLightLeft[6] > 20) &&
		(rawLightLeft[7] > 20));
		if (rightAlert) leftAlert = false;
		//--------------
		e = rwLeft - rwRight - es;

		if (leftAlert)
		{
			if (playOn == false) playOn = true;
			vLeft  -= 5;
			vRight += 5;
			iAlert ++;
		}
		else if (rightAlert)
		{
			if (playOn == false) playOn= true;
			vLeft  += 5;
			vRight -= 5;
			iAlert ++;
		}
		else if (playOn)
		{
			iRec  = 0;
			clearTimer(T1);
			while((recToPlay > iRec) && (time1[T1] < 4000) )
			{
				motor[mLeft] = p[iRec].left;
				motor[mRight] = p[iRec].right;
				sleep(25);
				iRec ++;
			}
			eOld = 0;
			i = 0;
			iSpeed = 0;
			iAlert =0;
			motor[mLeft] = 0;
			motor[mRight] = 0;
			return;
		}
		else
		{
			if ( iAlert != 0 )
			{
				i = 0;
				iSpeed = 0;
			}

			i = i + e / 2500;
			if ( fabs(i) > maxI ) i = sgn(i) * maxI ;
			u = (e * 1  + (e - eOld ) * 7) / k  + i;
			v = (vBase - abs (u) * 0.65 ) ;

			vLeft = v + u;
			vRight = v - u;

			iAlert = 0;
		}

		eOld = e ;

		if (vLeft  < vMin)  vLeft  = vMin;
		if (vRight < vMin)  vRight = vMin;
		if (vLeft  > vMax)  vLeft  = vMax;
		if (vRight > vMax)  vRight = vMax;

#ifdef RELEASE
		motor[mLeft]  = vLeft;
		motor[mRight] = vRight;
#endif
#ifdef DEBUG
		displayTextLine(0,"rwL %d",rwLeft);
		displayTextLine(1,"rwR %d", rwRight);
		displayTextLine(2,"lAlert %d", leftAlert);
		displayTextLine(3,"rAlert %d",rightAlert);
		displayTextLine(4,"e %d",e);
		displayTextLine(5,"u %d",u);
		displayTextLine(6,"vLeft %d",vLeft);
		displayTextLine(7, "vRight %d",vRight);
#endif
		//sleep (1);
		//if (SensorValue(sTouch) == 1) break;
	}
	LLsleep(sLightLeft);  // Sleep to conserve power when not in use
	LLsleep(sLightRight); // Sleep to conserve power when not in use

	motor[mLeft]  = 0;
	motor[mRight] = 0;

}

//---------------------------------

void calibrate ()
{
	displayBigTextLine(4, "  WHITE");

	while(true)
	{
		if (nNxtButtonPressed == 3)
		{
			while (nNxtButtonPressed == 3)
			{
				sleep (10);
			}
			break;
		}
		sleep (10);
	}
	LLcalWhite(sLightLeft);
	LLcalWhite(sLightRight);

	playSound(soundBlip);

	displayBigTextLine(4, "   Done");

	sleep(1000);

	displayBigTextLine(4, "  BLACK");

	while(true)
	{
		if (nNxtButtonPressed == 3)
		{
			while (nNxtButtonPressed == 3)
			{
				sleep (10);
			}
			break;
		}
		sleep (10);
	}
	LLcalBlack(sLightLeft);
	LLcalBlack(sLightRight);

	playSound(soundBlip);

	displayBigTextLine(4, "  Done");

	sleep(1000);
}

//-------------------------------

void waitTouchRelease()
{
	displayBigTextLine(4, " START");

	while(true)
	{
		if (nNxtButtonPressed == 3)
		{
			while (nNxtButtonPressed == 3)
			{
				sleep (10);
			}
			break;
		}
		sleep (10);
	}
	eraseDisplay();
}
//---------------
int getRWLeft(tByteArray rawLight)
{

	LLreadSensorRaw(sLightLeft, rawLight);
	int r =
	(
	rawLight[0]  +
	rawLight[1]  +
	rawLight[2]  +
	rawLight[3]  +
	rawLight[4]  +
	rawLight[5]  +
	rawLight[6]  +
	rawLight[7]
	) ;
	if (( rawLight[7] < 10 ) && ( rawLight[0] > 10 )) r = r * -1;
	return r;
}
//--------------------
int getRWRight(tByteArray rawLight)
{

	LLreadSensorRaw(sLightRight, rawLight);
	int r =
	(
	rawLight[0] +
	rawLight[1] +
	rawLight[2] +
	rawLight[3] +
	rawLight[4] +
	rawLight[5] +
	rawLight[6] +
	rawLight[7]
	) ;
	if (( rawLight[0] < 10 ) && ( rawLight[7] > 10 )) r = r * -1;
	return r;
}
// returns records
int getRec(power *p){
	TFileHandle hFile;
	TFileIOResult ioResult;
	byte rByte;
	string fileName = "r0.bin";
	short fileSize = 0;


	OpenRead(hFile, ioResult, fileName, fileSize);

	int rec = fileSize / sizeof(power);
	for(int i = 0; i < rec; i++){
		ReadByte(hFile, ioResult, rByte);
		p[i].left = rByte;
		ReadByte(hFile, ioResult, rByte);
		p[i].right= rByte;
	}
	Close(hFile, ioResult);
	return rec;
}