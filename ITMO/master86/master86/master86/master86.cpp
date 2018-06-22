#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include "BTConnect.h"
#include "sstream"
#include "string"
#include "fstream"
#include "map"
#include "cmath"
#include <time.h>

#define CAMERA
#define LEGO
#define FLOOD_PROTECT
#define CAMERA_INDEX 1


using namespace cv;
using namespace std;

typedef struct ScalarRange{
	ScalarRange(Scalar ls, Scalar us){
		lowBound = ls;
		upBound = us;
	}
	Scalar lowBound;
	Scalar upBound;
}ScalarRange;

////
int Capture();
bool compar(short *a1, short *a2, int size);
bool comparAppr(short *a1, short *a2, int size);
void zeroArray(short *a1, int size);
void copyArr(short *a1, short *a2, int size);
void GetListOfBricks(vector<btSender> &lBricks);
vector<string> GetListOfCOMPorts();
void builtTreshHold(Mat &src, Mat &thd, vector<ScalarRange> &inlude);
void getIncludeScalar(vector<ScalarRange> &scRange, string inFile);
void initInclude();
void loadIncludes(vector<ScalarRange> &scRange);
void getErAndDi(Size &Er1, Size &Di1, Size &Di2, Size &Er2);
int getBrokenLine(OutputArrayOfArrays curve);
void geometry(Mat &treshHold);
string getColorFilterAsString();
void getSetings();
////

int points =0;
double minMass =0;

typedef enum color{
	none	= 0,
	red		= 2,
	yellow  = 4,
	green   = 8
}color;

map<color, string> includeNameOfFile;
int colorFilter = red | yellow; 

Mat dst;

int main()
{
	Capture();
	return 0;
}

vector<string> GetListOfCOMPorts() 
{
	vector<string> lCom;
	string line;
	string i ="";
	ifstream file("comPorts.txt");
	while(getline(file, line)){
		if((line.length() > 3) && (line[0] != '/')){
			istringstream sStream(line);
			sStream >> i;
			lCom.push_back(i);
		}
	}

	return lCom;
}

void GetListOfBricks(vector<btSender> &lBricks)
{
	vector<string>  lCom =  GetListOfCOMPorts();
	for (vector<string>::iterator it = lCom.begin(); it != lCom.end(); it++)
	{
		while(true){		
			cout << "Connecting to '" << *it << "'" << endl;
			lBricks.push_back (btSender());
			if (lBricks.back().Connect(*it))
			{
				cout << "Has been connected to '" << *it << "'" << endl; 
				break;
			}
			cout << "Error: " << lBricks.back().GetErrorID() << " " << lBricks.back().GetErrorMessage() << endl;
			Sleep(1000); 
		} 
	}
}


int Capture()
{
	char key = '0';  
	const int msgElements = 3;
	short *dataToSend = new short[msgElements];
	short *dataToSendOld = new short[msgElements];
	short *msg = new short[msgElements];
	Size er1, er2, di1, di2;
	vector<btSender> lBricks;
	Mat imgTmp;
	bool chekGeometry = true;
	// Start and end times
	time_t start, end;
	int frmCount = 0;
	int fps = 0;

	getErAndDi(er1, di1, di2, er2);
#ifdef LEGO
	GetListOfBricks(lBricks);
#endif

#ifdef CAMERA
	VideoCapture cap(CAMERA_INDEX); //capture the video from webcam

	if ( !cap.isOpened() )  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}
	//Capture a temporary image from the camera
	cap.read(imgTmp);
#else
	imgTmp= imread(".\\pictures\\red.png", IMREAD_COLOR); // Read the file
