
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
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

	colorPattern cPattern = colorPattern(color(0,0,0), color(50,70,100));
	color newColor = color(100,0,0);

    Mat_<Vec3b> image;

	VideoCapture cap(0);
	
	vector<vector<int>> used;

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