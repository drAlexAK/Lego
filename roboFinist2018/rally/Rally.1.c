#pragma config(Sensor, S1,     sMUX,           sensorEV3_GenericI2C)
#pragma config(Sensor, S2,     sSonarRight,    sensorSONAR)
#pragma config(Sensor, S3,     sSonarLeft,     sensorSONAR)
#pragma config(Sensor, S4,     sSonarFront,    sensorI2CCustomFastSkipStates9V)
#pragma config(Motor,  motorA,          mFront,        tmotorNXT, openLoop, reversed)
#pragma config(Motor,  motorB,          mRear,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mWheel,        tmotorNXT, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

////////////////////////////////////////////////////////////////
//
// rally motors
//
////////////////////////////////////////////////////////////////
// includes
//
#include "mindsensors-ev3smux.h"
#include "mindsensors-irdist.h"

// defines
//
// distances
#define DIST_MAX 						            120
#define DIST_MIN 						            5
// wheel
#define WHEEL_DEGREE_MAX 	 	            80
#define WHEEL_ERROR_DEGREE_MIN_IGNORE 	2
#define WHEEL_SPEED_MAX 		            100
#define WHEEL_SPEED_MIN 		            60
// speed
#define SPEED_MAX 					            100
#define SPEED_MIN 					            75
#define SPEED_BEFORE_STOP_LINE          70
// debug info
//#define DEBUG_DIST
#define LIGHT_BEEP
//#define GO_AHEAD
//#define DEBUG_WHEEL
//#DEBUG_LIGHT
//#define STOP
#define FRONT_DIST_MAX                   40.0
#define FRONT_DIST_MIN			             10.0
// line
#define REFLACTION_WHITE		             18
#define REFLACTION_BLACK		             6
#define COLOR_REPEATER			             2
#define LOST_LINE 											 5
//
#define MAX_I								             5.0
//
// headers
//
// calculates distances and error
//
task dist();
//
// restrics MIN and MAX value of distance
//
int normalyzeDist(int d);
//
// turns the wheel
//
task wheel();
// controls car speed
//
task speed();
//
// restrics MIN and MAX wheel speed value
//
int normalyzeWheelSpeed(int v);
//
// restrics MIN and MAX speed value
//
int normalyzeSpeed(int v);
//
// hhghg
//
int normalyzeDistFront(int d);
//
// test sensor setup
//
bool test();
//
//
//
void waitButton();
// variables
//
// error of distance
int eDist = 0;
// width beetwen walls
int eDistWidth = DIST_MAX;
// error front distance
int distFront = FRONT_DIST_MAX;
int eDistFront = 0;
// error side distance
int eDistSide = 0;
// The SMUX can have up to 3 sensors attached to it. Should be GLOBAL variable
tMSEV3 muxedSensor[3];
// counters black line
int iLine = 0;
// multiplicator depend on front sesnors
float kDistFront = 0.0;
//
bool afterStopLine = false;
bool beforeStopLine = false;


task main()
{
	bool bBlack = false;

	int iBlack = 0;
	int iWhite = 0;
	int iGrey = 0;
	int iLostLine = 0 ;


	//initSensor(&muxedSensor[0], msensor_S1_1, sonarCM);
	//initSensor(&muxedSensor[1], msensor_S1_2, sonarCM);
	//initSensor(&muxedSensor[2], msensor_S1_3, colorReflectedLight);
	initSensor(&muxedSensor[0], msensor_S1_1, colorReflectedLight);

	if (test() == false)
	{
		motor[mRear] = -30;
		motor[mFront] = -30;

		playSound(soundBeepBeep);

		sleep(500);

		playSound(soundBeepBeep);

		motor[mFront] = 0;
		motor[mRear] = 0;

		return;
	}
	waitButton();

	startTask(dist);
#ifndef GO_AHEAD
	startTask(wheel);
#endif
	startTask(speed);

	int sLight = 0;

	// light ---
	while(true)
	{
		readSensor(&muxedSensor[0]);
		sLight = muxedSensor[0].light;

		if (sLight > REFLACTION_WHITE)
		{
			iWhite++;
			iLostLine++;
		}
		else if (sLight < REFLACTION_BLACK)
		{
			iBlack++;
		}
		else
		{
			iGrey++;
			iLostLine++;
		}

		if ( iBlack > COLOR_REPEATER )
		{
			if (beforeStopLine)
			{
				iLine = 0;
				afterStopLine = true;
				playSound(soundException);
				sleep(400);
				afterStopLine = false;
				beforeStopLine = false;
				break; // exit here. stops analyze light sensor after stop line
			}
			bBlack = true;
			iWhite = 0;
			iBlack = 0;
			iGrey = 0;
		}

		if ((iWhite > COLOR_REPEATER) || (iGrey > COLOR_REPEATER))
		{
			if(bBlack == true)
			{
				iLine++;
				bBlack = false;
				iLostLine = 0;
#ifdef LIGHT_BEEP
				playSound(soundBeepBeep);
#endif
			}
			iBlack = 0;
			iGrey = 0;
			iWhite = 0;
			if (iLostLine > LOST_LINE)
			{
				iLostLine = 0;
				if ((iLine !=0) && (iLine < 4))
				{
					iLine = 0;
					playSound(soundException);
				}
			}
		}

#ifdef DEBUG_LIGHT
		displayTextLine(2, "%d", iLine);
		displayTextLine(4, "%d", sLight);
#endif
		if (iLine == 4) beforeStopLine = true;
		sleep (10);
	}

	while(true)
	{
		sleep(500);
	}

}


