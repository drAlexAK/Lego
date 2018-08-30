#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)

#define ITER 460

typedef struct power{
	byte left;
	byte right;
	byte sl;
}power;

power p[ITER];

void getRec(power *p, int size);

task main()
{
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
	getRec(p, ITER);
	sleep(1000);
	for(int i = 0; i < ITER; i ++){
		motor[mLeft] = p[i].left;
		motor[mRight] = p[i].right;
		sleep(p[i].sl);
		//sleep(25);
	}
}

void getRec(power *p, int size){
	TFileHandle hFile;
	TFileIOResult ioResult;
	string fileName = "rec.bin";
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
