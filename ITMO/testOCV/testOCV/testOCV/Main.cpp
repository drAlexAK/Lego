#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <queue>

using namespace cv;
using namespace std;

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
/// 
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
	return ((p.R >= pattern.min.R) && (p.R <= pattern.max.R) && 
	(p.G >= pattern.min.G) && (p.G <= pattern.max.G) && 
	(p.B >= pattern.min.B) && (p.B <= pattern.max.B));
}

void changeColor(Vec3b &p, const color &c){
	p[0] = c.B;
	p[1] = c.G;
	p[2] = c.R;
}

int main()
{

	colorPattern cPattern = colorPattern(color(0,0,0), color(30,50,20));
	color newColor =color(100,0,0);

    Mat_<Vec3b> image;

	VideoCapture cap(0);
	
	char c = cvWaitKey(20);

	while(c != 27)
	{
		cap >> image;

		for(int i =0; i < image.rows; i++)
		{
			for(int j = 0; j < image.cols; j++)
			{
				if(matchColor(color(image(i,j)),cPattern)) changeColor(image(i,j), newColor);
			}
		}

		imshow("test", image);

		c = cvWaitKey(100);
	}

    return 0;
}