// controls car speed
//
task speed()
{
	int speed = 0;

	while(true)
	{
		speed = SPEED_MAX - (((abs(eDist) * 100 / (eDistWidth )) * SPEED_MAX / 300) * kDistFront);
		speed = normalyzeSpeed(speed);
		motor[mFront] = speed;
		motor[mRear]  = speed;
		sleep(25);
	}
}

// turns the wheel
//

task wheel()
{
	//int initWheelEncoder = nMotorEncoder[mWheel];
	nMotorEncoder[mWheel] = 0;

	float wheelDegree  = 0.0;
	int wheelDegreeRatio  = 0;
	int wheelDegreeRatioOld = 0;
	int eWheelDegree = 0;
	int mWheelSpeed  = 0;

	float i = 0.0;

	while (true)
	{
		wheelDegreeRatio = ((eDist * 100 / (eDistWidth  )) * WHEEL_DEGREE_MAX) /100 ;
		i = i + eDist * 0.001;
		if (fabs(i) > MAX_I) i = sgn (i) * MAX_I;
		wheelDegree = (wheelDegreeRatio + (wheelDegreeRatio - wheelDegreeRatioOld) * 5) * kDistFront + i ;
		wheelDegreeRatioOld = wheelDegreeRatio;

		if (fabs(wheelDegree) >  WHEEL_DEGREE_MAX ) wheelDegree = sgn(wheelDegree) * WHEEL_DEGREE_MAX; // restrics WHEEL MAX DEGREE
			if ( kDistFront >= FRONT_DIST_MAX )
		{
			if (abs(eDist) < 10) wheelDegree = wheelDegree / 5.0;
			else if (abs(eDist) < 20) wheelDegree = wheelDegree / 3.5;
			else if (abs(eDist) < 30) wheelDegree = wheelDegree / 2.5;
			else if (abs(eDist) < 40) wheelDegree = wheelDegree / 1.75;
			else if (abs(eDist) < 50) wheelDegree = wheelDegree / 1.25;
			else if (abs(eDist) > 80) wheelDegree = wheelDegree * 1.25;
		}

		eWheelDegree = wheelDegree -  nMotorEncoder[mWheel]  ; // calculates error of wheel, degree

		mWheelSpeed = (( eWheelDegree * 100 / WHEEL_DEGREE_MAX) * WHEEL_SPEED_MAX) / 100 * kDistFront ; // calculates while speed
		mWheelSpeed = normalyzeWheelSpeed(mWheelSpeed);
#ifndef DEBUG_WHEEL
		motor[mWheel]  = mWheelSpeed;
#endif

#ifdef DEBUG_WHEEL
		displayTextLine(0, "eDist:%d", eDist);
		displayTextLine(1, "wd :%d", wheelDegree);
		displayTextLine(2, "ewd:%d", eWheelDegree);
		displayTextLine(3, "speed :%d", mWheelSpeed);

#endif

		sleep(25);
	}

}

