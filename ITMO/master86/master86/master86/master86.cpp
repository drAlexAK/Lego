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


//#define CAMERA

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
void zeroArray(short *a1, int size);
void copyArr(short *a1, short *a2, int size);
void GetListOfBricks(vector<btSender> &lBricks);
vector<string> GetListOfCOMPorts();
void replaceColor(Mat &src, Mat &bkg, vector<ScalarRange> &exlude);
void builtTreshHold(Mat &src, Mat &thd, vector<ScalarRange> &inlude);
void getExcludeScalar(vector<ScalarRange> &scRange);
void getIncludeScalar(vector<ScalarRange> &scRange, string inFile);
void initInclude();
void loadIncludes(vector<ScalarRange> &scRange);
void getErAndDi(Size &Er1, Size &Di1, Size &Di2, Size &Er2);
int getBrokenLine(OutputArrayOfArrays curve);
void geometry(Mat &treshHold);
////



typedef enum color{
	none	= 0,
	red		= 2,
	yellow  = 4,
	green   = 8
}color;

map<color, string> includeNameOfFile;
int colorFilter = red | yellow; 

int main()
{
	Capture();
	return 0;
}

vector<string> GetListOfCOMPorts() 
{
	vector<string> lCom;

	lCom.push_back("COM13"); 
	lCom.push_back("COM3"); // COM3 - platform, COM13 - arm
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
	getErAndDi(er1, di1, di2, er2);
	vector<btSender> lBricks;
	Mat imgTmp;
	bool chekGeometry = true;

	//GetListOfBricks(lBricks);
#ifdef CAMERA
	VideoCapture cap(0); //capture the video from webcam

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

	Mat imgBlack(imgTmp.size(), CV_8UC3, Scalar(0));
	int rowCenter = imgTmp.rows / 2;
	int colCenter = imgTmp.cols / 2;
	vector<ScalarRange> exclude;
	vector<ScalarRange> include;

	getExcludeScalar(exclude);
	//	getIncludeScalar(include);
	initInclude();
	loadIncludes(include);

	while (true)
	{
		Mat imgOriginal;
#ifdef CAMERA
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}
#else
		imgOriginal= imread(".\\pictures\\red.png", IMREAD_COLOR); // Read the file
#endif
		Mat imgThresholded;

		GaussianBlur(imgOriginal, imgOriginal, Size(5, 5), 0, 0);

		replaceColor(imgOriginal, imgBlack, exclude);
		builtTreshHold(imgOriginal, imgThresholded, include);

		//inRange(imgOriginal, Scalar(8, 0, 60), Scalar(90, 84, 255), imgThresholded); //Threshold the image

		//morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, er1));
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, di1));

		//morphological closing (removes small holes from the foreground)
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, di2) ); 
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, er2) );

		if((colorFilter != none) && (chekGeometry)) geometry(imgThresholded);

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 5000000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;        

			circle(imgOriginal, Point(posX, posY), 20, Scalar(200, 100, 50), 5);

			if (posX >= 0 && posY >= 0)
			{
				dataToSend[0] = posY - rowCenter;
				dataToSend[1] = posX - colCenter;
				dataToSend[2] = 1;				
			}
			else
			{
				zeroArray(dataToSend, msgElements);
			}
		}
		else
		{
			zeroArray(dataToSend, msgElements);
		}

		if (!compar(dataToSend, dataToSendOld, 3)) {
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

		imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original", imgOriginal); //show the original image
		key = waitKey(100);
		switch (key){
		case  27:
			for (vector<btSender>::iterator it = lBricks.begin(); it != lBricks.end(); it++){
				it->Disconnect();
			}
			cout << "esc key is pressed by user" << endl;
			return 0; 
			break;
		case 'r':			
			getExcludeScalar(exclude);
			loadIncludes(include);
			getErAndDi(er1, di1, di2, er2);
			//getIncludeScalar(include);
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
		//Sleep(100);
	}
	for (vector<btSender>::iterator it = lBricks.begin(); it != lBricks.end(); it++){
		it->Disconnect();
	}	
	return 0;
}

bool compar(short *a1, short *a2, int size){
	//return false;
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

void replaceColor(Mat &src, Mat &bkg, vector<ScalarRange> &exclude){
	Mat dst;
	for(vector<ScalarRange>::iterator it = exclude.begin(); it != exclude.end(); it++){
		inRange(src, it->lowBound, it->upBound, dst); // cut of colors by vector
		bkg.copyTo(src, dst);
	}
}

void builtTreshHold(Mat &src, Mat &thd, vector<ScalarRange> &include){
	if(include.size() == 0) thd = Mat(Scalar(0)).clone();
	Mat dst;
	for(vector<ScalarRange>::iterator it = include.begin(); it != include.end(); it++){
		inRange(src, it->lowBound, it->upBound, dst); //
		if(it == include.begin())
			thd = dst.clone();
		else 
			bitwise_or(dst, thd, thd);
	}
}


void getExcludeScalar(vector<ScalarRange> &scRange){
	scRange.clear();
	const string exFile = "exclude.txt";
	string line;
	ifstream file(exFile);
	int lR, lB, lG,
		uR, uB, uG;
	while(getline(file, line)){
		if((line.length() > 6) && (line[0] != '/')){
			istringstream sStream(line);
			sStream >> lB >> lG >> lR;
			sStream >> uB >> uG >> uR;
			cout << "  - color: (" << lB << ' ' << lG << ' ' << lR << ") (" << uB << ' ' << uG << ' ' << uR << ')' << endl;  
			scRange.push_back(ScalarRange(Scalar(lB, lG, lR), Scalar(uB, uG, uR)));
		}
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
	ifstream file("erodeAndDilet.txt");
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
	Mat oCanny;
	Mat imgDrawing(imgThreshold.size(), CV_8UC3, Scalar(255, 255, 255));
	vector<Vec4i> hierarchy;
	vector<vector<Point>> contours; 
	Canny(imgThreshold, oCanny, 100, 300, 3);
	findContours(oCanny, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	int i = 0;
	for(vector<vector<Point>>::iterator it = contours.begin(); it != contours.end(); it++){
		if(getBrokenLine(*it) > 5) {
			//drawContours(imgThreshold, contours, -1, 128, CV_FILLED);
			drawContours( imgDrawing, contours, i, 0, CV_FILLED  );
			//bitwise_xor(imgThreshold, imgDrawing, imgThreshold);
			imshow("imgDrawing", imgDrawing); //show the original image
		}
		i++;
	}
}

int getBrokenLine(OutputArrayOfArrays curve){
	int perimeter = arcLength(curve, true);
	vector<Point> appCurve;
	approxPolyDP(curve, appCurve, perimeter * 0.01, true);
	return appCurve.size();
}