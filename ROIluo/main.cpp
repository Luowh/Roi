#include <stdio.h>  
#include <iostream>

#include <math.h>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

struct Params
{
	Mat src;
};

struct MainParams
{
	int choice;//0 is auto 1 is input
	string src;
	string ouput_src;
	int x;
	int y;
	int width;
	int hegiht;
};
void printHelp()
{
	cout << "用法：ROI [option]" << endl;
	cout << "选项：" << endl;
	cout << "\t --auto Program caculate the black border of the image and build a new image,input two params:image's path,output image's path" << endl;
	cout << "\t --input input six params:image's path,output image's path, x, y, width, height" << endl;
}

MainParams getChoice(char* arg_val[])
{
	MainParams back;
	if (strcmp(arg_val[1], "--auto") == 0)
	{
		back.choice = 0;
		back.src = arg_val[2];
		back.ouput_src = arg_val[3];
	}
	else
	{
		if (strcmp(arg_val[1], "--input") == 0)
		{
			back.choice = 1;
			back.src = arg_val[2];
			back.ouput_src = arg_val[3];
			back.x = atoi(arg_val[4]);
			back.x = atoi(arg_val[5]);
			back.x = atoi(arg_val[6]);
			back.x = atoi(arg_val[7]);

		}
		else
		{
			printHelp();
		}
	}
	return back;
}

void autoCutImage(MainParams arg_main_params)
{
	Mat source = imread(arg_main_params.src, CV_LOAD_IMAGE_COLOR), output;
	//将图片转换为灰度图
	Mat gray;
	cvtColor(source, gray, CV_BGR2GRAY);
	//将图片低于制定灰度的图像去掉 
	Mat binary,dst;
	threshold(gray, binary, 70, 255, cv::THRESH_BINARY);
	Mat morph;
	//获取图像中形状
	Mat element = getStructuringElement(MORPH_RECT, Size(20, 20));
	morphologyEx(binary, morph, MORPH_OPEN, element);
	
	//直接对二值化图进行查找外轮廓
	vector<vector<Point>> allContours, contours;
	vector<Vec4i> hierarchy;
	findContours(binary, allContours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (int i = 0; i < allContours.size(); i++)
	{
		double area = contourArea(allContours[i]);
		if (area > 190 * 190)
		{
			contours.push_back(allContours[i]);
		}
		//drawContours(source, allContours, i, Scalar(0, 255, 0), 1, 8, hierarchy);
	}
	if (contours.size() == 0)
	{
		cout << "x=0" << endl << "y=0" << endl;
		cout << "width=" << source.size().width << endl << "height=" << source.size().height << endl;
		return;
	}
	Point frame[4] = { contours[0][0], contours[0][0], contours[0][0], contours[0][0] };
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			Point p = contours[i][j];

			// 左上角
			frame[0].x = MIN(frame[0].x, p.x);
			frame[0].y = MIN(frame[0].y, p.y);

			// 右上角
			frame[1].x = MAX(frame[1].x, p.x);
			frame[1].y = MIN(frame[1].y, p.y);

			// 右下角
			frame[2].x = MAX(frame[2].x, p.x);
			frame[2].y = MAX(frame[2].y, p.y);

			// 左下角
			frame[3].x = MIN(frame[3].x, p.x);
			frame[3].y = MAX(frame[3].y, p.y);
		}
	}
	for (int i = 0; i < 4; i++)
	{
		Point p1 = frame[i];
		Point p2 = frame[(i + 1) % 4];
		//line(source, p1, p2, Scalar(255, 0, 0));
		//cout << p1 << endl;
	}
	//cout << "x=" << frame[0].x << endl << "y=" << frame[0].y << endl;
	//cout << "width=" << frame[1].x - frame[0].x << endl;
	//cout << "height=" << frame[2].y - frame[0].y << endl;
	imshow("source",source);
	imshow("binary", binary);
	imshow("morph", morph);
	/*Rect rect(frame[0].x, frame[0].y, frame[1].x - frame[0].x, frame[2].y - frame[0].y);
	source(rect).copyTo(output);
	imwrite(arg_main_params.ouput_src, output);*/

}

//根据给定参数切割图片
void cutImage(MainParams arg_main_params)
{
	Mat src = imread(arg_main_params.src, CV_LOAD_IMAGE_COLOR),output;
	Rect rect(arg_main_params.x, arg_main_params.y, arg_main_params.width, arg_main_params.hegiht);
	src(rect).copyTo(output);;
	imwrite(arg_main_params.ouput_src, output);
}

int main(int argc, char* argv[])
{
	MainParams val = getChoice(argv);
	if (val.choice == 0)
	{
		autoCutImage(val);
	}
	else
	{
		cutImage(val);
	}
	waitKey(0);
	return 0;
}
