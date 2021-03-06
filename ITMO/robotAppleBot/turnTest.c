#pragma config(Motor,  motorA,          mLeft,         tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, PIDControl, encoder)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define RIGHT_ENC 1100
#define LEFT_ENC 1100
#define CM5_ENC 244
#define CM40_ENC 1950 //1950
#define CM20_ENC 975
#define CM10_ENC 488
#define DEGREES_360_ENC 		4250

void TurnAntiClock();
void TurnClock();
void GoAheadMM(int dist);
void turnRobotDegree(int deg);

task main()
{
	/*float a = radiansToDegrees(atan2(150,`));

	displayBigTextLine(1, "%f", a);

	a  = atan(1);
	*/
	sleep(7000);
	turnRobotDegree(360);
	return;
	turnRobotDegree(-360);
	turnRobotDegree(360);
	turnRobotDegree(-360);
	return;
	TurnAntiClock();
	sleep(3000);
	TurnClock();

	//GoAheadMM(175);

}
// if argument is positive angel, robot will turn clockwise, otherwise - anticlockwise.
void turnRobotDegree(int deg){
	nMotorEncoder[mLeft] =0;
	int	enc = (DEGREES_360_ENC * deg) / 360;
	if(enc > 0){
		while(nMotorEncoder[mLeft] < enc){
			motor[mLeft] = 25;
			motor[mRight] = -25;
		}
		} else {
		while(nMotorEncoder[mLeft] > enc){
			motor[mLeft] = -25;
			motor[mRight] = 25;
		}
	}
	motor[mRight] = motor[mLeft] = 0;
}

void TurnAntiClock(){
	nMotorEncoder[mRight] =0;
	while(nMotorEncoder[mRight] < 4120){
		motor[mLeft] = -50;
		motor[mRight] = 50;
	}
	motor[mLeft] = 0;
	motor[mRight] = 0;
}

void TurnClock(){
	nMotorEncoder[mLeft] =0;
	while(nMotorEncoder[mLeft] < 4120){
		motor[mRight] = -50;
		motor[mLeft] = 50;
	}
	motor[mRight] = 0;
	motor[mLeft] = 0;
}

void GoAheadMM(int dist){ //MM
		int enc = (CM40_ENC * dist) / 400;
		nMotorEncoder[mLeft] =0;
		while(nMotorEncoder[mLeft] < enc){
			motor[mRight] = 50;
			motor[mLeft] = 50;
		}
		motor[mRight] = 0;
		motor[mLeft] = 0;
}
