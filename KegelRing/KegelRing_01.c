#pragma config(Sensor, S1,     sSonarLeft,     sensorSONAR)
#pragma config(Sensor, S2,     sUltrasonicCenter, sensorEV3_Ultrasonic)
#pragma config(Sensor, S3,     sSonarRight,    sensorSONAR)
#pragma config(Sensor, S4,     sLight,         sensorLightActive)
#pragma config(Motor,  motorA,          mLeft,         tmotorEV3_Large, openLoop, reversed, encoder)
#pragma config(Motor,  motorB,           ,             tmotorEV3_Large, openLoop)
#pragma config(Motor,  motorC,           ,             tmotorEV3_Large, openLoop)
#pragma config(Motor,  motorD,          mRight,        tmotorEV3_Large, openLoop, reversed, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

task main()
{
	// init sensors
	SensorValue(sLight) = 0;
	SensorValue(sSonarLeft) = 0;
	SensorValue(sSonarRight) = 0;
	SensorValue(sUltrasonicCenter) = 0;

	const int black          = 20;
	const int maxSpeed       = 120;
	int v                    = maxSpeed; 	// current speed
	const int distance       = 70;				//
	const int distanceCenter = 30;

	int lastAction=0; // 0 - start, 1 - back; 2 - forward; 3 - turn left; 4 - turn right

	int iForward=0;												// forward counter, preservs outside
	const int minForwardIteration = 200;	//

	while(true)
	{
		sleep(1);

		int light = SensorValue(sLight);
		iForward = iForward + 1;	// add forward counter

		if ((light < black) && (iForward>=minForwardIteration)) // back
		{
			iForward=0; // reset forward counter
			for ( int i = -60; i<=40; i = i + 10) // slow back
			{
				motor[mLeft] = -i ;
				motor[mRight] = -i ;
				sleep(35);
			}

			int rearLeft 	= 1;
			int rearRight = 1;

			if (lastAction==3) rearLeft = 0.7;
			if (lastAction==4) rearRight = 0.7;

			for ( int i = 40; i<=maxSpeed; i = i + 20) // slow back
			{
				motor[mLeft] = -i * rearLeft;
				motor[mRight] = -i  * rearRight;
				sleep(100);
			}

			rearLeft 	= 1;
			rearRight = 1;
			if (lastAction==2) rearRight = 0.6;
			if (lastAction==3) rearRight = 0.6;
			if (lastAction==4) rearLeft  = 0.6;

			for ( int i = maxSpeed; i>0; i = i - 20) // slow stop
			{
				motor[mLeft] = -i * rearLeft;
				motor[mRight] = -i * rearRight;
				sleep(150);

			}
			lastAction = 1; // back
		}
		else
		{
			int sonarLeft = SensorValue(sSonarLeft);
			int sonarRight = SensorValue(sSonarRight);
			int ultrasonicCenter =  SensorValue(sUltrasonicCenter);


			if (((ultrasonicCenter < distanceCenter) &&  (ultrasonicCenter > 0)) || ((sonarLeft < distance) && (sonarRight < distance))) // forward
			{
				motor[mLeft]  = v ;
				motor[mRight] = v ;
				lastAction = 2; // forward
			}
			else
			{
				if (sonarLeft < distance) // turn left
				{
					motor[mLeft] = v / 2;
					motor[mRight] = v ;
					lastAction = 3; // turn left
				}
				else // turn right
				{
					motor[mLeft] = v ;
					motor[mRight] = v / 2 ;
					lastAction = 4; // turn right
				}
			}
		}
	}
}
