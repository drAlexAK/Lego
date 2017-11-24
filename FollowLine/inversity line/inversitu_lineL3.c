#pragma config(Sensor, S1,     sLightRight,    sensorI2CCustom9V)
#pragma config(Sensor, S3,     sLightCenter,   sensorI2CCustom9V)
#pragma config(Sensor, S4,     sLightLeft,     sensorI2CCustom9V)
#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "mindsensors-lineleader.h"

// time: 	      stable
// direction: left
// motors: 	  2
// ger: 	    direct
// tire:      \__/ #61481 + #56145c04

#define RELEASE
//#define DEBUG

typedef float tFloatArray[8];

int getRWRight( tByteArray rawLight, tFloatArray KL);
int getRWLeft ( tByteArray rawLight, tFloatArray KL);
int getRWCenter ( tByteArray rawLight, tFloatArray KL);
void calibrate ();
void waitTouchRelease();
void reverse(tByteArray a);
//-------------------
float KL[8] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
//--------------------
int vBase 			= 85;
int const vMax  = 100;
int const vMin	= 10;
int const maxI	= 10;
float const k 	= 35;
int iAlert			= 0;
bool isItBlack = false;
bool leftAlert  = false;
bool rightAlert = false;
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
	LLinit(sLightCenter); 	// Set up Line Leader sensor type
	_lineLeader_cmd(sLightLeft, 'E'); // European frequency compensation
	_lineLeader_cmd(sLightCenter, 'E'); // European frequency compensation
	_lineLeader_cmd(sLightRight, 'E'); // European frequency compensation

	calibrate();
	waitTouchRelease();

	startTask(speedUp);

	tByteArray rawLightLeft;
	tByteArray rawLightRight;
	tByteArray rawLightCenter;

	LLreadSensorRaw(sLightLeft, rawLightLeft);
	int rwLeft  		= getRWLeft(rawLightLeft, KL);

	LLreadSensorRaw(sLightRight, rawLightRight);
	int rwRight 		= getRWRight(rawLightRight, KL);

	LLreadSensorRaw(sLightCenter, rawLightRight);
	int rwCenter 		= getRWCenter(rawLightCenter, KL);
	//----------------------------------------------------------------------------

	int e       		= 0;
	int eOld 				= e;
	int es 					= rwLeft - rwRight - rwCenter ;
	float i 				= 0;
	int 	v 				= 0 ;
	int   u					= 0 ;
	int vLeft 			= 0;
	int vRight 			= 0;
	long iSpeed     = 0;
	leftAlert  			= false;
	rightAlert 			= false;


	int summSensor = 0;

	while(true)
	{
		//------------
		LLreadSensorRaw(sLightLeft, rawLightLeft);
		LLreadSensorRaw(sLightRight, rawLightRight);
		LLreadSensorRaw(sLightCenter, rawLightCenter);

		summSensor = 0;

	 isItBlack =((rawLightLeft[0] + rawLightLeft[7] + rawLightRight[0] + rawLightRight[7] + rawLightCenter[0] + rawLightCenter[7] ) < 300);
	// isItBlack =((rawLightLeft[0] + rawLightLeft[7] + rawLightRight[0] + rawLightRight[7]) < 200);
		if ( isItBlack == true )
		{
			reverse( rawLightRight );
			reverse( rawLightLeft );
			reverse( rawLightCenter );
		}

		rwLeft = getRWLeft(rawLightLeft, KL);
		rwRight = getRWRight(rawLightRight,KL);
		rwCenter = getRWCenter(rawLightRight,KL);

		//-------------

		if ((rightAlert == false) && ((rawLightLeft[7] < 20) || (rawLightLeft[6] < 20)) && ((rawLightLeft[1] > 40) || (rawLightLeft[0] > 40))) leftAlert = true;
		if (leftAlert && (iAlert > 0))
		{
			if ((	rawLightLeft[7] > 20) || (rawLightLeft[6] > 20 ) || (rawLightLeft[5] > 20 ) || (rawLightLeft[4] > 20 ))
			{
				leftAlert = ((rawLightCenter[0] > 20) && (rawLightCenter[1] > 20) && (rawLightCenter[2] > 20) && (rawLightCenter[3] > 20));
			}
		}
		/*if (leftAlert && (iAlert > 0)) leftAlert = ((rawLightCenter[0] > 20) && (rawLightLeft[4] > 20) &&
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
		(rawLightRight[7] > 20)); */
		if (leftAlert) rightAlert = false;

		if ((leftAlert == false) && ((rawLightRight[0] < 20) || (rawLightRight[1] < 20)) && ((rawLightRight[6] > 40) || (rawLightRight[7] > 40))) rightAlert = true;
		if (rightAlert && (iAlert > 0))
		{
			if ((	rawLightRight[0] > 20) || ( rawLightRight[1] > 20 ) || ( rawLightRight[2] > 20 ) || ( rawLightRight[3] > 20 ))
			{
				rightAlert = ((rawLightCenter[7] > 20) && (rawLightCenter[6] > 20) && (rawLightCenter[5] > 20)  && (rawLightCenter[4] > 20));
			}
		}

		/*if (rightAlert && (iAlert > 0)) rightAlert = ((rawLightRight[3] > 20) &&
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
		(rawLightLeft[7] > 20));*/
		if (rightAlert) leftAlert = false;
		//--------------
		e = rwLeft - rwRight - rwCenter - es;

		if (leftAlert)
		{
			vLeft  = vMin;
			vRight = 90 ;
			for (int j = 7; j >= 0; j--  )
			{
				if (rawLightLeft[j] < 20 ) vRight = vRight - 7;
			}
			iAlert ++;
		}
		else if (rightAlert)
		{
			vLeft  = 90;
			vRight = vMin;
			for (int j = 0; j <= 7; j++  )
			{
				if (rawLightRight[j] < 20 ) vLeft = vLeft - 7;
			}
			iAlert ++;
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
		sleep (1);
	}
	//if (SensorValue(sTouch) == 1) break;
	LLsleep(sLightLeft);  // Sleep to conserve power when not in use
	LLsleep(sLightRight); // Sleep to conserve power when not in use
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
	LLcalWhite(sLightCenter);

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
	LLcalBlack(sLightCenter);

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
int getRWLeft( tByteArray &rawLight, tFloatArray &KL)
{
	int r = 0 ;
	for(int i = 0; i < 8; i++)
	{
		r = r + rawLight[i] * KL[i];
	}
	if (( rawLight[7] < 10 ) && ( rawLight[0] > 10 )) r = r * -1;
	return r;
}
//--------------------
int getRWRight ( tByteArray &rawLight, tFloatArray &KL)
{
	int r = 0 ;
	for(int i = 0; i < 8; i++)
	{
		r = r + rawLight[i] * KL[7 - i];
	}
	if (( rawLight[0] < 10 ) && ( rawLight[7] > 10 )) r = r * -1;
	return r;
}

int getRWCenter( tByteArray &rawLight, tFloatArray &KL)
{
	int r = 0 ;
	for(int i = 0; i < 3; i++)
	{
		r = r + ((rawLight[i] * KL[7 - i]) - (rawLight[i + 4] * KL[7 - (i + 4)]));
	}

	return r;
}

void reverse(tByteArray &a)
{
	for(int i = 0; i < 8; i++)
	{
		a[i] = abs(a[i] - 100);
	}
}
