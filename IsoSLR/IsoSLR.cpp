// IsoSLR.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include <opencv2\opencv.hpp>
#include "I_galleryCreate.h"
using namespace std;
using namespace cv;

CI_galleryCreate myGallery;

int _tmain(int argc, _TCHAR* argv[])
{
	
	CString route = "..\\input";
	myGallery.galleryReadFromDat(route);
	return 0;
}

