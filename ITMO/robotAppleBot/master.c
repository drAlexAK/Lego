#include "nxtPipe.h";

ubyte input[4] = {0,0,0,0};
char output[4] = {250,0,0,0};



task read()
{
	int num = 0;
	nxtEnableHSPort();
	nxtHS_Mode = hsRawMode;
	nxtSetHSBaudRate(9600);
	wait1Msec(1000);
	while(1)
	{
		num = nxtGetAvailHSBytes();
		if (num == 4)
		{
			nxtReadRawHS(input, num);
		}
		sleep(1000);
	}
}

task send()
{
	nxtEnableHSPort();
	nxtHS_Mode = hsRawMode;
	nxtSetHSBaudRate(9600);
	wait1Msec(1000);
	while(1)
	{
		nxtWriteRawHS(output, 4, 0);
		wait1Msec(1000);
	}
}

task main()
{
	char a[4] = {'h', 'e', 'l', 'o'};

	SendMsg(a, 4, true, 0, 0);

	bool flag = false;
	startTask(send);
	startTask(read);
	clearTimer(timer1);
	clearTimer(timer2);
	while(1)
	{
		output[1] = time100(timer1) % 255;
		output[2] = random(254);
		if (input[3] > 0 && !flag)
		{
		nxtDisplayTextLine(4, "fuck %i", time100(timer1) / 10);
		flag = true;
		playSound(soundException);
		}
		for (int i =0 ; i <4 ; i++){
		nxtDisplayTextLine(1, "i %i %i %i %i", input[0], input[1], input[2], input[3]);
		nxtDisplayTextLine(2, "o %i %i %i %i", output[0], output[1], output[2], output[3]);
		nxtDisplayTextLine(3, "%i", time100(timer1) / 10);
		sleep(1000);
	}
	}
}
