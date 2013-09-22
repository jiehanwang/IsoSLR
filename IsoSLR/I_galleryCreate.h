#pragma once
#include <opencv2\opencv.hpp>
//#include <iostream>
#include <fstream>
#include <string>
#include <atlstr.h>
#include "globalDefine.h"
using namespace std;
using namespace cv;

struct State
{
	int r;             //Indicator
	int l;
	int b;
	int R;             //Hand posture label
	int L;
	int B;
	CvPoint3D32f PL;   //Hand position
	CvPoint3D32f PR;  
	int TL;            //Hand trajectory label
	int TR;
	double frequency;  //Frequency of this state in the combined gallery.
	// 	int previous;      //-1 is the start.
	// 	int next;          //-2 is the end. 
};
class CI_galleryCreate
{
private:
	int           keyFrameNo[Word_num];
	State         myState[Word_num][MaxCombine];         //This is the state before gallery generating.
	double        tranfer[Word_num][MaxCombine+2][MaxCombine+2]; 
public:
	CI_galleryCreate(void);
	~CI_galleryCreate(void);
	void galleryReadFromDat(CString route);
};

