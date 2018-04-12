#pragma config(Sensor, S1,     sLightRight,    sensorLightActive)
#pragma config(Sensor, S2,     Sonar,          sensorEV3_IRSensor)
#pragma config(Sensor, S3,     sColor,         sensorNone)
#pragma config(Sensor, S4,     sLightLeft,     sensorEV3_Color)
#pragma config(Motor,  motorA,          mLeft,         tmotorEV3_Large, openLoop)
#pragma config(Motor,  motorB,           ,             tmotorEV3_Large, PIDControl, reversed, encoder)
#pragma config(Motor,  motorC,           ,             tmotorEV3_Large, PIDControl, reversed, encoder)
#pragma config(Motor,  motorD,          mRight,        tmotorEV3_Large, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int const vMax  = 70;
int v 					= vMax;
int const vMin  = 0;
int vLeft 			= 0;
int vRight 			= 0;
//---------------------
int getSpeedByDistance ();
void stopOnTheLine(int const blackValue);
void lights(int iGreen);
//---------------------

long redValue;
long greenValue;
long blueValue;

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
	int es 					     = SensorValue(sLightLeft) - SensorValue(sLightRight);
	int e     			     = 0;
	int eOld  			     = e;
	int u     			     = 0;
	int i     			     = 0;
	int iBlack           = 0;
	int const blackValue = 30;
	//----------------------------------
	e  = SensorValue(sLightLeft) - SensorValue(sLightRight) - es;
	eOld = e;
	int iGreen = 0;
	//----------------------------------
	while(true)
	{
		if((SensorValue(sLightLeft) < blackValue) && (SensorValue(sLightRight) < blackValue))
		{
			vLeft = vMax / 2;
			vRight = vMax / 2;
			iBlack++ ;
			if(iBlack >= 2)
			{
				i++ ;

				stopOnTheLine(blackValue);

				if( i % 2 != 0)
				{
					lights(iGreen);
				}

			}
		}
	 	else
		{
			iBlack = 0;
			//-------------------------
			e = SensorValue(sLightLeft) - SensorValue(sLightRight) - es;
			u = (e * 2.5 + (e - eOld) * );
			v = getSpeedByDistance() - abs(u) * 0.30;
			eOld = e;
			//		v = v - abs(u) * 2;
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

void stopOnTheLine(int const blackValue)
{
	while((SensorValue(sLightLeft) < blackValue) && (SensorValue(sLightRight) < blackValue))
	{
		v = getSpeedByDistance();
		vLeft  = v / 1.5;
		vRight = v / 1.5;
		sleep(1);
	}
}

void lights(int iGreen)
{
	setSensorMode(sLightLeft, modeEV3Color_Color);

	iGreen = 0;
	while(true)
	{
		vLeft  =  vLeft / 2;
		vRight =  vRight / 2;
		getColorRGB(sLightLeft, redValue, greenValue, blueValue);
		//if(getColorName(sColor) == colorGreen) break; // HERE IS A PROBLEM. IF WE CATCHE THE GREEN COLOR ROBOT SHOOLD GO AHEAD BUT FRONT HIM HERE IS UNATHERA ROBOT.
		if((greenValue >= 5) && (redValue < 5))
		{
			iGreen++;
			if(iGreen >= 6)
			{
				setSensorMode(sLightLeft, modeEV3Color_Reflected );
				sensorReset(sLightLeft);

				vLeft  =  vMax / 2;
				vRight =  vMax / 2;
				sleep(250);
				break;
			}
		}
		else
		{
			vLeft = 0;
			vRight = 0;
			iGreen = 0;
		}
	}
}
