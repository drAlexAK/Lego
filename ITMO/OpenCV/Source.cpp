#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <queue>
#include <iostream>

using namespace cv;
using namespace std;

int main( int argc, char** argv ) {
	Mat imgOriginal= imread("1.png", IMREAD_COLOR); // Read the file
	Mat imgThresholded;

	namedWindow( "Original", WINDOW_AUTOSIZE ); // Create a window for display.
	imshow( "Original" , imgOriginal );         // Show our origianl image inside it.

	imgOriginal.copyTo(imgThresholded);

	while (true) {
		char key = waitKey(100);
		switch (key){
		case  27:
			return 0; 
			break;
		case ' ':
			imgOriginal.copyTo(imgThresholded);
			break;
		case 'e':
			//  размытие
			erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(15, 15)) );
		case 'r':
			// сжатие
			dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(15, 15)) ); 
		default:
			imshow( "Threshholded", imgThresholded);                // Show our image inside it.
			break;
		}
	}
	return 0;
}