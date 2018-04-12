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
#include "mindsensors-ev3lights.h"]
// defines
//
#define DEBUG

#define ZONE_A -1
#define ZONE_B  1
//
#define DIST_SHORT      150    // calculates minimum distance for short distance sensor
#define DIST_MIN_IGNORE 80     // minimum of distance
#define INTERVAL_SMALL  40     // sleep interval for release car
#define INTERVAL_LONG   20     // sleep interval for new car
#define IR_COUNT			  2      // repeats quantity
#define SMART_MODE		  1
#define ORD_MODE			  -1
//
// light
const int vMaxLight = 40;
const int vMinLight = 5;
const int degreeA   = 145; //170;
const int degreeB   = 145;
// headers
//
task counterA1();
task counterA2();
task counterB1();
task counterB2();
task Time();
void distRelease(tSensors sensorName);

void Switch(int direction);
int selectZone(int startZone);
void initLight(tSensors sensorName);
void switchLights(int direction);

#ifdef DEBUG
task displayInfo();
#endif

// variables
//
int  qA        = 0;     // queue in the zone A
bool stopLineA = false; // car stops near stop line zone A
int  qB        = 0;     // queue in the zone B
bool stopLineB = false; // car stops near stop line zone B
int countCar   = 0;
int time 			 = 0;
int lightMode  = SMART_MODE;

void initLight(tSensors sensorName)
{
	EV3Lights(sensorName, colorBlue, 0);
	EV3Lights(sensorName, colorRed, 0);
	EV3Lights(sensorName, colorGreen, 0);
}

task main()
{
	initLight(S1);
	initLight(S4);

	int activeZone = selectZone(ZONE_A);
	switchLights(activeZone);
	eraseDisplay();
	displayTextLine(4,"initializing",);

	startTask(counterA1);
	sleep(500);
	startTask(counterA2);
	sleep(500);
	startTask(counterB1);
	sleep(500);
	startTask(counterB2);
	sleep(500);
	startTask(Time);
	eraseDisplay();

#ifdef DEBUG
	startTask(displayInfo);
#endif
	while(true)
	{
		sleep(100);
		// nNxtButtonPressed 0 = Gray Rectangle button. 1 = Right Arrow button. 2 = Left Arrow button. 3 = Orange Square button.
		if (nNxtButtonPressed == 3) // reset counters
		{
			qA        = 0;     // queue in the zone A
			stopLineA = false; // car stops near stop line zone A
			qB        = 0;     // queue in the zone B
			stopLineB = false; // car stops near stop line zone B
			countCar  = 0;
			time    = 0;
		}
		if((nNxtButtonPressed == 1) || (nNxtButtonPressed == 2))
		{
			while((nNxtButtonPressed == 1) || (nNxtButtonPressed == 2)) sleep(10);
			lightMode *= -1;
			countCar  = 0;
			time    = 0;
		}
		if(lightMode == SMART_MODE){
			int lightDirection = qA - qB ;

			if ((lightDirection > 0) && (activeZone != ZONE_A))
			{
				activeZone = ZONE_A;
				Switch(activeZone);
			}
			else if ((lightDirection < 0) && (activeZone != ZONE_B))
			{
				activeZone = ZONE_B;
				Switch(activeZone);
			}
		}
		else{
			if(time % 15 == 0) {
				activeZone = activeZone * -1;
				Switch(activeZone);
				while(time % 15 == 0) sleep(1);
			}
		}
	}
}

void switchLights(int direction)
{
	if (direction == ZONE_B)  	{
		EV3Lights(S4, colorRed, 254);
		EV3Lights(S1, colorGreen, 254);
		EV3Lights(S1, colorRed, 0);
		} else {
		EV3Lights(S1, colorRed, 254);
		EV3Lights(S4, colorGreen, 254);
		EV3Lights(S4, colorRed, 0);
	}
}

