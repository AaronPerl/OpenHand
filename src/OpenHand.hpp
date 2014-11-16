#ifndef __OPENHAND_H__
#define __OPENHAND_H__

#include "opencv2/imgproc/imgproc.hpp"

// Takes in an array of ints representing
// the x- and y- values of fingers and the
// back of the hand on-screen.
void processFrame(int* data, cv::Mat& img);

#endif