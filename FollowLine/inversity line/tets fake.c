#pragma config(Sensor, S1,     sLightRight,    sensorI2CCustom9V)
#pragma config(Sensor, S4,     sLightLeft,     sensorI2CCustom9V)
#pragma config(Motor,  motorB,          mLeft,         tmotorNXT, openLoop)
#pragma config(Motor,  motorC,          mRight,        tmotorNXT, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "mindsensors-lineleader.h"

//                                           0 = Gray Rectangle button. 1 = Right Arrow button. 2 = Left Arrow button. 3 = Orange Square button.

/*
COME TO THE DARK SIDE!!!
                 _.-'~~~~~~`-._
                /      ||      \
               /       ||       \
              |        ||        |
              | _______||_______ |
              |/ ----- \/ ----- \|
             /  (     )  (     )  \
            / \  ----- () -----  / \
           /   \      /||\      /   \
          /     \    /||||\    /     \
         /       \  /||||||\  /       \
        /_        \o========o/        _\
          `--...__|`-._  _.-'|__...--'
                  |    `'    |
*/

// time: 	      stable
// direction: left
// motors: 	  2
// ger: 	    direct
// tire:      \__/ #61481 + #56145c04

#define RELEASE
//#define DEBUG

typedef float tFloatArray[8];

int getRWRight  ( tByteArray rawLight, tFloatArray kl);
int getRWLeft   ( tByteArray rawLight, tFloatArray kl);
//int getRWCenter ( tByteArray rawLight, tFloatArray kl);
void calibrate ();
void waitTouchRelease();
void reverse(tByteArray a);
int getStartSquare();
//-------------------
float KL[8] = { 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 };
//--------------------
int vBase 			= 75;
int const vMax  = 100;
int const vMin	= 10;
int const maxI	= 10;
int const kReturn = 4;
float const k 	= 32;
int iAlert			= 0;
int const aLenght	= 12;

int a[aLenght] = {0,-1,0,0,-1,0,-1,-1,1,1,-1,-1};

bool leftAlert  = false;
bool rightAlert = false;
//--------------------
task speedUp()
{
	int vFinish        = vBase;
	int const vStart   = 30;
	int const tSpeedUp = 600;
	int tSleep = tSpeedUp / ( vFinish - vStart );

	for ( int i = vStart ; i <= vFinish ; i++ )
	{
		vBase = i ;
		sleep(tSleep);
	}
}