void Switch(int direction)
{
	nMotorEncoder[mLightA] = 0;
	nMotorEncoder[mLightB] = 0;

	EV3Lights(S1, colorRed, 254);
	EV3Lights(S1, colorGreen, 0);
	EV3Lights(S4, colorRed, 254);
	EV3Lights(S4, colorGreen, 0);

	while( (abs(nMotorEncoder[mLightA]) < degreeA ) || ( abs(nMotorEncoder[mLightB]) < degreeB ) ){

		if(abs(nMotorEncoder[mLightA]) >= degreeA)
			motor[mLightA] =0;
		else
			motor[mLightA] = direction * (vMaxLight - ((vMaxLight - vMinLight) * (nMotorEncoder[mLightA] / degreeA))) ;

		if(abs(nMotorEncoder[mLightB]) >= degreeB)
			motor[mLightB] =0;
		else
			motor[mLightB] = (-1 * direction) * (vMaxLight - ((vMaxLight - vMinLight) * (nMotorEncoder[mLightB] / degreeB))) ;
	}
	motor[mLightA] =0;
	motor[mLightB] =0;
	switchLights(direction);
}


void distRelease(tSensors sensorName)
{
	const ubyte address = 0x02;
	int d = 0;
	int i = 0;

	while(i < IR_COUNT)
	{
		d = MSDISTreadDist(sensorName, address);
		while ((d > DIST_SHORT) || (d < DIST_MIN_IGNORE))
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sensorName, address);
			i = 0;
		}
		i++;
		sleep(INTERVAL_LONG);
	}
	i = 0;
	while(i < IR_COUNT)
	{
		d = MSDISTreadDist(sensorName, address);
		while (((d <= DIST_SHORT) && (d >= DIST_MIN_IGNORE)) || (d == -1))
		{
			sleep(INTERVAL_LONG);
			d = MSDISTreadDist(sensorName, address);
			i = 0;
		}
		i++;
		sleep(INTERVAL_LONG);
	}
}


task counterA1()
{
	const ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistA1, address);
	while(true)
	{
		distRelease(sDistA1);
		qA++;
	}
}

task counterA2()
{
	const ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistA2, address);
	while(true)
	{
		distRelease(sDistA2);
		if (qA > 0) qA--;
		countCar++;
	}
}

task counterB1()
{
	const ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistB1, address);
	while(true)
	{
		distRelease(sDistB1);
		qB++;
	}
}

task counterB2()
{
	const ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistB2, address);
	while(true)
	{
		distRelease(sDistB2);
		if (qB > 0) qB--;
		countCar++;
	}
}

task displayInfo()
{
	float cpm = 0;
	int lightModeOld = lightMode;
	while(true)
	{
		cpm = ((float)countCar / (float)(time+1));
		if(lightMode != lightModeOld) {
			playSound(soundBeepBeep);
			lightModeOld = lightMode;
			eraseDisplay();
		}
		if(lightMode == SMART_MODE){
			displayTextLine(0,"   SMART_MODE   ");
			displayTextLine(1,"A %d        ",qA);
			displayTextLine(2,"stopA %d   ",stopLineA);
			displayTextLine(3,"B1 %d        ",qB);
			displayTextLine(4,"stopB %d   ",stopLineB);
			displayTextLine(5,"carCount %d", countCar);
			displayTextLine(6,"car/min %.2f", cpm * 60);
			displayTextLine(7,"time %d, sec", time);
		}
		else {
			displayTextLine(0,"   ORD_MODE   ");
			displayTextLine(1,"A %d        ",qA);
			displayTextLine(2,"stopA %d   ",stopLineA);
			displayTextLine(3,"B1 %d        ",qB);
			displayTextLine(4,"stopB %d   ",stopLineB);
			displayTextLine(5,"carCount %d", countCar);
			displayTextLine(6,"car/min %.2f", cpm * 60);
			displayTextLine(7,"time %d, sec", time);
		}
		sleep(100);

	}
}
task Time(){
	while(true){
		time ++;
		sleep(1000);
	}
}

int selectZone(int startZone){
	int z = startZone;
	displayTextLine(4,"select the zone",);
	while(nNxtButtonPressed != 3)
	{
		if((nNxtButtonPressed == 1) || (nNxtButtonPressed == 2))
		{
			while((nNxtButtonPressed == 1) || (nNxtButtonPressed == 2)) sleep(10);

			z *= -1;
			if(z == ZONE_B) displayBigTextLine(4,"zone  B",);
			else displayBigTextLine(4,"zone  A",);
		}
	}
	return z;
}
