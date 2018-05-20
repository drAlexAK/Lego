// ConsoleApplication4.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "stdlib.h"
#include "math.h"
#include "cassert"

int getLimitSpeed(const int speedMin, int speedMax, int startEnc, int currentEnc, int targetEnc);

int sgn(int x){
	if (x >= 0) return 1;
	if (x < 0) return -1;
	return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	
	int res[50] = {0};
	for (int i = -500; i< 0; i+=10){
		res[abs((i + 500) / 10)] = getLimitSpeed(20,80,-500, i,0);

	}
	int s[50] = {0};
	for (int i = 500; i> 0; i-=10){
		s[abs((500 -i) / 10)] = getLimitSpeed(20,80,500,i,0);
	}
	

	int r = 0;
	int speedMin = 10;
	int speedMax = 50;
	const int speedMiddle = speedMin + (speedMax - speedMin) / 2; 
	const int speedQuatter = speedMin + (speedMax - speedMin) / 4;
	const int speedOneEighth = speedMin + (speedMax - speedMin) / 8;
	r = getLimitSpeed(speedMin, speedMax, 0, -166, -318);
	// max speed at the middle
	assert( getLimitSpeed(speedMin, speedMax, 0, 400, 2000) == speedMax);
	assert( getLimitSpeed(speedMin, speedMax, 0, -400, -2000) == -1 * speedMax);
	assert( getLimitSpeed(speedMin, speedMax, -100, -500, -2000) == -1 * speedMax);
	assert( getLimitSpeed(speedMin, speedMax, -100, 300, 1000) == speedMax);
	// speed up at the start
	assert( getLimitSpeed(speedMin, speedMax, 0, 180, 2000) == speedMiddle);
	assert( getLimitSpeed(speedMin, speedMax, 0, 90, 180) == speedQuatter);
	//assert( getLimitSpeed(speedMin, speedMax, 0, 45, 180) == speedOneEighth);
	//assert( getLimitSpeed(speedMin, speedMax, 0, 135, 180) == speedOneEighth); // here is bug
	assert( getLimitSpeed(speedMin, speedMax, 0, 90, 2000) == speedQuatter);
	assert( getLimitSpeed(speedMin, speedMax, -180, 0, 2000) == speedMiddle);
	assert( getLimitSpeed(speedMin, speedMax, -90, 0, 2000) == speedQuatter);
	assert( getLimitSpeed(speedMin, speedMax, 0, -180, -2000) == -1 * speedMiddle);
	assert( getLimitSpeed(speedMin, speedMax, -180, -180, 2000) == speedMin);
	assert( getLimitSpeed(speedMin, speedMax, -180, -180, -2000) == -1 * speedMin);
	// slow down at the end
	assert( getLimitSpeed(speedMin, speedMax, 0, 1000, 1360) ==  speedMax);
	assert( getLimitSpeed(speedMin, speedMax, 0, 1180, 1360) ==  speedMiddle);
	assert( getLimitSpeed(speedMin, speedMax, 0, 1270, 1360) ==  speedQuatter);
	assert( getLimitSpeed(speedMin, speedMax, 0, -1270, -1360) ==  -1 * speedQuatter);
	assert( getLimitSpeed(speedMin, speedMax, -180, 0, 180) ==  speedMiddle);
	
	
	speedMin = 20;
	speedMax = 80;
	//_______________________________________________________________________________
	assert( getLimitSpeed(speedMin, speedMax, -500, 0, 0)    == 20);//				 |
	assert( getLimitSpeed(speedMin, speedMax, -500, -60, 0)  == 29);//				 |
	assert( getLimitSpeed(speedMin, speedMax, -500, -250, 0) == 61);//				 |
	assert( getLimitSpeed(speedMin, speedMax, -500, -440, 0) == 29); // ***		     |
	assert( getLimitSpeed(speedMin, speedMax, -500, -500, 0) == 20);//				 |
	//-------------------------------------------------------------------------------
	assert( getLimitSpeed(speedMin, speedMax, 500, 500, 0)   == -20);//				 |
	assert( getLimitSpeed(speedMin, speedMax, 500, 440, 0)  == -29);//	        	 |
	assert( getLimitSpeed(speedMin, speedMax, 500, 250, 0) == -61);//				 |
	assert( getLimitSpeed(speedMin, speedMax, 500, 60, 0) == -29); // ***			 |
	assert( getLimitSpeed(speedMin, speedMax, 500, 0, 0)  ==  20);//				 |
	//-------------------------------------------------------------------------------


	return r;

}

int getLimitSpeed(const int speedMin, 
				  const int speedMax, 
				  const int startEnc, 
				  const int currentEnc, 
				  const int targetEnc){

					  const int maxEnc = 360; //540
					  int dir = sgn(targetEnc - currentEnc); // direction 
					  int complPercent = 0;

					  if(dir * (targetEnc) < dir * (currentEnc)) return dir * speedMin; // current encoder longer than target 
					  if ((dir * (currentEnc - startEnc  - dir * maxEnc) > 0) && 
						  (dir * (currentEnc - targetEnc + dir * maxEnc) < 0)) return dir * speedMax; // current enc longer than maxEncoder and shorter than target enc - maxEncoder

					  if (abs(targetEnc - startEnc) < maxEnc * 2){  
						  if( abs(currentEnc - startEnc) < abs((targetEnc - startEnc) / 2)) 
							  complPercent = abs ((currentEnc - startEnc) * 100 / maxEnc);
						  else
							  complPercent = abs(abs(targetEnc - startEnc) * 100 / maxEnc - (abs(currentEnc - startEnc) * 100 / maxEnc));
					  }
					  else if (dir * (currentEnc - startEnc  - dir * maxEnc) <= 0)  {
						  complPercent = abs((currentEnc - startEnc) * 100 / (maxEnc));
					  } else {
						  complPercent = 100 - abs((currentEnc - (targetEnc - dir * maxEnc)) * 100 / maxEnc);
					  }

					return (dir * (speedMin + (speedMax - speedMin) * complPercent / 100));
}