//--------------------
task main()
{

	int iPos = getStartSquare();

	LLinit(sLightLeft); 	// Set up Line Leader sensor type
	LLinit(sLightRight); 	// Set up Line Leader sensor type

	_lineLeader_cmd(sLightLeft, 'E'); // European frequency compensation
	_lineLeader_cmd(sLightRight, 'E'); // European frequency compensation

	calibrate();
	waitTouchRelease();

	startTask(speedUp);

	tByteArray rawLightLeft;
	tByteArray rawLightRight;

	LLreadSensorRaw(sLightLeft, rawLightLeft);
	int rwLeft  		= getRWLeft(rawLightLeft, KL);

	LLreadSensorRaw(sLightRight, rawLightRight);
	int rwRight 		= getRWRight(rawLightRight, KL);

	//----------------------------------------------------------------------------

	int e       		= 0;
	int eOld 				= e;
	int es 					= rwLeft - rwRight ;
	float i 				= 0;
	int 	v 				= 0 ;
	int   u					= 0 ;
	int vLeft 			= 0;
	int vRight 			= 0;
	long iSpeed     = 0;
	leftAlert  			= false;
	rightAlert 			= false;
	bool isItBlack   = false;
	bool isItBlackOld    = false;
	int summSensor   =0;

	while(true)
	{
		//------------
		LLreadSensorRaw(sLightLeft, rawLightLeft);
		LLreadSensorRaw(sLightRight, rawLightRight);

		//	if (rightAlert) isItBlack =((rawLightLeft[7] + rawLightLeft[6]) < 200);
		//	else if (leftAlert) isItBlack =((rawLightRight[0] + rawLightRight[1]) < 200);
		//	else isItBlack =((rawLightLeft[0] + rawLightLeft[7] + rawLightRight[0] + rawLightRight[7] + rawLightCenter[0] + rawLightCenter[7] ) < 300);
		  isItBlack =((rawLightLeft[0] + rawLightLeft[7] + rawLightRight[0] + rawLightRight[7]) < 200) ;


		//summSensor = 0;

		//for(int i = 0; i < 8; i++)
		//{
		//	summSensor = summSensor + rawLightLeft[i] + rawLightRight[i];
		//}

		//isItBlack = (summSensor < 800);

		//if (isItBlackOld != isItBlack) sleep(20);
		//if((isItBlackOld != isItBlack) && ((leftAlert == true))
		//{
		//leftAlert = true;
		//rightAlert = false;
		//kAlert = 0;
		//}
		//else if(rightAlert == true)) && (kAlert >= 1)

		if ( isItBlack == true )
		{
			reverse( rawLightRight );
			reverse( rawLightLeft );
		}

		rwLeft = getRWLeft(rawLightLeft, KL);
		rwRight = getRWRight(rawLightRight,KL);
		//-----------------------

		if (isItBlackOld != isItBlack)
		{
			i = 0;
			iPos++;
			iPos = (iPos % aLenght) - 1;
		}

   switch(a[iPos])
   {
     case -1:
     vLeft = 20;
     vRight = 40;
     break;

     case 1:
     vRight = 20;
     vLeft = 40;
     break;

   default:

		if (((leftAlert && (iAlert > 1)) && (rawLightLeft[7] > 40) && (rawLightLeft[6] > 40) && (rawLightRight[0] > 40)) && (isItBlackOld == isItBlack))
		{
			if (((rawLightLeft[2] < 20) || (rawLightLeft[1] < 20) || (rawLightLeft[0] < 20) ||
				(rawLightRight[7] < 20) || (rawLightRight[6] < 20) || (rawLightRight[5] < 20) || (rawLightRight[4] < 20) || (rawLightRight[3] < 20) ||
			(rawLightRight[2] < 20) || (rawLightRight[1] < 20))) leftAlert = false;
		}

		if (leftAlert) rightAlert = false;

		if (((rightAlert && (iAlert > 1)) && ( rawLightRight[0] > 40) && ( rawLightRight[1] > 40) && (rawLightLeft[7] >  40)) &&  (isItBlackOld == isItBlack))
		{
			if (( (rawLightRight[5] < 20) || (rawLightRight[6] < 20) || (rawLightRight[7] < 20) ||
				(rawLightLeft[0] < 20) || (rawLightLeft[1] < 20) || (rawLightLeft[2] < 20) || (rawLightLeft[3] < 20) || (rawLightLeft[4] < 20) ||
			(rawLightLeft[5] < 20) || (rawLightLeft[6] < 20))) rightAlert = false ;
		}
		if (rightAlert) leftAlert = false;
		// end right
		// end of original line

		e = rwLeft - rwRight  - es;

		if (leftAlert)
		{
			vLeft  = vMin;
			vRight = 75 ;
			for (int j = 0; j <= 7; j++  )
			{
				if (rawLightLeft[j] < 20 )
				{
					vRight = vRight - (8 - j) * kReturn;
					vLeft  = vLeft  + (8 - j) * kReturn;
					break;
				}
			}
			iAlert ++;
		}
		else if (rightAlert)
		{
			vLeft  = 75;
			vRight = vMin;
			for (int j = 7; j >= 0; j--  )
			{
				if (rawLightRight[j] < 20 )
				{
					vLeft  = vLeft  - (j + 1) * kReturn;
					vRight = vRight + (j + 1) * kReturn;
					break;
				}
			}
			iAlert ++;
		}
		else
		{
			if ( iAlert != 0 )
			{
				i = 0;
				iSpeed = 0;
			}

			i = i + e / 2500;
			if ( fabs(i) > maxI ) i = sgn(i) * maxI ;
			u = (e * 1  + (e - eOld ) * 7) / k + i;
			v = (vBase - abs (u) * 0.75 ) ;
			vLeft = v + u;
			vRight = v - u;
			iAlert = 0;
		}

		eOld = e ;

}
		isItBlackOld = isItBlack;

		if (vLeft  < vMin)  vLeft  = vMin;
		if (vRight < vMin)  vRight = vMin;
		if (vLeft  > vMax)  vLeft  = vMax;
		if (vRight > vMax)  vRight = vMax;



#ifdef RELEASE
		motor[mLeft]  = vLeft;
		motor[mRight] = vRight;
#endif
#ifdef DEBUG
		displayTextLine(0,"u %d", u);
		displayTextLine(1,"vLeft %d",vLeft);
		displayTextLine(2,"vRight6 %d", vRight);

		displayTextLine(3,"rwRight %d", rwRight);
		displayTextLine(4,"rwLeft %d", rwLeft);
		displayTextLine(6,"e %d",e);

		displayTextLine(2,"lAlert %d", leftAlert);
		displayTextLine(3,"rAlert %d",rightAlert);
		/*
		displayTextLine(6,"vLeft %d",vLeft);
		displayTextLine(7, "vRight %d",vRight);

		displayTextLine(2,"lAlert %d", leftAlert);
		displayTextLine(3,"rAlert %d",rightAlert);
		displayTextLine(4,"e %d",e);
		displayTextLine(5,"u %d",u);
		displayTextLine(6,"vLeft %d",vLeft);
		displayTextLine(7, "vRight %d",vRight);
		*/
#endif
		sleep (1);
	}
	//if (SensorValue(sTouch) == 1) break;
	LLsleep(sLightLeft);  // Sleep to conserve power when not in use
	LLsleep(sLightRight); // Sleep to conserve power when not in use
}
//---------------------------------

