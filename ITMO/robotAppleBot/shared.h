int getLimitSpeed(const int speedMin, int speedMax, int currentEnc, int targetEnc);

// speed limiter
int getLimitSpeed(const int speedMin,const int speedMax, const int currentEnc, const int targetEnc){
	const int maxEnc = 360; //540
	//int direction = sgn(targetEnc);
	currentEnc = abs(currentEnc);
	targetEnc = abs (targetEnc);

	if(targetEnc < currentEnc) currentEnc = targetEnc;
	if((currentEnc >= maxEnc) && (currentEnc <= targetEnc - maxEnc)) return speedMax;

	if(targetEnc < (maxEnc * 2)){
		int accel = (speedMax - speedMin) / maxEnc;
		speedMax = ((targetEnc / 2) * accel) + speedMin;
		} else {
		if (currentEnc > maxEnc) currentEnc -= (targetEnc - (maxEnc*2));
		targetEnc = (maxEnc * 2);
	}

	int percentComplete = (currentEnc * 100) / targetEnc;

	if(percentComplete < 50){
		return (speedMin + ((speedMax - speedMin) * percentComplete) / 100);
		}else{
		return (speedMin + (speedMax - speedMin) * (100 - percentComplete) / 100);
	}
}