#pragma config(Sensor, S1,     sSonarLeft,     sensorSONAR)
#pragma config(Sensor, S2,     sLight,         sensorI2CCustom9V)
#pragma config(Sensor, S3,     sTouch,         sensorTouch)
#pragma config(Sensor, S4,     sSonarRight,    sensorSONAR)
#pragma config(Motor,  motorA,          mRight,        tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          mShLeft,       tmotorNXT, openLoop, reversed, encoder)
#pragma config(Motor,  motorC,          mLeft,         tmotorNXT, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "mindsensors-lineleader.h"
#include "mindsensors-irdist.h"


#define RELEASE
#define DEBUG

bool IsHereWhite();

task main()
{

	//-----------------------------------------------------
	//motor[mShLeft]=30; // shovel light pressure
	//sleep(20000);
	//return;

	//-----------------------------------------------------
	int wheel 								 =	0 ; // 0 - go ahead, -1 turn left , +1 - turn right, 2 - lost
	const float sonarDistance  =59;
	const int vShovel					 = 25; // shovel speed press
	float sonarLeft 					 = 0;				// current left sonar sensor
	float sonarRight 					 = 0;				// current left sonar sensor
	//float sonarLeftOld 					 = 0;				// previous left sonar sensor
	//float sonarRightOld 					 = 0;				// previous left sonar sensor
	SensorValue(sSonarLeft)	 = 0;				// init left sonar sensor
	SensorValue(sSonarRight) = 0;				// init right sonar sensor
	//-----------------------------------------------------

	//-----------------------------------------------------
	while (SensorValue(sTouch)== 0)
	{
		sleep (10);
	}
	while (SensorValue(sTouch)== 1)
	{
		sleep (10);
	}

	//-----------------------------------------------------
#ifdef RELEASE
	sleep (5000);
#endif

	//-----------------------------------------------------
#ifdef RELEASE
	motor[mShLeft]=100; 				// shovel down
	LLinit(sLight);
	LLwakeUp(sLight);
	for (int i = 10; i <= 130; i = i + 10)
	{
		motor[mLeft] = i/10;					// go ahead smoothly
		motor[mRight]  = i/10;					// go ahead smoothly
		sleep(10);
	}
	motor[mShLeft]=vShovel; // shovel light pressure
#endif

	//-----------------------------------------------------

	int sensor = LLreadResult(sLight);

	// Initit motors
	//-----------------------------------------------------
	float vLeft 	= 20;					// right motor power
	float vRight 	= 20;					// left motor power

#ifdef RELEASE
	motor[mLeft] = vLeft;					// go ahead
	motor[mRight]= vRight;					// go ahead

#endif

	//-----------------------------------------------------

	while (true)
	{
#ifdef RELEASE
		// Get light sensor value and turn when current value less or more than start value
		//-----------------------------------------------------
		if ( IsHereWhite() ) // U turn
		{
			motor[mLeft]   = -100;		  // move back
			motor[mRight]  = -100; 	  	// move back
			motor[mShLeft] = -100;
			sleep(75);
			motor[mShLeft] = 0;
			sleep(200);
			motor[mShLeft] = 100;
			sleep(75);
			motor[mShLeft] = vShovel;
			motor[mLeft]   = -50;		  // turn
			motor[mRight]  =  50;		  // turn
			sleep(350);
		}
#endif
		//-----------------------------------------------------

		// use sonar sensor here
		//-----------------------------------------------------
		sonarLeft  = SensorValue(sSonarLeft);
		sonarRight = SensorValue(sSonarRight);


		if (( sonarLeft  < 5 ) || ( sonarLeft > sonarDistance )) sonarLeft  = sonarDistance + 1; 		// protects from stupid sonar issue when value is 0
			if (( sonarRight < 5 ) || (sonarRight >  sonarDistance )) sonarRight = sonarDistance + 1; // protects from stupid sonar issue when value is 0

#ifdef DEBUG
		displayVariableValues(0,sonarLeft);
		displayVariableValues(1,sonarRight);
#endif

		if ( sonarLeft < sonarDistance ) // see on the left
		{
			if ( sonarRight < sonarDistance ) // see on the right too. Go ahead
			{
				if (( sonarLeft < 24 ) || ( sonarRight < 24 )) // max power go ahead. Kill it
				{
					vLeft = 100 ;
					vRight  = 100 ;

				}
				else  // smart go ahead
				{
					vLeft  = 100 * pow(sonarLeft / sonarRight , 1.1) ;
					vRight = 100 * pow(sonarRight / sonarLeft , 1.1) ;
					//vLeft  = 100 * sonarLeft / sonarRight  ;
					//vRight = 100 * sonarRight / sonarLeft  ;
				}
				wheel = 0;
			}
			else // see only left only. Turn left
			{
				vLeft  = -10  ;
				vRight =  10 ;
				wheel = -1;
			}
		}
		else
		{
			if ( sonarRight < sonarDistance ) // see on the right only. Turn right
			{
				vLeft  =  10 ;
				vRight = -10 ;
				wheel = 1;
			}
			else // victim is lost
			{
				if ( wheel ==0 )
				{
					vLeft  = 100 ;
					vRight = 100 ;
				}
				if (( wheel == 1) || ( wheel == -1 ))
				{
					vLeft  =  vLeft * 1.5 ;
					vRight = vRight * 1.5 ;
				}
				wheel = 2;
			}
		}

#ifdef RELEASE
		// sets motors power value
		motor[mLeft] = vLeft;
		motor[mRight]  = vRight;

#endif

#ifdef DEBUG
		displayVariableValues(3,vLeft);
		displayVariableValues(4,vRight);
#endif

		sleep(10);
		if (SensorValue(sTouch) == 1) break ;
	}
	LLsleep(sLight);
}

// returns true when value of anysensor more than 45
bool IsHereWhite()
{
	tByteArray signalstr;
	LLreadSensorRaw(sLight, signalstr); // read the raw sensor data (8 bit data)

	for (int i = 0 ; i < 8; i=i+1)
	{
		if ( signalstr[i] > 35 ) return true;
	}
	return false;
}
