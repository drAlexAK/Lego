#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)

#define ITER 460

typedef struct power{
	byte left;
	byte right;
	byte sl;
}power;

power p[ITER];

void getRec(power *p, int size, string fileName);
int getFileName();

task main()
{
	string fileName;
	getFileName();
	sprintf(fileName, "rec%d.bin",getFileName());
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
	getRec(p, ITER, fileName);
	sleep(1000);
	for(int i = 0; i < ITER; i ++){
		motor[mLeft] = p[i].left;
		motor[mRight] = p[i].right;
		sleep(p[i].sl);
		//sleep(25);
	}
}

int getFileName(){

	string fileName;
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
			//sprintf(fileName, "rec%d.bin", i);

			return i;
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
		//if(i >= IFILE) i = i - IFILE;

		displayBigTextLine(4, "  %d", i);

		sleep (10);
	}
	displayBigTextLine(4, "  DONE");
	sleep(1000);
}

void getRec(power *p, int size, string fileName){
	TFileHandle hFile;
	TFileIOResult ioResult;
	short sizeFile = size * sizeof(power);
	byte rByte;

	OpenRead(hFile, ioResult, fileName, sizeFile);
	ReadByte(hFile, ioResult, rByte);

	for(int i = 0; i < sizeFile; i++){
		ReadByte(hFile, ioResult, rByte);
		p[i].left = rByte;

		ReadByte(hFile, ioResult, rByte);
		p[i].right= rByte;

		ReadByte(hFile, ioResult, rByte);
		p[i].sl = rByte;

	}
}