#endif

	Scalar fontScalar(255, 255, 255);
	int rowCenter = imgTmp.rows / 2;
	int colCenter = imgTmp.cols / 2;
	vector<ScalarRange> include;
	getSetings();

	initInclude();
	loadIncludes(include);
	// improve perfomance
	Mat imgOriginal(imgTmp.size(), CV_8UC3);
	Mat imgOriginalGray(imgTmp.size(), CV_8UC1);
	Mat imgThresholded(imgTmp.size(),  CV_8UC1);
	double sumBrightness;
	int maxBrightnessSum = imgTmp.rows * imgTmp.cols * 255 / 100; // 100% lights from matrix
	int brightness = 0;

	while (true)
	{
		if (frmCount==0) time(&start); // Start time

#ifdef CAMERA
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
#else
		if(imgOriginal.data == NULL)imgOriginal= imread(".\\pictures\\red.png", IMREAD_COLOR); // Read the file
#endif
		cvtColor(imgOriginal, imgOriginalGray, COLOR_BGR2GRAY);
		sumBrightness = sum(imgOriginalGray)[0]; 
		GaussianBlur(imgOriginal, imgOriginal, Size(7, 7), 0, 0);   // standart kenel size 7x7 for best perfomance
		//GaussianBlur(imgOriginal, imgOriginal, Size(5, 5), 0, 0); // size only odd
		//blur(imgOriginal, imgOriginal, Size(5, 5)); // too much

		cvtColor(imgOriginal, imgOriginal, COLOR_BGR2HSV);
		builtTreshHold(imgOriginal, imgThresholded, include);

		//morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, er1));
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, di1));

		//morphological closing (removes small holes from the foreground), two iteration
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, di2), Point(-1, -1), 2); 
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, er2), Point(-1, -1), 2);

		if((colorFilter != none) && (chekGeometry)) geometry(imgThresholded);

		brightness = sumBrightness / maxBrightnessSum;
		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// skip small objects
		if (dArea > minMass)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;        

			circle(imgOriginal, Point(posX, posY), 20, Scalar(200, 100, 50), 5);

			if (posX >= 0 && posY >= 0)
			{
				dataToSend[0] = posY - rowCenter;
				dataToSend[1] = posX - colCenter;
				dataToSend[2] = brightness | 1;				
			}
			else
			{
				zeroArray(dataToSend, msgElements);
				dataToSend[2] = brightness &~ 1;
			}
		}
		else
		{
			zeroArray(dataToSend, msgElements);
			dataToSend[2] = brightness &~ 1;
		}

		if (!comparAppr(dataToSend, dataToSendOld, 3)) {
			cout << dataToSend[0] << " " << dataToSend[1] << "    " << dataToSend[2] << endl;  
			for (vector<btSender>::iterator it = lBricks.begin(); it != lBricks.end(); it++){
				copyArr(dataToSend,  msg, msgElements);
				if (it->Send(msg, msgElements) == false) {
					cout << "Error: " << it->GetErrorID() << " Cannot send to port '" << it->GetComPortName() << "' " << it->GetErrorMessage();
				}
			}
			copyArr(dataToSend, dataToSendOld, msgElements);
		} else {
			cout << ".";
		}
		frmCount++;
		if (frmCount >= 50)
		{
			time(&end);
			fps = frmCount/difftime (end, start);
			frmCount = 0;
		}

		putText(imgOriginal, getColorFilterAsString(), Point(25, 25), FONT_HERSHEY_DUPLEX, 1.0, fontScalar, 2, CV_AA); 
		putText(imgOriginal, "g:" + to_string(chekGeometry), Point(25, 65), FONT_HERSHEY_DUPLEX, 1.0, fontScalar, 2, CV_AA); 
		putText(imgOriginal, "fps: " + to_string(fps), Point(25, 105), FONT_HERSHEY_DUPLEX, 1.0, fontScalar, 2, CV_AA);
		putText(imgOriginal, "b:" + to_string(brightness), Point(25, 145), FONT_HERSHEY_DUPLEX, 1.0, fontScalar, 2, CV_AA);

		imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original", imgOriginal); //show the original image

		key = waitKey(10);
		switch (key){
		case  27:
			for (vector<btSender>::iterator it = lBricks.begin(); it != lBricks.end(); it++){
				it->Disconnect();
			}
			cout << "esc key is pressed by user" << endl;
			return 0; 
			break;
		case 'r':			
			loadIncludes(include);
			getErAndDi(er1, di1, di2, er2);
			getSetings();
			break;
		case '1':
			colorFilter = colorFilter ^ (2 << 0);
			loadIncludes(include);
			break;
		case '2':
			colorFilter = colorFilter ^ (2 << 1);
			loadIncludes(include);
			break;
		case '3':
			colorFilter = colorFilter ^ (2 << 2);
			loadIncludes(include);
			break;
		case '0':
			colorFilter = 0;
			loadIncludes(include);
			break;
		case 'g':
			chekGeometry ^= true;
			cout << "chekGeometry: " << chekGeometry << endl;
		}
	}
	for (vector<btSender>::iterator it = lBricks.begin(); it != lBricks.end(); it++){
		it->Disconnect();
	}	
	return 0;
}

