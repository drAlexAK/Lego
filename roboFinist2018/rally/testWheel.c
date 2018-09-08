#pragma config(Motor,  motorB,          mWheel,        tmotorNXT, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define WHEEL_ENCODER_MAX 175

task main()
{

	int r = sgn(0);

	resetMotorEncoder(mWheel);

	motor[mWheel] = 10;
	eraseDisplay();

	while(true) {
		if (abs(WHEEL_ENCODER_MAX) <= abs(nMotorEncoder[mWheel])) break;
		displayBigTextLine(2, "%d", nMotorEncoder[mWheel]);
		sleep(10);
	}

	motor[mWheel] = 0;

}