void calibrate ()
{
	displayBigTextLine(4, "  WHITE");

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
	LLcalWhite(sLightLeft);
	LLcalWhite(sLightRight);

	playSound(soundBlip);

	displayBigTextLine(4, "   Done");

	sleep(1000);

	displayBigTextLine(4, "  BLACK");

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
	LLcalBlack(sLightLeft);
	LLcalBlack(sLightRight);

	playSound(soundBlip);

	displayBigTextLine(4, "  Done");

	sleep(1000);
}

//-------------------------------

void waitTouchRelease()
{
	displayBigTextLine(4, " START");

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
	eraseDisplay();
}
//---------------
int getRWLeft( tByteArray &rawLight, tFloatArray &kl)
{
	int r = 0 ;
	for(int i = 0; i < 8; i++)
	{
		r = r + rawLight[i] * kl[i];
	}
	if (( rawLight[7] < 20 ) && ( rawLight[0] > 20 )) r = r * -1;
	return r;
}
//--------------------
int getRWRight ( tByteArray &rawLight, tFloatArray &kl)
{
	int r = 0 ;
	for(int i = 0; i < 8; i++)
	{
		r = r + rawLight[i] * kl[7 - i];
	}
	if (( rawLight[0] < 20 ) && ( rawLight[7] > 20 )) r = r * -1;
	return r;
}

void reverse(tByteArray &a)
{
	for(int i = 0; i < 8; i++)
	{
		a[i] = abs(a[i] - 100);
	}
}

int getStartSquare()
{
int iCount = 0;

  do
  {
   if (nNxtButtonPressed == 1)
   {
     while (nNxtButtonPressed == 1) sleep(100);
     iCount++;
   }
   else if (nNxtButtonPressed == 2)
   {
     while (nNxtButtonPressed == 2) sleep(100);
     iCount--;
   }

   if (iCount < 0) iCount = aLenght;
   else if (iCount > aLenght) iCount = 0;

		displayBigTextLine(1, "%d", iCount);

  }while(nNxtButtonPressed != 3);

  while(nNxtButtonPressed == 3)
  {
  	sleep(100);
  }
  	return iCount;
}