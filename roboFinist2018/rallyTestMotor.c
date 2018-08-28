#pragma config(Motor,  motorA,          motorA,        	tmotorNXT, openLoop)
#pragma config(Motor,  motorB,          motorB,         tmotorNXT, openLoop)

task main()
{
	//int v = 0;

	for(int i = 0; i <= 100;  i += 5){
 			motor[motorA] = i;
 			motor[motorB] = i;
 			sleep(200);
	}

	for(int i = 100; i >= 50;  i -= 5){
 			motor[motorA] = i;
 			motor[motorB] = i;
 			sleep(200);
	}

	motor[motorA] = 0;
	motor[motorB] = 0;

	for(int i = 0; i <= 100;  i += 5){
 			motor[motorA] = i;
 			motor[motorB] = i;
 			sleep(200);
	}

}