// approximate comparison
bool comparAppr(short *a1, short *a2, int size){
#ifndef FLOOD_PROTECT
	return false;
#endif // ! FLOOD_PROTECT
	if (abs(a1[0] - a2[0]) > 1) return false;
	if (abs(a1[1] - a2[1]) > 1) return false;
	return (a1[2] == a2[2]);
}

// accurate comparison
bool compar(short *a1, short *a2, int size){
#ifndef FLOOD_PROTECT
	return false;
#endif // ! FLOOD_PROTECT
	for(int i = 0; i < size; i++){
		if(a1[i] != a2[i]) return false;
	}
	return true;
}

void zeroArray(short *a, int size){
	for(int i = 0; i < size; i++){
		a[i] = 0;
	}
}

void copyArr(short *a1, short *a2, int size){
	for(int i = 0; i < size; i++){
		a2[i] = a1[i];
	}
}

void builtTreshHold(Mat &src, Mat &thd, vector<ScalarRange> &include){
	if(include.size() == 0) thd = Mat(Scalar(0)).clone();
	//Mat dst;
	for(vector<ScalarRange>::iterator it = include.begin(); it != include.end(); it++){
		inRange(src, it->lowBound, it->upBound, dst); //
		if(it == include.begin())
			thd = dst.clone();
		else 
			bitwise_or(dst, thd, thd);
	}
}

void getIncludeScalar(vector<ScalarRange> &scRange, string inFile){
	string line;
	ifstream file(inFile);
	int lR, lB, lG,
		uR, uB, uG;
	while(getline(file, line)){
		if((line.length() > 6) && (line[0] != '/')){
			istringstream sStream(line);
			sStream >> lB >> lG >> lR;
			sStream >> uB >> uG >> uR;
			cout << "  + color: (" << lB << ' ' << lG << ' ' << lR << ") (" << uB << ' ' << uG << ' ' << uR << ')' << endl;  
			scRange.push_back(ScalarRange(Scalar(lB, lG, lR), Scalar(uB, uG, uR)));
		}
	}
}

void loadIncludes(vector<ScalarRange> &scRange){
	scRange.clear();
	map<color, string> ::iterator includeNameOfFileIterator;
	for(includeNameOfFileIterator = includeNameOfFile.begin(); includeNameOfFileIterator != includeNameOfFile.end(); includeNameOfFileIterator++){
		if(((int)includeNameOfFileIterator->first & colorFilter) == (int)includeNameOfFileIterator->first){
			cout << "+ " << includeNameOfFileIterator->second << endl;
			getIncludeScalar(scRange,includeNameOfFileIterator->second);
		}
		else
			cout << "- " << includeNameOfFileIterator->second << endl; 
	}
}

void initInclude(){
	includeNameOfFile.insert(make_pair(red, "includeRed.txt"));
	includeNameOfFile.insert(make_pair(yellow, "includeYellow.txt"));
	includeNameOfFile.insert(make_pair(green, "includeGreen.txt"));
}

