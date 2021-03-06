#pragma config(Sensor, S1,     sLightLeft,     sensorEV3_Color)
#pragma config(Sensor, S2,     Sonar,          sensorEV3_IRSensor)
#pragma config(Sensor, S3,     sColor,         sensorNone)
#pragma config(Sensor, S4,     sLightRight,    sensorEV3_Color)
#pragma config(Motor,  motorA,          mLeft,         tmotorEV3_Large, PIDControl)
#pragma config(Motor,  motorD,          mRight,        tmotorEV3_Large, PIDControl)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//#define SOUND

int BLACK_LINE_LEFT  = 0;
int BLACK_LINE_RIGHT = 0;

int const vMax  = 75;
int vMin        = 0;
int vLeft 			= 0;
int vRight 			= 0;

//---------------------
int getSpeedByDistance ();
void waitGRcolor();
void findLineLeft();
void findLineRight();
void calibration();
int getES();
//---------------------

float dDyn  = 0;

long redValue;
long greenValue;
long blueValue;

int greenColor1 =0;
int redColor1 =0;
int blueColor1 =0;

int greenValue2 =0;
int blueValue2 =0;
int redValue2 =0;

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
	sleep(30000);
	startTask (manageMotors);
	sleep(500);
	calibration();
	sleep(500);

	//----------------------------------

	float e     			= 0;
	float eOld  			= 0;
	float u     			= 0;
	int v 				    = vMax;
	float in          = 0;
	int iCross     	  = 0;
	int k						  = 0;
	int es 						= getES();

	findLineLeft();
	//findLineRight();
	//----------------------------------
	//----------------------------------
	while(true)
	{
		v = getSpeedByDistance();
		if (v ==0){
			vRight = vLeft  = 0;
			sleep(400);
			continue;
		}
		k = 0;

		while((SensorValue(sLightRight) < BLACK_LINE_RIGHT) && (SensorValue(sLightLeft)< BLACK_LINE_LEFT))
		{
			k++;
			if ((iCross % 2) == 0) vRight = vLeft = v / 2;
			sleep(3);
		}

		if (k >= 2)
		{
			iCross++;
			if ((iCross % 2) != 0)
			{
#ifdef SOUND
	playSound(soundBeepBeep);
#endif
				waitGRcolor();
			}
			else
			{
#ifdef SOUND
	playSound(soundException);
#endif
			}
		}
		else
		{
			//-------------------------
			e = SensorValue(sLightRight) - SensorValue(sLightLeft) - es;
			in = in + e /2500.0;
			if ( fabs(in) > 10 ) in = sgn(in) * 10;
			dDyn = dDyn * 0.9 + (e - eOld) * 0.1;
			u = (e * 0.8 + dDyn * 60) ; // + in;
			v = v - fabs(u) * 0.2;
			eOld = e;
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
	if ( s > 45 )      v = vMax;
	else if ( s > 40 ) v = vMax - 10;
	else if ( s > 35 ) v = vMax - 20;
	else if ( s > 30 ) v = vMax - 30;
	else if ( s > 25 ) v = vMax - 45;
	else if ( s > 20 ) v = vMax - 55;
	else               v = vMin ;
	return v;
}

void waitGRcolor()
{
	int iGreen = 0;

	vRight = vLeft = vMax / 2 ;
	setSensorMode(sLightRight, modeEV3Color_Color);
	getColorRGB(sLightRight, redValue, greenValue, blueValue); // for dummy sensors
	sleep(10);
	while(true){
		getColorRGB(sLightRight, redValue, greenValue, blueValue);
		if ((greenValue >= greenValue2) && (redValue <= redValue2) && (blueValue <= blueValue2))
		{																																 //  R | 57 |  9 |  5
			iGreen++;																											 //  G | 45 | 34 | 15
			if (iGreen >= 3)																							 //  B | 43 | 17 | 10
			{																															 //      wh | up | down
#ifdef SOUND
	playSound(soundBeepBeep);
#endif
				setSensorMode(sLightRight, modeEV3Color_Reflected );
				int k = 0;
				do{
					vRight = vLeft = getSpeedByDistance();
					if (vRight != 0) k++;
					sleep(100);
				} while ((vRight == 0) || (k < 1)); //protects traffic incident on the intersection
					break;
			}
		}
		else
		{
			vRight = vLeft = 0;
			iGreen = 0;
		}
		sleep(10);
	}
	setSensorMode(sLightRight, modeEV3Color_Reflected );
}

void findLineLeft(){
	while((SensorValue(sLightRight) > BLACK_LINE_RIGHT) || (SensorValue(sLightLeft) > BLACK_LINE_LEFT)){
		vRight = vLeft = 40 ;
		sleep(10);
	}

	while(SensorValue(sLightRight) < BLACK_LINE_RIGHT){
		vRight = vLeft = 40; // getSpeedByDistance() / 2;
		sleep(10);
	}
	vMin = -40;
	sleep(10);
	while(SensorValue(sLightRight) > BLACK_LINE_RIGHT){
		vLeft  = -40;
		vRight =  40;
		sleep(10);
	}
	vMin = 0;
	vRight = vLeft = getSpeedByDistance();

}

void findLineRight(){
	while((SensorValue(sLightRight) > BLACK_LINE_RIGHT) || (SensorValue(sLightLeft) > BLACK_LINE_LEFT)){
		vRight = vLeft = getSpeedByDistance() / 1.5;
		sleep(10);
	}

	while(SensorValue(sLightLeft) < BLACK_LINE_RIGHT){
		vRight = vLeft = getSpeedByDistance() / 2;
		sleep(10);
	}
	vMin = -40;
	sleep(10);
	while(SensorValue(sLightLeft) > BLACK_LINE_RIGHT){
		vLeft  = 40;
		vRight = -40;
		sleep(10);
	}
	vMin = 0;
	vRight = vLeft = getSpeedByDistance();

}

void calibration(){

	getColorRGB(sLightRight, redValue, greenValue, blueValue);
	sleep(500);

	getColorRGB(sLightRight, redValue, greenValue, blueValue);
	greenColor1 = greenValue;
	blueColor1 = blueValue;
	redColor1 = redValue;

	playSound(soundBeepBeep);

	setSensorMode(sLightRight, modeEV3Color_Reflected);

	nMotorEncoder[mLeft] = 0;
	nMotorEncoder[mRight] = 0;

	while((nMotorEncoder[mLeft] <= 90) && (nMotorEncoder[mRight] <= 90)) vLeft = vRight = 20;
	//sleep(3000);
	vLeft = vRight =0;
	int tmp = SensorValue(sLightLeft) - SensorValue(sLightRight);
	sleep(100);
	BLACK_LINE_LEFT = SensorValue(sLightLeft) + 10;
	BLACK_LINE_RIGHT = SensorValue(sLightRight) + 10;
	playSound(soundBeepBeep);

	greenValue2 = greenColor1 - 17;
	redValue2 = redColor1 + 2;
	blueValue2 = blueColor1 + 2;

	while((nMotorEncoder[mLeft] <= 180) && (nMotorEncoder[mRight] <= 180)) vLeft = vRight = 20;

	playSound(soundBeepBeep);
	vLeft = vRight =0;
}

int getES(){
	int tmp = SensorValue(sLightRight) - SensorValue(sLightLeft);
	sleep(500);

	int es =0;

	for(int i =0; i< 5; i++ ){
		es += SensorValue(sLightRight) - SensorValue(sLightLeft);
		sleep(100);
	}
	return es/5;
}
