#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)

typedef struct power{
	byte left;
	byte right;
}power;

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

	power p[240];

	getRec(p, 240);

	for(int i = 0; i < 240; i ++){
		motor[mLeft] = p[i].left;
		motor[mRight] = p[i].right;
		sleep(25);
	}
}

void getRec(power *p, int size){
	TFileHandle hFile;
	TFileIOResult ioResult;
	string fileName = "rec.bin";
	short sizeFile = size * sizeof(power);
	char power2;

	OpenRead(hFile, ioResult, fileName, sizeFile);
	for(int i = 0; i < sizeFile; i++){
		ReadByte(hFile, ioResult, power2);
		if(i % 2 != 1){
			p[i/2].left = power2;
			}else{
			p[i/2].right = power2;
		}
	}
}