//
// calculates distances and error
//
task dist()
{
	int dLeftFront 		= 0;
	int dRightFront 	= 0;
	int dLeft 				= 0;
	int dRight 				= 0;
	int eDistOld      = 0;
	// initialyze IR sensors
	ubyte address = 0x02;
	string type = MSDISTreadModuleType(sSonarFront, address);

	while ( true)
	{
		//distFront = normalyzeDistFront (SensorValue(sSonarFront));
		distFront = (normalyzeDistFront (MSDISTreadDist(sSonarFront, address) / 10) );

		// front distance
		kDistFront = (((FRONT_DIST_MAX - FRONT_DIST_MIN) * 100) / (distFront * 100)) * 1.5;

		//readSensor(&muxedSensor[0]);
		//dLeftFront = muxedSensor[0].distance / 10 ; // returns cm

		//readSensor(&muxedSensor[1]);
		//dRightFront = muxedSensor[1].distance / 10 ; // returns cm

		dLeft  = SensorValue[sSonarLeft];
		dRight = SensorValue[sSonarRight];

		//dLeftFront	= normalyzeDist(dLeftFront);
		//dRightFront	= normalyzeDist(dRightFront);
		dLeft 			= normalyzeDist(dLeft);
		dRight 			= normalyzeDist(dRight);

		eDist = dLeft - dRight; //((2 * dLeft  + 3 * dLeftFront) - (2 * dRight  + 3 * dRightFront)) / 5;
		eDistWidth  = dLeft + dRight;  //((2 * dLeft  + 3 * dLeftFront) + (2 * dRight  + 3 * dRightFront)) / 5;
		if (eDistWidth ==0) eDistWidth = DIST_MAX * 2;

		//eDistFront = dLeftFront - dRightFront;
		eDistSide  = dLeft - dRight;

		eDistOld = eDist;

#ifdef DEBUG_DIST
		displayTextLine(0, "LF: %d", dLeftFront);
		displayTextLine(1, "L :%d", dLeft);
		displayTextLine(2, "RF:%d", dRightFront);
		displayTextLine(3, "R :%d", dRight);
		displayTextLine(4, "F :%d", distFront);
		displayTextLine(5, "eDist:%d", eDist);
#endif
		sleep(25);

	}
}

//
// restrics MIN and MAX value of distance
//
int normalyzeDistFront(int d)
{
	if (d > FRONT_DIST_MAX) return FRONT_DIST_MAX;
	if (d < FRONT_DIST_MIN) return FRONT_DIST_MIN;
	return d;
}

int normalyzeDist(int d)
{
	if (d < DIST_MIN) return DIST_MIN;
	if (d > DIST_MAX) return DIST_MAX;
	return d;
}

//
// restrics MIN and MAX wheel speed value
//
int normalyzeWheelSpeed(int v)
{
	// restrics WHEEL MIN SPEED
	if (abs(v) < WHEEL_SPEED_MIN) return sgn(v) * WHEEL_SPEED_MIN;
	// restrict WHEEL MAX SPEED
	if (abs(v) > WHEEL_SPEED_MAX) return sgn(v) * WHEEL_SPEED_MAX;
	return v;
}

//
// restrics MIN and MAX speed value
//
int normalyzeSpeed(int v)
{

#ifdef STOP
	return  0;
#endif
	if (afterStopLine) return 0;
	if (beforeStopLine) return SPEED_BEFORE_STOP_LINE;


	// restrics MIN SPEED
	if (v < SPEED_MIN) return SPEED_MIN;
	// restrict MAX SPEED
	if (v > SPEED_MAX) return SPEED_MAX;
	return v;
}

void waitButton()
{
	playSound(soundBeepBeep);
	while(true)
	{
		if (nNxtButtonPressed == 3)
		{
			playSound(soundBeepBeep);
			while (nNxtButtonPressed == 3)
			{
				sleep(10);
			}
			break;
		}
		sleep(10);
	}
}

bool test()
{
	int testsLight = 0;
	int dist = 0;
	int testdLeftFront =0;
	int testdRightFront =0;

	ubyte address = 0x02;
	string type = MSDISTreadModuleType(sSonarFront, address);


	for(int i = 0; i < 2; i++)
	{
		//dist = MSDISTreadDist(sSonarFront, address);

		if(((SensorValue(sSonarLeft) == 0) | (SensorValue(sSonarLeft) == 255)) && i == 1) return false;
		if(((SensorValue(sSonarRight) == 0) | (SensorValue(sSonarRight) == 255)) && i == 1) return false;

		//if(((dist < 100) | (dist > 800)) && i == 1) return false;

		readSensor(&muxedSensor[0]);
		testsLight = muxedSensor[0].light;
		sleep(50);
		if(((testsLight <= 0) | (testsLight >= 255)) && i == 1) return false;
		sleep(200);
	}
	return true;
}
