#pragma config(Sensor, S1,     sLightRight,    sensorEV3_Color)
#pragma config(Sensor, S2,     Sonar,          sensorEV3_IRSensor)
#pragma config(Sensor, S3,     sColor,         sensorNone)
#pragma config(Sensor, S4,     sLightLeft,     sensorEV3_Color)
#pragma config(Motor,  motorA,          mLeft,         tmotorEV3_Large, openLoop)
#pragma config(Motor,  motorB,           ,             tmotorEV3_Large, PIDControl, reversed, encoder)
#pragma config(Motor,  motorC,           ,             tmotorEV3_Large, PIDControl, reversed, encoder)
#pragma config(Motor,  motorD,          mRight,        tmotorEV3_Large, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define BLACK_LINE 10

int const vMax  = 80;
int const vMin  = 0;
int vLeft 			= 0;
int vRight 			= 0;

//---------------------
int getSpeedByDistance ();
void waitGRcolor();
void findLine();
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

	int e     			= 0;
	int eOld  			= 0;
	int u     			= 0;
	int v 					= vMax;
	int i     			= 0;
	int k						= 0;
	int es 					= SensorValue(sLightLeft) - SensorValue(sLightRight);

	startTask (manageMotors);
	sleep(2500);
	findLine();
	//----------------------------------
	//----------------------------------
	while(true)
	{
		v = getSpeedByDistance();
		if(v ==0){
			vLeft = 0;
			vRight =0;
			sleep(400);
			continue;
		}
		k = 0;

		while((SensorValue(sLightLeft) < BLACK_LINE) && (SensorValue(sLightRight) < BLACK_LINE))
		{
			k++;
			vLeft  = v / 2;
			vRight = v / 2;
			sleep(1);
		}

		if (k >= 2)
		{
			i++;
			if( i % 2 != 0)
			{
				vLeft  = 0; //vLeft / 2;
				vRight = 0; //vRight / 2;
				waitGRcolor();
			}
			else
				playSound(soundException);
		}
		else
		{
			//-------------------------
			e = SensorValue(sLightLeft) - SensorValue(sLightRight) - es;
			u = (e * 1.5  + (e - eOld) * 10) * 0.7;
			v = v - abs(u) * 0.8;
			eOld = e;
			vLeft = v - u;
			vRight = v + u;
			displayBigTextLine(3,"e:%d u:%d v:%d",e,u,v);
			displayBigTextLine(5,"%d %d",SensorValue[sLightLeft],SensorValue[sLightRight]);
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

void waitGRcolor()
{

	int iGreen = 0;
	setSensorMode(sLightLeft, modeEV3Color_Color);
	while(true){

		vLeft = 0;
		vRight = 0;

		getColorRGB(sLightLeft, redValue, greenValue, blueValue);
		//if(getColorName(sColor) == colorGreen) break; // HERE IS A PROBLEM. IF WE CATCHE THE GREEN COLOR ROBOT SHOOLD GO AHEAD BUT FRONT HIM HERE IS UNATHERA ROBOT.
		if ((greenValue >= 12) && (redValue <= 10) && (blueValue <= 10)) //&& (redValue < 5))
		{
			iGreen++;
			if(iGreen >= 6)
			{
				playSound(soundBeepBeep);
				setSensorMode(sLightLeft, modeEV3Color_Reflected );
				do{
				vRight = vLeft  =  getSpeedByDistance();
				sleep(10);
				}while(vRight == 0); //protects traffic incident on the intersection
				sleep(400);
				break;
			}
		}
		else
		{
			vLeft = 0;
			vRight = 0;
			iGreen = 0;
		}
		sleep(10);
	}
}

void findLine(){
	while((SensorValue(sLightLeft) > BLACK_LINE) || (SensorValue(sLightRight) > BLACK_LINE)){
		vRight = vLeft = getSpeedByDistance();
		sleep(10);
	}

	while(SensorValue(sLightRight) < BLACK_LINE){
		vRight = vLeft = getSpeedByDistance() /3;
		sleep(10);
	}

	sleep(10);
	while(SensorValue(sLightRight) > BLACK_LINE){
		vLeft =  0;
		vRight = 80;
		sleep(10);
	}

	vRight = vLeft = getSpeedByDistance();

}
