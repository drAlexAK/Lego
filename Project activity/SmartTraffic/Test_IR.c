#pragma config(Sensor, S1,     sDistA1,        sensorI2CCustomFastSkipStates9V)

#include "mindsensors-irdist.h"

task main()
{
	int d =0;
	ubyte address = 0x02;
	string type = MSDISTreadModuleType(sDistA1, address);
	while(true){
		d = MSDISTreadDist(sDistA1, address);

		displayBigTextLine(3,"		%d",d);
		if((d < 50) || (d > 800)){
			displayBigTextLine(5, " 		FAILE");
			sleep(5000);
			displayBigTextLine(5, " 		     ");
		}

		sleep(100);
	}
}
