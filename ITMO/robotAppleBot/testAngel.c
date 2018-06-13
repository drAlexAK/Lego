int getAngelDeviationDegree(int errorDist, int testLenght);


task main()
{
	int error = 0;
	while (true) {
		if (getButtonPress(buttonLeft))   error -=10;
		if (getButtonPress(buttonRight))  error += 10;
		displayBigTextLine(2, "200 %d", error);
		displayBigTextLine(4, "    %d", sgn(error) * getAngelDeviationDegree(abs(error), 200));
		sleep(100);
	}
}

// returns degree of diviation by error and lenght distances
int getAngelDeviationDegree(int errorDist, int testLenght){
	return radiansToDegrees( atan2(errorDist, testLenght));
}
