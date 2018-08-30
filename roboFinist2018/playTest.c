#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)

#define MAX_BUFFER 500

typedef struct power{
	byte left;
	byte right;
	byte sl;
}power;

string fileName = "";
power p[MAX_BUFFER];
byte Iter  = 0;

void getRec(power *p);
void getFileName();

task main()
{
	getFileName();

	displayBigTextLine(4, "  START");
	sleep(1000);

	while(true)
	{
		if (nNxtButtonPressed == 3)
		{
			while (nNxtButtonPressed == 3)
			{
				sleep (10);
			}
			break;
		}
		sleep (10);
	}

	sleep(1000);
	getRec(p);

	sleep(1000);
	for(int i = 0; i < Iter; i ++){
		motor[mLeft] = p[i].left;
		motor[mRight] = p[i].right;
		sleep(p[i].sl);
		//sleep(25);
	}
	motor[mLeft] = 0;
	motor[mRight] = 0;

}

void getFileName(){

	int i =0;

	displayBigTextLine(4, "  FILE NAME");

	while(true)
	{
		if (nNxtButtonPressed == 3)
		{
			while (nNxtButtonPressed == 3)
			{
				sleep (10);
			}
			string tmp = "";
			sprintf(tmp, "rec%d.bin", i);
			fileName = tmp;
			return;
		}

		if (nNxtButtonPressed == 1)
		{
			while (nNxtButtonPressed == 1)
			{
				sleep (10);
			}
			i++;
		}

		if (nNxtButtonPressed == 2)
		{
			while (nNxtButtonPressed == 2)
			{
				sleep (10);
			}
			i--;
		}

		if(i < 0) i = 0;

		displayBigTextLine(4, "  %d", i);

		sleep (10);
	}
}

void getRec(power *p){
	TFileHandle hFile;
	TFileIOResult ioResult;
	short fileSize = 1;
	byte rByte;

	OpenRead(hFile, ioResult, fileName, fileSize);
	ReadByte(hFile, ioResult, Iter); // get iteration
	memset(p, Iter, Iter * sizeof(power));
	Close(hFile, ioResult);

	fileSize = Iter * sizeof(power) + 1;
	OpenRead(hFile, ioResult, fileName, fileSize);

	ReadByte(hFile, ioResult, rByte);
	ReadByte(hFile, ioResult, rByte);

	for(int i = 0; i < Iter; i++){
		ReadByte(hFile, ioResult, rByte);
		p[i].left = rByte;

		ReadByte(hFile, ioResult, rByte);
		p[i].right= rByte;

		ReadByte(hFile, ioResult, rByte);
		p[i].sl = rByte;

	}
	Close(hFile, ioResult);
}
