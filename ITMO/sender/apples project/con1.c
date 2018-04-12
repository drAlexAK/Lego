
task main()
{
	// a = tolkatel b = gnilie c = peregorodca
	nmotorEncoder[motorA] = 0 ;
	nmotorEncoder[motorB] = 0 ;
	nmotorEncoder[motorC] = 0 ;

	motor[motorA]= 30;
	wait1Msec(50);

	int en = nmotorEncoder[motorA];

	motor[motorA]= -30;
	wait1Msec(50/**/);
	if (en > 20)
	{
		while(nMotorEncoder[motorB] < 85)
		{
			motor[motorB]= 30 ;
		}

		while(nMotorEncoder[motorA] < 185)
		{
			motor[motorB]= 30 ;
		}
	}
	//else
	//{
}