void getErAndDi(Size &Er1, Size &Di1, Size &Di2, Size &Er2){
	string line;
	int i =0;
	int k =0;
	Size r;
	int ic = 1;
	ifstream file("erodeAndDilate.txt");
	while(getline(file, line)){
		if((line.length() > 3) && (line[0] != '/')){
			istringstream sStream(line);
			sStream >> i >> k;

			if(ic == 1){
				Er1.height = i; 
				Er1.width = k;
			}
			else if(ic == 4){
				Er2.height = i;
				Er2.width = k;
			}
			else if(ic == 2){
				Di1.height = i;
				Di1.width = k;
			}
			else if(ic == 3){
				Di2.height = i;
				Di2.width = k;
			}
			ic++;
		}
	}
	cout << "noise: " << Er1.height << 'x' << Er1.width << ' ' << Di1.height << 'x' << Di1.width << endl;
	cout << "hole:  " << Di2.height << 'x' << Di2.width << ' ' << Er2.height << 'x' << Er2.width << endl;
}

void geometry(Mat &imgThreshold){
	rectangle(imgThreshold,Point(0, 0), Point(imgThreshold.cols - 1, imgThreshold.rows -1), Scalar(0));
	Mat oCanny;
	Mat imgDrawing(imgThreshold.size(), CV_8UC1, Scalar(0));
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours; 
	Canny(imgThreshold, oCanny, 100, 300, 3);
	findContours(oCanny, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	for(int i = 0; i  < contours.size(); i++){
		RotatedRect rr = minAreaRect(Mat(contours[i]));
		RotatedRect re;
		if(contours[i].size() > 6) re = fitEllipse(Mat(contours[i]));
		double aspectRatioRR = 0; // Rectangle
		double aspectRatioRE = 0; // Ellipse
		if((rr.size.height != 0) && (rr.size.width != 0)){
			aspectRatioRR = rr.size.height / rr.size.width;
			if(aspectRatioRR < 0) aspectRatioRR = rr.size.width / rr.size.height;
			if((aspectRatioRR > 1.75) && (rr.angle > -30 ) && (rr.angle < 30)) {
				drawContours( imgDrawing, contours, i, Scalar(255, 255, 255), CV_FILLED );
				continue;
			}

			if((re.size.height != 0) && (re.size.width != 0)){
				aspectRatioRE = re.size.height / re.size.width;
				if(aspectRatioRE < 0) aspectRatioRE = re.size.width / re.size.height;
				if((aspectRatioRE > 1.75)  && (re.angle > -30 ) && (re.angle < 30)){	
					drawContours( imgDrawing, contours, i, Scalar(255, 255, 255), CV_FILLED );
					continue;
				}
			}
			if(getBrokenLine(contours[i]) < points)//------------------------------------------------------------------------------------------------------
				drawContours( imgDrawing, contours, i, Scalar(255, 255, 255), CV_FILLED );
		}
	}
	imshow("imgDrawing", imgDrawing);
	bitwise_xor(imgThreshold, imgDrawing, imgThreshold);
}

int getBrokenLine(OutputArrayOfArrays curve){
	int perimeter = arcLength(curve, true);
	vector<Point> appCurve;
	approxPolyDP(curve, appCurve, perimeter * 0.005, true);
	return appCurve.size();
}

string getColorFilterAsString(){
	string filter = "";
	if((colorFilter & red) == red) filter += "r";
	if((colorFilter & yellow) == yellow) filter += "y";
	if((colorFilter & green) == green) filter += "g";
	return filter;
}

void getSetings(){
	string line;
	ifstream file("getSetings.txt");
	int i =0;
	while(getline(file, line)){
		if((line.length() > 0) && (line[0] != '/')){
			istringstream sStream(line);
			if(i == 0){
				sStream >> points;
				cout << "points: "<< points << endl;  
			}
			else{
				sStream >> minMass;
				cout <<"minMass: "<< minMass<<endl;  
			}
		i++;
		}
	}
}