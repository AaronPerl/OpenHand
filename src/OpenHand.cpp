#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "OpenHand.hpp"

using namespace cv;
using namespace std;

int hues[] = 
{
	(140*255)/360,	(170*255)/360,	// Blue
	(5*255)/360,	(25*255)/360,	// Orange
	(100*255)/360,	(130*255)/360,	// Green
	(250*255)/360,	(5*255)/360,	// Red
	(40*255)/360,	(70*255)/360,	// Yellow
	(170*255)/360,	(245*255)/360	// Purple
};
int sats[] = 
{
	(80*255)/100,	(100*255)/100,	// Blue
	(80*255)/100,	(100*255)/100,	// Orange
	(60*255)/100,	(100*255)/100,	// Green
	(50*255)/100,	(100*255)/100,	// Red
	(60*255)/100,	(100*255)/100,	// Yellow
	(40*255)/100,	(100*255)/100	// Purple
};
int values[] = 
{
	(50*255)/100,	(100*255)/100,	// Blue
	(50*255)/100,	(100*255)/100,	// Orange
	(30*255)/100,	(100*255)/100,	// Green
	(35*255)/100,	(100*255)/100,	// Red
	(50*255)/100,	(100*255)/100,	// Yellow
	(50*255)/100,	(100*255)/100	// Purple
};

void processFrame(int* data, Mat& img)
{
	Mat imgHSV;
	cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	// For each color
	for(int i = 0; i < 6; i++)
	{
		Mat imgThresh;
		if(hues[i+i] < hues[(i+i) + 1])
		{
			inRange(imgHSV, Scalar(hues[i+i], sats[i+i], values[i+i]), Scalar(hues[i+i+1], sats[i+i+1], values[i+i+1]), imgThresh); //Threshold the image
			//if(i == 5)				
			//	imshow("Purple", imgThresh);
		}
		else
		{
			Mat imgThreshCopy = imgThresh;
			inRange(imgHSV, Scalar(hues[i+i], sats[i+i], values[i+i]), Scalar(255, sats[i+i+1], values[i+i+1]), imgThresh);		//Threshold the image
			inRange(imgHSV, Scalar(0, sats[i+i], values[i+i]), Scalar(hues[i+i+1], sats[i+i+1], values[i+i+1]), imgThreshCopy);	//Threshold the image
			imgThresh += imgThreshCopy;
		}
			
		//morphological opening (removes small objects from the foreground)
		erode(imgThresh, imgThresh, getStructuringElement(MORPH_ELLIPSE, Size(8,8)));
		dilate(imgThresh, imgThresh, getStructuringElement(MORPH_ELLIPSE, Size(8,8))); 

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresh, imgThresh, getStructuringElement(MORPH_ELLIPSE, Size(8,8))); 
		erode(imgThresh, imgThresh, getStructuringElement(MORPH_ELLIPSE, Size(8,8)));

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresh);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 4000)
		{
			//calculate the position of the ball
			data[i+i+i]		= dM10 / dArea;
			data[i+i+i+1]	= dM01 / dArea;
			data[i+i+i+2]	= dArea;		
		}
		else
		{
			data[i+i+i]		= -1;
			data[i+i+i+1]	= -1;
			data[i+i+i+2]	= 0;
		}
	}
}