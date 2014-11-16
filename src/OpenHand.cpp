#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "OpenHand.hpp"

using namespace cv;
using namespace std;

int hues[] = 
{
	(5*255)/360,	(25*255)/360,		// Orange
	(100*255)/360,	(130*255)/360,		// Green
	(140*255)/360,	(170*255)/360,		// Blue
	(40*255)/360,	(70*255)/360,		// Yellow
	(190*255)/360,	(200*255)/360,		// -
	(250*255)/360,	(5*255)/360			// Red
};
int sats[] = 
{
	(80*255)/100,	(100*255)/100,
	(60*255)/100,	(100*255)/100,
	(80*255)/100,	(100*255)/100,
	(60*255)/100,	(100*255)/100,
	(50*255)/100,	(100*255)/100,
	(45*255)/100,	(100*255)/100
};
int values[] = 
{
	(50*255)/100,	(100*255)/100,
	(50*255)/100,	(100*255)/100,
	(50*255)/100,	(100*255)/100,
	(50*255)/100,	(100*255)/100,
	(50*255)/100,	(100*255)/100,
	(30*255)/100,	(100*255)/100
};

void processFrame(int* data, Mat& img)
{
	// For each color
	for(int i = 0; i < 6; i++)
	{
		Mat imgHSV;
		cvtColor(img, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
		Mat imgThresh;
		
		if(hues[i+i] < hues[(i+i) + 1])
		{
			inRange(imgHSV, Scalar(hues[i+i], sats[i+i], values[i+i]), Scalar(hues[i+i+1], sats[i+i+1], values[i+i+1]), imgThresh); //Threshold the image
			if (i == 0)
				imshow("Orange", imgThresh);
		}
		else
		{
			Mat imgThreshCopy = imgThresh;
			inRange(imgHSV, Scalar(hues[i+i], sats[i+i], values[i+i]), Scalar(255, sats[i+i+1], values[i+i+1]), imgThresh);		//Threshold the image
			inRange(imgHSV, Scalar(0, sats[i+i], values[i+i]), Scalar(hues[i+i+1], sats[i+i+1], values[i+i+1]), imgThreshCopy);	//Threshold the image
			imgThresh += imgThreshCopy;
		}
			
		//morphological opening (removes small objects from the foreground)
		erode(imgThresh, imgThresh, getStructuringElement(MORPH_ELLIPSE, Size(9,9)));
		dilate(imgThresh, imgThresh, getStructuringElement(MORPH_ELLIPSE, Size(9,9))); 

		//morphological closing (removes small holes from the foreground)
		dilate(imgThresh, imgThresh, getStructuringElement(MORPH_ELLIPSE, Size(9,9))); 
		erode(imgThresh, imgThresh, getStructuringElement(MORPH_ELLIPSE, Size(9,9)));

		//Calculate the moments of the thresholded image
		Moments oMoments = moments(imgThresh);

		double dM01 = oMoments.m01;
		double dM10 = oMoments.m10;
		double dArea = oMoments.m00;

		// if the area <= 10000, I consider that the there are no object in the image and it's because of the noise, the area is not zero 
		if (dArea > 7500)
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