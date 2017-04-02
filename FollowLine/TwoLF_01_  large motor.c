#pragma config(Sensor, S1,     sLightRight,    sensorI2CCustom9V)
#pragma config(Sensor, S3,     sTouch,         sensorTouch)
#pragma config(Sensor, S4,     sLightLeft,     sensorI2CCustom9V)
#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop, reversed)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "mindsensors-lineleader.h"

#define RELEASE//
//#define DEBUG//

task main()
{


//float batteryLife = getBatteryVoltage();


	LLinit(sLightLeft); 	// Set up Line Leader sensor type
	LLinit(sLightRight); 	// Set up Line Leader sensor type

	while (SensorValue(sTouch)== 0)
	{
		sleep (10);
	}

	LLwakeUp(sLightLeft); 	// Wake sensor from sleep mode
	LLcalWhite(sLightLeft); 	// Set white threshold for light area
	LLwakeUp(sLightRight); 	// Wake sensor from sleep mode
	LLcalWhite(sLightRight); 	// Set white threshold for light area

	while (SensorValue(sTouch)== 1)
	{
		sleep (10);
	}

	int const vMax = 100;
	float es = 0 ;
	float  e = 0;
	float eOld = e;


	tByteArray rawLight;

	LLreadSensorRaw(sLightLeft, rawLight); // read the raw sensor data (8 bit data)
	float	rwLeft = (rawLight[0] * 1.7 + rawLight[1] * 1.6 + rawLight[2] * 1.5 + rawLight[3] * 1.4 + rawLight[4] * 1.3 + rawLight[5] * 1.2 + rawLight[6]  * 1.1 + rawLight[7]) / 30;
	LLreadSensorRaw(sLightRight, rawLight); // read the raw sensor data (8 bit data)
	float	rwRight = (rawLight[0] + rawLight[1] * 1.1 + rawLight[2] * 1.2 + rawLight[3] * 1.3  + rawLight[4] * 1.4 + rawLight[5] * 1.5  + rawLight[6] * 1.6 + rawLight[7] * 1.7) / 30;
	es = rwLeft - rwRight;
 int v = 0 ;
	while(true)
	{

		LLreadSensorRaw(sLightLeft, rawLight); // read the raw sensor data (8 bit data)
		rwLeft = rwLeft = (rawLight[0] * 1.7 + rawLight[1] * 1.6 + rawLight[2] * 1.5 + rawLight[3] * 1.4 + rawLight[4] * 1.3 + rawLight[5] * 1.2 + rawLight[6] * 1.1 + rawLight[7]) /30;
		LLreadSensorRaw(sLightRight, rawLight); // read the raw sensor data (8 bit data)
		rwRight = (rawLight[0] + rawLight[1] * 1.1 + rawLight[2] * 1.2 + rawLight[3] * 1.3  + rawLight[4] * 1.4 + rawLight[5] * 1.5  + rawLight[6] * 1.6 + rawLight[7] * 1.7) /30;

		e = rwLeft - rwRight - es;

		int	u = e  + (e - eOld) * 5 ;

		eOld = e ;

    v = vMax - abs (u) ;

#ifdef RELEASE
		motor[mLeft] = v + u;
		motor[mRight] = v - u;
#endif
#ifdef DEBUG
		displayVariableValues(0,rwLeft);
		displayVariableValues(1,rwRight);
		displayVariableValues(3,u);
		displayVariableValues(4,motor[mLeft]);
		displayVariableValues(5,motor[mRight]);
#endif

		//	if (mLeft < 0) mLeft = 0.0;
		//if (mLeft < 0) mLeft = 0.0;

		sleep (1);
		if (SensorValue(sTouch)== 1) break;
	}
	LLsleep(sLightLeft); // Sleep to conserve power when not in use
	LLsleep(sLightRight); // Sleep to conserve power when not in use
}
