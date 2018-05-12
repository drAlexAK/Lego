
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <queue>
#include <iostream>
#include "BTConnect.h"

using namespace cv;
using namespace std;

////
int Capture();
////

int main()
{
	Capture();
	return 0;
}

vector<string> GetListOfCOMPorts()
{
	vector<string> lCom;
	lCom.push_back("COM3");
	return lCom;
}

vector<bt_out> GetListOfBricks()
{
	vector<bt_out> lBricks;

	vector<string>  lCom =  GetListOfCOMPorts();
	for (uint i = 0; i < lCom.size(); i++)
	{
		while(true){		
			cout << "Connecting to '" << lCom.at(i) << "'" << endl;
			bt_out *brick = new bt_out(lCom.at(i));
			if (brick->IsItConnected())
			{
				cout << "Has been connected to '" << lCom.at(i) << "'" << endl; 
				lBricks.push_back(*brick); // if you push back an object vector will call the object destructor and handle will be closed after that
				break;
			}
			cout << "Error: " << brick->GetErrorID() << " " << brick->GetErrorMessage() << endl;
			Sleep(1000); 
		} 
	}
	return lBricks;
}

int Capture()
{
	short * dataToSend = new short[3];
	vector<bt_out> lBricks = GetListOfBricks();

	VideoCapture cap(0); //capture the video from webcam

	if ( !cap.isOpened() )  // if not success, exit program
	{
		cout << "Cannot open the web cam" << endl;
		return -1;
	}

	//Capture a temporary image from the camera
	Mat imgTmp;
	cap.read(imgTmp); 
	int rowCenter = imgTmp.rows / 2;
	int colCenter = imgTmp.cols / 2;

	//Create a black image with the size as the camera output
	//Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;

	while (true)
	{
		Mat imgOriginal;
		bool bSuccess = cap.read(imgOriginal); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{
			cout << "Cannot read a frame from video stream" << endl;
			break;
		}

		// Mat imgHSV;
		// cvtColor(imgOriginal, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		Mat imgThresholded;
		Mat imgTreeTunk;

		inRange(imgOriginal, Scalar(0, 0, 0), Scalar(0, 60, 0), imgTreeTunk); // cut of tuee tunk
		Mat imgBlack(imgOriginal.size(), CV_8UC3, Scalar(0)); 
		imgBlack.copyTo(imgOriginal, imgTreeTunk);

		imgTreeTunk.release();
		imgBlack.release();

		//inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
		inRange(imgOriginal, Scalar(8, 0, 60), Scalar(90, 84, 255), imgThresholded); //Threshold the image

		//morphological opening (removes small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) );
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)) ); 

		//morphological closing (removes small holes from the foreground)
		dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) ); 
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)) );

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresholded);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		dataToSend[0] = 0;
		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 10000)
		{
			//calculate the position of the ball
			int posX = dM10 / dArea;
			int posY = dM01 / dArea;        

			if (posX >= 0 && posY >= 0)
			{
				dataToSend[1] = posX - colCenter;
				dataToSend[2] = posY - rowCenter;
				dataToSend[0] = 1;
			}
			else
			{
				dataToSend[0] = 0;
				dataToSend[1] = 0;

			}
		}

		for (uint i = 0; i < lBricks.size(); i++)	{
			if (lBricks.at(i).Send(dataToSend, 3) == false) {
				cout << "Error: " << lBricks.at(i).GetErrorID() << " Cannot send to port '" << lBricks.at(i).GetComPortName() << "' " << lBricks.at(i).GetErrorMessage() ;
			}
		}

		imshow("Thresholded Image", imgThresholded); //show the thresholded image
		imshow("Original", imgOriginal); //show the original image

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break; 
		}
		Sleep(100);
	}

	return 0;
}
