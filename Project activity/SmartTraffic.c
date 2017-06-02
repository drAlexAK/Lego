#pragma config(Sensor, S1,     sLightLeft,     sensorLightActive)
#pragma config(Sensor, S2,     Sonar,          sensorEV3_IRSensor)
#pragma config(Sensor, S3,     sLightRight,    sensorLightActive)
#pragma config(Sensor, S4,     sColor,         sensorEV3_Color, modeEV3Color_Color)
#pragma config(Motor,  motorA,           ,             tmotorEV3_Large, openLoop)
#pragma config(Motor,  motorB,          mLeft,         tmotorEV3_Large, PIDControl, reversed, encoder)
#pragma config(Motor,  motorC,          mRight,        tmotorEV3_Large, PIDControl, reversed, encoder)
#pragma config(Motor,  motorD,           ,             tmotorEV3_Large, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int const vMax  = 80;
int const vMin  = 0;
int vLeft 			= 0;
int vRight 			= 0;
//---------------------
int getSpeedByDistance ();
//---------------------
task manageMotors()
{
	while(true)
	{
		if ( vLeft  < vMin ) vLeft  = vMin;
		if ( vRight < vMin ) vRight = vMin;
		if ( vLeft  > vMax ) vLeft  = vMax;
		if ( vRight > vMax ) vRight = vMax;
		motor[mLeft]  = vLeft;
		motor[mRight] = vRight;
		sleep(1);
	}
}
task main()
{
	sleep(2500);
	startTask (manageMotors);
	//	SensorType[sColor] = sensorEV3_Color;
	//SensorMode[sColor] = modeEV3Color_Color;
	//wait1Msec(2000);
	//SensorType[sColor] = sensorNone ;
	//sleep(2000);
	//SensorType[sColor] = sensorEV3_Color;
	//SensorMode[sColor] = modeEV3Color_Color;
	//wait1Msec(2000);
	//SensorType[sColor] = sensorNone ;
	//sleep(2000);

	//----------------------------------
	int es 					= SensorValue(sLightLeft) - SensorValue(sLightRight);
	int e     			= 0;
	int eOld  			= e;
	int u     			= 0;
	int v 					= vMax;
	int i     			= 0;
	//----------------------------------
	e  = SensorValue(sLightLeft) - SensorValue(sLightRight) - es;
	eOld = e;
	//----------------------------------
	while(true)
	{
		if((SensorValue(sLightLeft) < 40) && (SensorValue(sLightRight) < 40))
		{
			i++ ;
			while((SensorValue(sLightLeft) < 40) && (SensorValue(sLightRight) < 40))
			{
				v = getSpeedByDistance();
				vLeft  = v / 2;
				vRight = v / 2;
				sleep(1);
			}
			if( i % 2 != 0)
			{
				while(true)
				{
					if(getColorName(sColor) == colorGreen) break; // HERE IS A PROBLEM. IF WE CATCHE THE GREEN COLOR ROBOT SHOOLD GO AHEAD BUT FRONT HIM HERE IS UNATHERA ROBOT.
					vLeft  = 0;
					vRight = 0;
					sleep(1);
				}
			}
		}
		else
		{
			//-------------------------
			v = getSpeedByDistance();
			e = SensorValue(sLightLeft) - SensorValue(sLightRight) - es;
			u = (e * 1.0 + (e - eOld) * 7) * 2;
			eOld = e;
			v = v - abs(u) * 2;
			vLeft = v - u;
			vRight = v + u;
			//-------------------------
		}
		sleep(1);
	}
}

int getSpeedByDistance ()
{
	int v = 0;
	int	s = SensorValue(Sonar);
	if ( s > 40 )      v = vMax;
	else if ( s > 35 ) v = vMax - 10;
	else if ( s > 30 ) v = vMax - 20;
	else if ( s > 25 ) v = vMax - 30;
	else if ( s > 20 ) v = vMax - 45;
	else if ( s > 15 ) v = vMax - 60;
	else               v = vMin ;
	return v;
}