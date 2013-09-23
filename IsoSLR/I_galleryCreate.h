#pragma once
#include <opencv2\opencv.hpp>
//#include <iostream>
#include <fstream>
#include <string>
#include <atlstr.h>
#include "globalDefine.h"
using namespace std;
using namespace cv;


class CI_galleryCreate
{
public:
	int            classNum[LRB];  //The class number of left, right, both postures. 
	float          postureC[LRB][maxClassNum][HOG_dimension];
	float          postureMatrix[LRB][maxClassNum][maxClassNum];

	int           keyFrameNo[Word_num];
	State         myState[Word_num][MaxCombine];         //This is the state before gallery generating.
	double        tranfer[Word_num][MaxCombine+2][MaxCombine+2]; 
public:
	CI_galleryCreate(void);
	~CI_galleryCreate(void);
	void readstr(FILE *f,char *string);
	void readstr2(FILE *f,char *string);
	void readInPostureC(CString route, int lrb);
	void readInPostureMatrix(CString route, int lrb);
	void galleryReadFromDat(CString route);
	void readGallery(CString route);
};

