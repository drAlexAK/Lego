#pragma config(Sensor, S1,     sMUX,           sensorEV3_GenericI2C)
#pragma config(Sensor, S2,     sSonarLeft,     sensorSONAR)
#pragma config(Sensor, S3,     sSonarFront,    sensorSONAR)
#pragma config(Sensor, S4,     sSonarRight,    sensorSONAR)
#pragma config(Motor,  motorA,          mLeft,         tmotorNXT, openLoop, reversed, encoder)
#pragma config(Motor,  motorB,          mRight,        tmotorNXT, openLoop, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "mindsensors-ev3smux.h"

#define RELEASE
#define DEBUG

task main()
{

	tMSEV3 muxedSensor[3];

	initSensor(&muxedSensor[0], msensor_S1_1, colorReflectedLight);
	//initSensor(&muxedSensor[1], msensor_S1_2, colorReflectedLight);
	initSensor(&muxedSensor[2], msensor_S1_3, touchStateBump);

	int const maxDist 	= 50;
	int const maxLight  = 35;
	int vLeft 					= 100;
	int vRight 					= 100;

	playSound(soundBeepBeep);

	do
	{
		sleep(100);
		readSensor(&muxedSensor[2]);
	}
	while(muxedSensor[2].touch==0);
	playSound(soundBlip);

	do
	{
		sleep(100);
		readSensor(&muxedSensor[2]);
	}
	while(muxedSensor[2].touch==1);
	playSound(soundBlip);

	sleep(5000);

	while (true)
	{

		readSensor(&muxedSensor[0]);
		int sLightLeft = muxedSensor[0].light;

		readSensor(&muxedSensor[1]);
		int sLightRight = muxedSensor[1].light;

		if (( sLightLeft > maxLight ) || ( sLightRight > maxLight)) // go back
		{
			if (( sLightLeft > maxLight ) && ( sLightRight > maxLight))
			{
				vLeft  = -100;
				vRight = -100;
			}
			else if ( sLightLeft > maxLight )
			{
				vLeft  = -100;
				vRight = -75;
			}
			else if ( sLightRight > maxLight )
			{
				vLeft  = -75;
				vRight = -100;
			}

			motor[mLeft]  = vLeft;
			motor[mRight] = vRight;
			sleep(350);
			motor[mLeft]  = -50;
			motor[mRight] = 50;
			sleep(350);
			vLeft  = 100;
			vRight = 100;
		}

		int sLeft = SensorValue(sSonarLeft);
		if (( sLeft < 3 )  || ( sLeft > maxDist))  sLeft  = 0;

		int sRight = SensorValue(sSonarRight);
		if (( sRight < 3 ) || ( sRight > maxDist)) sRight = 0;

		int sFront = SensorValue(sSonarFront);
		if (( sFront < 3 ) || ( sFront > maxDist)) sFront = 0;

		if (( sLeft > 0 ) && ( sFront > 0 ) && ( sRight > 0 ))
		{
			vLeft 	= 100;
			vRight 	= 100;
		}
		else if (( sLeft > 0 ) && ( sFront > 0 ))
		{
			vLeft 	= 50;
			vRight 	= 100;
		}
		else if (( sRight > 0 ) && ( sFront > 0 ))
		{
			vLeft 	= 100;
			vRight 	= 50;
		}
		else if ( sLeft > 0 )
		{
			vLeft 	= -50;
			vRight 	=  50;
		}
		else if ( sRight > 0 )
		{
			vLeft 	=  50;
			vRight 	= -50;
		}
		else if ( sFront > 0 )
		{
			vLeft 	= 100;
			vRight 	= 100;
		}
		else
		{
			// nothing to do
		}

#ifdef DEBUG
		displayTextLine(0, "left  :%d", sLeft);
		displayTextLine(1, "front :%d", sFront);
		displayTextLine(2, "right :%d", sRight);

		displayTextLine(3, "vLeft :%d", vLeft);
		displayTextLine(4, "vRight :%d", vRight);
#endif
#ifdef RELEASE
		motor[mLeft]  = vLeft;
		motor[mRight] = vRight;
#endif
		sleep(100);

		readSensor(&muxedSensor[2]);
		if (muxedSensor[2].touch == 1) break;
	}

}
