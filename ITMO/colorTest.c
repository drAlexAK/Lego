#pragma config(Sensor, S1,     sColor,         sensorEV3_Color)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

int greenValue;
int redValue;
int blueValue;

task main()
{
	while(true){
		getColorRGB(sColor,redValue,greenValue,blueValue);
		displayBigTextLine(1,"%d",redValue);
		displayBigTextLine(3,"%d",greenValue);
		displayBigTextLine(5,"%d",blueValue);
	}
}