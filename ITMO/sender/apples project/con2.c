#pragma config(Sensor, S1,     color,          sensorEV3_Color, modeEV3Color_Color)
#pragma config(Motor,  motorA,          mCon,          tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorB,          mB,            tmotorNXT, PIDControl, encoder)
#pragma config(Motor,  motorC,           ,             tmotorEV3_Large, openLoop)
#pragma config(Motor,  motorD,           ,             tmotorEV3_Large, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

task conV();

void getYellow();
void getRed();

#define redCForY 50
#define blueCForY 25
#define greenCForY 5

#define redCForR  45
#define blueCForR  40
#define greenCForR  30

long greenValue;
long blueValue;
long redValue;

task main()
{

nMotorEncoder[mB] =0;

int k =0;
int i =0;

	startTask(conV);

	while(true){
		getColorRGB(color,redValue,greenValue,blueValue);

		if((redValue > redCForR) && (blueValue < blueCForR) && (greenValue < greenCForR))
			{
				i = 0;
				k++;
				sleep(100);
			}
			else if((redValue > redCForY) && (blueValue > blueCForY) && (greenValue < greenCForY))
			{
				k = 0;
				i++;
				sleep(100);
			}
			if(k > 2) getRed();
			else if(i > 2) getYellow();
	}

}

task conV(){
	while(true){
		motor[mCon] = 100;
		sleep(10);
	}
}

void getRed(){
	while(nMotorEncoder[mB] > -41){
	motor[mB] =30;
	}
	motor[mB] = 0;
}

void getYellow(){
	while(nMotorEncoder[mB] < 0){
	motor[mB] = -30;
	}
	motor[mB] = 0;
}
