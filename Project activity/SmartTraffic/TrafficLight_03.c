#pragma config(Sensor, S1,     sDistB1,        sensorI2CCustomFastSkipStates9V)
#pragma config(Sensor, S2,     sDistB2,        sensorI2CCustomFastSkipStates9V)
#pragma config(Sensor, S3,     sDistA2,        sensorI2CCustomFastSkipStates9V)
#pragma config(Sensor, S4,     sDistA1,        sensorI2CCustomFastSkipStates9V)
#pragma config(Motor,  motorA,          mLightA,       tmotorNXT, PIDControl, reversed, encoder)
#pragma config(Motor,  motorB,          mLightB,       tmotorNXT, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// includes
//
#include "mindsensors-irdist.h"

// defines
//
#define DEBUG

#define ZONE_A 0
#define ZONE_B 1
//
#define DIST_SHORT     150 		// calculates minimum distance for short distance sensor
#define INTERVAL_SMALL 10     // sleep interval for release car
#define INTERVAL_LONG  20     // sleep interval for new car
//
// light
#define LIGHT_DEGREE_MAX  			68
#define LIGHT_DEGREE_MIN  			0
#define LIGHT_SPEED_ROTATION   	30
// headers
//
task counterA1();
task counterA2();
task counterB1();
task counterB2();
void Switch(int zone);

#ifdef DEBUG
task displayInfo();
#endif

// variables
//
int  qA        = 0;     // queue in the zone A
bool stopLineA = false; // car stops near stop line zone A
int  qB        = 0;     // queue in the zone B
bool stopLineB = false; // car stops near stop line zone B


task main()
{

	nMotorEncoder[mLightA] = LIGHT_DEGREE_MIN  ; // resets encoder to half
	nMotorEncoder[mLightB] = LIGHT_DEGREE_MIN  ; // resets encoder to half

	int activeZone = ZONE_A;

	while( nMotorEncoder[mLightB] < LIGHT_DEGREE_MAX)
	{
		motor[mLightB] = LIGHT_SPEED_ROTATION - LIGHT_SPEED_ROTATION / 2 * nMotorEncoder[mLightB] / LIGHT_DEGREE_MAX;
	}

	motor[mLightB] = 0;


	startTask(counterA1);
	startTask(counterA2);
	startTask(counterB1);
	startTask(counterB2);

#ifdef DEBUG
	startTask(displayInfo);
	int i = 0;
#endif
	while(true)
	{
		sleep(100);
		// nNxtButtonPressed 0 = Gray Rectangle button. 1 = Right Arrow button. 2 = Left Arrow button. 3 = Orange Square button.
		if (nNxtButtonPressed == 3) // reset counters
		{
			qA        = 0;     // queue in the zone A
			stopLineA = false; // car stops near stop line zone A7
			qB        = 0;     // queue in the zone B
			stopLineB = false; // car stops near stop line zone B
		}
		int lightDirection = qA - qB ;

		if ((lightDirection > 0) && (activeZone != ZONE_A))
		{
			activeZone = ZONE_A;
			Switch(ZONE_A);
		}
		else if ((lightDirection < 0) && (activeZone != ZONE_B))
		{
			activeZone = ZONE_B;
			Switch(ZONE_B);
		}


	}
}


void Switch(int zone)
{
	int ea = nMotorEncoder[mLightA];
	int eb = nMotorEncoder[mLightB];
	if (zone == ZONE_A )
	{
		while(nMotorEncoder[mLightA] > LIGHT_DEGREE_MIN || nMotorEncoder[mLightB] < LIGHT_DEGREE_MAX)
		{
			//motor[mLightB] = -LIGHT_SPEED_ROTATION - LIGHT_SPEED_ROTATION / 2 * nMotorEncoder[mLightB] / LIGHT_DEGREE_MAX;
			//motor[mLightA] = -LIGHT_SPEED_ROTATION - LIGHT_SPEED_ROTATION / 2 * nMotorEncoder[mLightA] / LIGHT_DEGREE_MAX;
			if (nMotorEncoder[mLightA] > LIGHT_DEGREE_MIN) motor[mLightA] = -LIGHT_SPEED_ROTATION + LIGHT_SPEED_ROTATION / 2 * (LIGHT_DEGREE_MAX - nMotorEncoder[mLightA]) / LIGHT_DEGREE_MAX;
			if (nMotorEncoder[mLightB] < LIGHT_DEGREE_MAX) motor[mLightB] = LIGHT_SPEED_ROTATION - LIGHT_SPEED_ROTATION / 2 * nMotorEncoder[mLightB] / LIGHT_DEGREE_MAX;
		}
	}
	else if (zone == ZONE_B )
	{
		while(nMotorEncoder[mLightA] < LIGHT_DEGREE_MAX  || nMotorEncoder[mLightB] > LIGHT_DEGREE_MIN)
		{
			//motor[mLightB] = LIGHT_SPEED_ROTATION - LIGHT_SPEED_ROTATION / 2 * nMotorEncoder[mLightB] / LIGHT_DEGREE_MAX;
			if (nMotorEncoder[mLightB] > LIGHT_DEGREE_MIN)  motor[mLightB] = -LIGHT_SPEED_ROTATION + LIGHT_SPEED_ROTATION / 2 * (LIGHT_DEGREE_MAX - nMotorEncoder[mLightB]) / LIGHT_DEGREE_MAX;
			if (nMotorEncoder[mLightA] < LIGHT_DEGREE_MAX)  motor[mLightA] = LIGHT_SPEED_ROTATION - LIGHT_SPEED_ROTATION / 2 * nMotorEncoder[mLightA] / LIGHT_DEGREE_MAX;
		}
	}
	motor[mLightA] = 0;
	motor[mLightB] = 0;
}


task counterA1()
{
	int d = 0 ;
	ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistA1, address);
	while(true)
	{
		do
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sDistA1, address);
		} while (d  >= DIST_SHORT  | d == -1);

		do
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sDistA1, address);
		} while (d  < DIST_SHORT  | d == -1);

		qA++;
	}
}

task counterA2()
{
	int d = 0 ;
	ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistA2, address);
	while(true)
	{
		do
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sDistA2, address);
		} while (d  >= DIST_SHORT  | d == -1);

		do
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sDistA2, address);
		} while (d  < DIST_SHORT  | d == -1);

		qA--;
	}
}

task counterB1()
{
	int d = 0 ;
	ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistB1, address);
	while(true)
	{
		do
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sDistB1, address);
		} while (d  >= DIST_SHORT  | d == -1);

		do
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sDistB1, address);
		} while (d  < DIST_SHORT  | d == -1);

		qB++;
	}
}

task counterB2()
{
	int d = 0 ;
	ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistB2, address);
	while(true)
	{
		do
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sDistB2, address);
		} while (d  >= DIST_SHORT  | d == -1);

		do
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sDistB2, address);
		} while (d  < DIST_SHORT  | d == -1);
		qB--;
	}
}

task displayInfo()
{
	while(true)
	{
		displayTextLine(0,"A %d        ",qA);
		displayTextLine(1,"stopA %d   ",stopLineA);
		displayTextLine(2,"B1 %d        ",qB);
		displayTextLine(3,"stopB %d   ",stopLineB);
		sleep(100);
	}
}
