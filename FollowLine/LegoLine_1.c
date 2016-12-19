                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         #pragma config(Sensor, S1,     sLightLeft,     sensorLightActive)
#pragma config(Sensor, S2,     sLightCenter,   sensorLightActive)
#pragma config(Sensor, S3,     sLightRight,    sensorLightActive)
#pragma config(Sensor, S4,     sTouch,         sensorTouch)
#pragma config(Motor,  motorA,           ,             tmotorNXT, openLoop)
#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, PIDControl, reversed, encoder)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, PIDControl, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

//
//
// struct
typedef struct{
	int left;
	int right;
	int center;
} LightSensors;
//
//
// headers
//
void getLightSensorsValuesByTouch(string *areaDecription, LightSensors &ls);
void WaitTouchSensor();
task main()
{
	LightSensors lsWhite;
	LightSensors lsBlack;
	
	getLightSensorsValuesByTouch("white", lsWhite);
	getLightSensorsValuesByTouch("black", lsBlack);
	
	WaitTouchSensor();
	
	
	const int minSpeed = 30;
	const int maxSpeed = 100;
		
	float sCenter      = lsBlack.center; // specify float by RobotC divide issue
	float sLeft        = lsWhite.left;
	float sRight       = lsWhite.right;
	
	int v              = maxSpeed;
	int u 				     = 1.0; // manipulated variable
	float vk1          = 1.0; // speed ratio by center light sensor
	int avSpeed        = maxSpeed - minSpeed;
  int es             = lsWhite.left - lsWhite.right; // static (initial) error (correction)
  int vs             = lsWhite.center - lsBlack.center; // static (initial) error (correction) for center
  int e              = es; // error 
  int eOld           = e;  // previous error value 
	
	eraseDisplay();
	
	while(true)
	{
		sCenter   = SensorValue(sLightCenter);
		float vkt = ( lsWhite.center  - sCenter );
		if ( vkt < 0.1 ) vkt = 0.1; 
		vk1       = vkt / vs ;
		
		v         = maxSpeed * vk1;
		
		sLeft    = SensorValue(sLightLeft);
		sRight   = SensorValue(sLightRight);
		
		
		
		e = sLeft - sRight - es;
		u=(e+(e-eOld)*1.5)*1.2;
		eOld=e;
		
		

		nxtDisplayTextLine(1, "vk1   : %f", vk1);
		nxtDisplayTextLine(2, "speed : %d", v);	
		nxtDisplayTextLine(3, "center: %d", sCenter);	
		nxtDisplayTextLine(4, "e:    : %f", e);
		nxtDisplayTextLine(5, "u:    : %f", u);
		
		
		motor[mLeft] = v+u;
		motor[mRight] = v-u;
		
		wait1Msec(10);
	}	
}

///////
//
// waits touch sensonr and 
//
void WaitTouchSensor()
{
	eraseDisplay();
	nxtDisplayTextLine(1, "Stay on the line");
	nxtDisplayTextLine(2, "press button");

	while(SensorValue(sTouch) == 0)
	{
		wait1Msec(10);
	}
	eraseDisplay();  // erase the entire NXT LCD display
	nxtDisplayTextLine(1, "Stay on the line");
	nxtDisplayTextLine(2, "release button");
	nxtDisplayTextLine(3, "to start");

	while(SensorValue(sTouch) == 1)
	{
		wait1Msec(1);
	}
}

///////
//
// waits touch sensor and 
// returns light sensors values
//
void getLightSensorsValuesByTouch(string *areaDecription, LightSensors &ls)
{
	eraseDisplay();
	nxtDisplayTextLine(1, "Stay at %s area", areaDecription);
	nxtDisplayTextLine(2, "press button");

	while(SensorValue(sTouch) == 0)
	{
		wait1Msec(10);
	}

	eraseDisplay();  // erase the entire NXT LCD display
	nxtDisplayTextLine(1, "Stay at %s area", areaDecription);
	nxtDisplayTextLine(2, "release button");
	nxtDisplayTextLine(3, "to save values");

	while(SensorValue(sTouch) == 1)
	{
		ls.left=SensorValue(sLightLeft);
		ls.right=SensorValue(sLightRight);
		ls.center=SensorValue(sLightCenter);

		nxtDisplayTextLine(4, "Left:   %d", ls.left);
		nxtDisplayTextLine(5, "Right:  %d", ls.right);
		nxtDisplayTextLine(6, "Center: %d", ls.center);
		nxtDisplayTextLine(7, "Diff:   %d", ls.left - ls.right);
		
		wait1Msec(10);
	}
	PlaySound(soundBeepBeep);
	wait10Msec(100);
	ClearSounds();
}                                                                                                                                                                                                                                                                                                                            