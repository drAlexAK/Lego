
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <queue>
#include <iostream>

using namespace cv;
using namespace std;


////
int TestTracker();
////

struct color
{
	color()
	{
		R = G = B = 0;
	}
	color(int r, int g, int b)
	{
		R = r;
		G = g;
		B = b;
	}
	color(Vec3b p){
		R = p[2];
		G = p[1];
		B = p[0];
	}
	int R, G, B;
};

struct colorPattern 
{
	colorPattern(){
		min = max = color();
	}
	colorPattern(color min, color max){
		colorPattern::min = min;
		colorPattern::max = max;
	}

	color min;
	color max;
};

struct point
{
	point(){
		x = 1;
		y = 1;
	}
	point(int x, int y){
		point::x = x;
		point::y = y;
	}
	int x, y;
};

bool matchColor(const color &p, const colorPattern &pattern){
	return 
	((p.R >= pattern.min.R) && (p.R <= pattern.max.R) && 
	(p.G >= pattern.min.G) && (p.G <= pattern.max.G) && 
	(p.B >= pattern.min.B) && (p.B <= pattern.max.B));
}

void changeColor(Vec3b &p, const color &c){
	p[0] = c.B;
	p[1] = c.G;
	p[2] = c.R;
}

color point_to_color(Mat& img, point& point)
{
	color color;
	color.R = img.at<cv::Vec3b>(point.y - 1, point.x - 1)[2];
	color.G = img.at<cv::Vec3b>(point.y - 1, point.x - 1)[1];
	color.B = img.at<cv::Vec3b>(point.y - 1, point.x - 1)[0];
	return color;
}

void BFS(Mat& imge, point& start_point, vector<vector<int>>& comp, int comp_now, colorPattern pattern)
{
	queue<point> queue;
	queue.push(start_point);

	//cout << start_point//.x << " " << start_point.y << endl;

	while (!queue.empty()) 
	{
		point last_point = queue.front();
		queue.pop();
		color new_color;
		point new_point;
		if (comp[last_point.y][last_point.x + 1] == 0)
		{
			new_point.x = last_point.x + 1;
			new_point.y = last_point.y;
			new_color = point_to_color(imge, new_point);
			if (matchColor(new_color, pattern))
			{
				comp[new_point.y][new_point.x] = comp_now;
				queue.push(new_point);
			}
		}
		if (comp[last_point.y][last_point.x - 1] == 0)
		{
			new_point.x = last_point.x - 1;
			new_point.y = last_point.y;
			new_color = point_to_color(imge, new_point);
			if (matchColor(new_color, pattern))
			{
				comp[new_point.y][new_point.x] = comp_now;
				queue.push(new_point);
			}
		}
		if (comp[last_point.y + 1][last_point.x] == 0)// here is a problem <<<<---------------------------------------------------
		{
			new_point.x = last_point.x;
			new_point.y = last_point.y + 1;
			new_color = point_to_color(imge, new_point);
			if (matchColor(new_color, pattern))
			{
				comp[new_point.y][new_point.x] = comp_now;
				queue.push(new_point);
			}
		}
		if (comp[last_point.y - 1][last_point.x] == 0)
		{
			new_point.x = last_point.x;
			new_point.y = last_point.y - 1;
			new_color = point_to_color(imge, new_point);
			if (matchColor(new_color, pattern))
			{
				comp[new_point.y][new_point.x] = comp_now;
				queue.push(new_point);
			}
		}
	}
}

void comp_init(vector<vector<int>>& comp, int x, int y)
{
	vector<int> new_line1, new_line2;

	for (int i = 0; i < x + 2; i++)
	{
		new_line1.push_back(-1);
	}

	new_line2.push_back(-1);
	for (int i = 0; i < x; i++)
	{
		new_line2.push_back(0);
	}
	new_line2.push_back(-1);

	comp.push_back(new_line1);
	for (int j = 0; j < y; j++)
	{
		comp.push_back(new_line2);
	}
	comp.push_back(new_line1);
}

color getColorByPoint(Mat& img, point& point)
{
	color color;
	color.R = img.at<cv::Vec3b>(point.y - 1, point.x - 1)[2];
	color.G = img.at<cv::Vec3b>(point.y - 1, point.x - 1)[1];
	color.B = img.at<cv::Vec3b>(point.y - 1, point.x - 1)[0];
	return color;
}

int main()
{

	TestTracker();
	return 0;

	colorPattern cPattern = colorPattern(color(0,0,0), color(50,70,100));
	color newColor = color(100,0,0);

	vector<vector<int>> used;

    Mat_<Vec3b> image;

	VideoCapture cap(0);

	cap >> image;

	comp_init(used,image.cols,image.rows);	
	

	char c = cvWaitKey(20);

	int used_now = 1;

	while(c != 27)
	{
    cap >> image;	
		for(int i =1; i < image.rows +1; i++)
		{
			for(int j = 1; j < image.cols+1; j++)
			{
				if(used[i][j] == 0){
				color start_color;
				point start_point;
				start_point.x = j;
				start_point.y = i;
				start_color = point_to_color(image, start_point);

				BFS(image,start_point,used,used_now, cPattern);
					used_now += 1;
				}
			}
		}

		imshow("test", image);

		c = cvWaitKey(100);
	}

    return 0;
}

int TestTracker()
 {
 VideoCapture cap(0); //capture the video from webcam

    if ( !cap.isOpened() )  // if not success, exit program
    {
         cout << "Cannot open the web cam" << endl;
         return -1;
    }

    namedWindow("Control", CV_WINDOW_AUTOSIZE); //create a window called "Control"

  int iLowH = 170;
 int iHighH = 179;

  int iLowS = 150; 
 int iHighS = 255;

  int iLowV = 60;
 int iHighV = 255;

  //Create trackbars in "Control" window
 createTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
 createTrackbar("HighH", "Control", &iHighH, 179);

  createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
 createTrackbar("HighS", "Control", &iHighS, 255);

  createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
 createTrackbar("HighV", "Control", &iHighV, 255);

  int iLastX = -1; 
 int iLastY = -1;

  //Capture a temporary image from the camera
 Mat imgTmp;
 cap.read(imgTmp); 

  //Create a black image with the size as the camera output
 Mat imgLines = Mat::zeros( imgTmp.size(), CV_8UC3 );;
 

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

  //inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
   inRange(imgOriginal, Scalar(8, 0, 60), Scalar(90, 84, 255), imgThresholded); //Threshold the image
      
  //morphological opening (removes small objects from the foreground)
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) ); 

   //morphological closing (removes small holes from the foreground)
  dilate( imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) ); 
  erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(3, 3)) );

   //Calculate the moments of the thresholded image
  Moments oMoments = moments(imgThresholded);

   double dM01 = oMoments.m01;
  double dM10 = oMoments.m10;
  double dArea = oMoments.m00;

   // if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
  if (dArea > 10000)
  {
   //calculate the position of the ball
   int posX = dM10 / dArea;
   int posY = dM01 / dArea;        
        
   if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
   {
    //Draw a red line from the previous point to the current point
    //line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0,0,255), 2);
   }

    iLastX = posX;
   iLastY = posY;
  }

   imshow("Thresholded Image", imgThresholded); //show the thresholded image

   imgOriginal = imgOriginal + imgLines;
  imshow("Original", imgOriginal); //show the original image

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
       {
            cout << "esc key is pressed by user" << endl;
            break; 
       }
    }

   return 0;
}
