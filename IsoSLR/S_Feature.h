#pragma once
#include <vector>
#include <opencv2\opencv.hpp>
#include "globalDefine.h"
using namespace std;
using namespace cv;

class S_CFeature
{
public:
	S_CFeature(void);
	~S_CFeature(void);
	bool GetHOGHistogram_Patch(IplImage* img, double hog_hist[]);
	double Histogram(double vec1[], double vec2[]);
};

