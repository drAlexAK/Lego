#pragma config(Sensor, S4,     sLightLeft,     sensorI2CCustom9V)
#pragma config(Sensor, S1,     sLightRight,    sensorI2CCustom9V)
#pragma config(Sensor, S3,     sTouch,         sensorTouch)
#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "mindsensors-lineleader.h"

// time: 	    08:30 - 08:70 fast/stable
// direction: left
// motors: 	  2
// ger: 	    direct
// tire:      \__/ #61481 + #56145c04

#define RELEASE
//#define DEBUG

int getRWRight();
int getRWLeft();
void calibrate ();
void waitTouchRelease();
//-------------------
float const KL0 = 1.00;
float const KL1 = 1.00;
float const KL2 = 1.00;
float const KL3 = 1.00;
float const KL4 = 1.00;
float const KL5 = 1.00;
float const KL6 = 1.00;
float const KL7 = 1.00;
//--------------------
int vBase 			= 95;
int const vMax  = 100;
int const vMin	= 10;
int const maxI	= 10;
float const k 	= 35;
int iAlert			= 0;
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
	_lineLeader_cmd(sLightLeft, 'E'); // European frequency compensation
	_lineLeader_cmd(sLightRight, 'E'); // European frequency compensation

	calibrate();
	waitTouchRelease();
	startTask(speedUp);

	int e       		= 0;
	int eOld 				= e;
	int rwLeft  		= getRWLeft();
	int rwRight 		= getRWRight();
	int es 					= rwLeft - rwRight;
	float i 				= 0;
	int 	v 				= 0 ;
	int   u					= 0 ;
	int vLeft 			= 0;
	int vRight 			= 0;
	long iSpeed     = 0;
	leftAlert  			= false;
	rightAlert 			= false;
	tByteArray rawLightLeft;
	tByteArray rawLightRight;

	while(true)
	{
		//------------
		LLreadSensorRaw(sLightLeft, rawLightLeft);

		rwLeft=
		(
		rawLightLeft[0] * KL0 +
		rawLightLeft[1] * KL1 +
		rawLightLeft[2] * KL2 +
		rawLightLeft[3] * KL3 +
		rawLightLeft[4] * KL4 +
		rawLightLeft[5] * KL5 +
		rawLightLeft[6] * KL6 +
		rawLightLeft[7] * KL7
		);
		if ((rawLightLeft[7] < 10) && (rawLightLeft[0] > 10)) rwLeft = rwLeft * -1;
		//-----------------
		LLreadSensorRaw(sLightRight, rawLightRight);

		rwRight =
		(
		rawLightRight[0] * KL7 +
		rawLightRight[1] * KL6 +
		rawLightRight[2] * KL5 +
		rawLightRight[3] * KL4 +
		rawLightRight[4] * KL3 +
		rawLightRight[5] * KL2 +
		rawLightRight[6] * KL1 +
		rawLightRight[7] * KL0
		);
		if ((rawLightRight[0] < 10) && (rawLightRight[7] > 10)) rwRight = rwRight  * -1;
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
			vLeft  = vMin;
			vRight = 90 ;
			iAlert ++;
		}
		else if (rightAlert)
		{
			vLeft  = 90;
			vRight = vMin;
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
		if (SensorValue(sTouch) == 1) break;
	}
	LLsleep(sLightLeft);  // Sleep to conserve power when not in use
	LLsleep(sLightRight); // Sleep to conserve power when not in use
}

//---------------------------------

void calibrate ()
{
	displayBigTextLine(4, "  WHITE");

	while (SensorValue(sTouch) == 0)
	{
		sleep (10);
	}

	while (SensorValue(sTouch) == 1)
	{
		sleep (10);
	}
	LLcalWhite(sLightLeft);
	LLcalWhite(sLightRight);

	playSound(soundBlip);

	displayBigTextLine(4, "   Done");

	sleep(1000);

	displayBigTextLine(4, "  BLACK");

	while (SensorValue(sTouch)== 0)
	{
		sleep (10);
	}
	while (SensorValue(sTouch)== 1)
	{
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

	while (SensorValue(sTouch)== 0)
	{
		sleep (10);
	}
	eraseDisplay();
	while (SensorValue(sTouch)== 1)
	{
		sleep (10);
	}

}
//---------------
int getRWLeft()
{
	tByteArray rawLight;
	LLreadSensorRaw(sLightLeft, rawLight);
	int r =
	(
	rawLight[0]  * KL0 +
	rawLight[1]  * KL1 +
	rawLight[2]  * KL2 +
	rawLight[3]  * KL3 +
	rawLight[4]  * KL4 +
	rawLight[5]  * KL5 +
	rawLight[6]  * KL6 +
	rawLight[7]  * KL7
	) ;
	if (( rawLight[7] < 10 ) && ( rawLight[0] > 10 )) r = r * -1;
	return r;
}
//--------------------
int getRWRight()
{
	tByteArray rawLight;
	LLreadSensorRaw(sLightRight, rawLight);
	int r =
	(
	rawLight[0] * KL7 +
	rawLight[1] * KL6 +
	rawLight[2] * KL5 +
	rawLight[3] * KL4 +
	rawLight[4] * KL3 +
	rawLight[5] * KL2 +
	rawLight[6] * KL1 +
	rawLight[7] * KL0
	) ;
	if (( rawLight[0] < 10 ) && ( rawLight[7] > 10 )) r = r * -1;
	return r;
}
