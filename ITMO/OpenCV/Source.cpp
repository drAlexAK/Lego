#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <queue>
#include <iostream>

using namespace cv;
using namespace std;

int getCorners(OutputArrayOfArrays curve);

int main( int argc, char** argv ) {
	Mat imgOriginal= imread("1.png", IMREAD_COLOR); // Read the file
	Mat imgThresholded;

	namedWindow( "Original", WINDOW_AUTOSIZE ); // Create a window for display.
	imshow( "Original" , imgOriginal );         // Show our origianl image inside it.

	imgOriginal.copyTo(imgThresholded);

	int thresh = 100;
	Mat cannyOutput;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat imgDrawing (imgOriginal.size(), CV_8UC3, Scalar(255, 255, 255));
	RNG rng(12345);


	while (true) {
		char key = waitKey(100);
		switch (key){
		case  27:
			return 0; 
			break;
		case ' ':
			imgOriginal.copyTo(imgThresholded);
			break;
		case 'c':

			//imgThresholded.copyTo(imgDrawing);
			
			/// Detect edges using canny
			Canny( imgThresholded, cannyOutput, thresh, thresh*2, 3 );
			/// Find contours
			findContours( cannyOutput, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

			/// Draw contours

			for( int i = 0; i< contours.size(); i++ )
			{
				if (getCorners(contours[i]) < 10) {
					Scalar color = Scalar(255, 0, 0 );
					//drawContours( imgDrawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
					drawContours( imgDrawing, contours, i, 0, CV_FILLED  );
					bitwise_and(imgThresholded, imgDrawing, imgThresholded);
				}
			}

			/// Show in a window
			namedWindow( "Contours", CV_WINDOW_AUTOSIZE );
			imshow( "Contours", imgDrawing );
			imshow( "Threshholded", imgThresholded);                // Show our image inside it.
			break;
		case 'e':
			//  ðàçìûòèå
			erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(15, 15)) );
		case 'r':
			// ñæàòèå
			dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(15, 15)) ); 
		default:
			imshow( "Threshholded", imgThresholded);                // Show our image inside it.
			break;
		}
	}
	return 0;
}

int getCorners(OutputArrayOfArrays curve) {
	// Calculates a contour perimeter or a curve length.
	//curve – Input vector of 2D points, stored in std::vector or Mat.
	//closed – Flag indicating whether the curve is closed or not.	
	double perim = arcLength(curve, true);

	//Approximates a polygonal curve(s) with the specified precision.
	//approxPolyDP(InputArray curve, OutputArray approxCurve, double epsilon, bool closed)
	// Ramer–Douglas–Peucker algorithm
	/*
	Parameters:	
	curve – Input vector of a 2D point stored in:

	std::vector or Mat (C++ interface)
	approxCurve – Result of the approximation. The type should match the type of the input curve. In case of C interface the approximated curve is stored in the memory storage and pointer to it is returned.
	epsilon – Parameter specifying the approximation accuracy. This is the maximum distance between the original curve and its approximation.
	closed – If true, the approximated curve is closed (its first and last vertices are connected). Otherwise, it is not closed.
	header_size – Header size of the approximated curve. Normally, sizeof(CvContour) is used.
	storage – Memory storage where the approximated curve is stored.
	method – Contour approximation algorithm. Only CV_POLY_APPROX_DP is supported.
	recursive – Recursion flag. If it is non-zero and curve is CvSeq*, the function cvApproxPoly approximates all the contours accessible from curve by h_next and v_next links
	*/
	vector<Point> approxCurve;
	approxPolyDP(curve, approxCurve,   0.0001 * perim, true);
	return approxCurve.size();
}
