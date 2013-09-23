// IsoSLR.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include<fstream>
#include <opencv2\opencv.hpp>
#include <vector>
#include "I_galleryCreate.h"
#include "Readvideo.h"
#include "S_Keyframe.h"
#include "I_probe.h"
using namespace std;
using namespace cv;

CI_galleryCreate myGallery;
S_Keyframe       myKeyframe;
CI_probe         myProbe;

struct RECVPARAM
{
	S_Keyframe   douKeyframe;
	int a;
};
static DWORD WINAPI RecvProc(LPVOID lpParameter);

int _tmain(int argc, _TCHAR* argv[])
{
	SLR_ST_Skeleton skeletonCurrent;    //The 3 current data.
	Mat             depthCurrent;
	IplImage        *frameCurrent;
	CvPoint3D32f    headPoint;
	vector<State>   myState;

	CString route = "..\\input";
	myGallery.readGallery(route);

	Readvideo myReadVideo;
	int sentenceIndex = 50;
	CString         videoFileName;
	if (sentenceIndex<10)
	{
		videoFileName.Format("D:\\iData\\p08_01\\S08_000%d_1_0_20130412.oni",sentenceIndex);
	}
	else if (sentenceIndex<100)
	{
		videoFileName.Format("D:\\iData\\p08_01\\S08_00%d_1_0_20130412.oni",sentenceIndex);
	}
	else if (sentenceIndex<226)
	{
		videoFileName.Format("D:\\iData\\p08_01\\S08_0%d_1_0_20130412.oni",sentenceIndex);
	}
	else if (sentenceIndex<1000)
	{
		//videoFileName.Format("D:\\iData\\p08_01\\S08_0%d_1_0_20130919.oni",sentenceIndex);
		videoFileName.Format("D:\\iData\\ydd-s\\S05_0%d_5_0_20130919.oni",sentenceIndex);

	}

	string  s   =   (LPCTSTR)videoFileName;
	myReadVideo.readvideo(s);
	int framSize = myReadVideo.vColorData.size();


	//Dual-thread begin
	RECVPARAM       *pRecvParam;        //Handle and parameter for dual-thread.
	DWORD           thredID;
	HANDLE          hThread;
	pRecvParam = new RECVPARAM;
	hThread = CreateThread(NULL, 0, RecvProc, (LPVOID)pRecvParam, 0, &thredID);
	CloseHandle(hThread);
	if(hThread == NULL)
	{
		DWORD dwError = GetLastError();
		cout<<"Error in Creating thread"<<dwError<<endl ;
	}

	int maxY  = min<int>(myReadVideo.vSkeletonData[0]._2dPoint[7].y, myReadVideo.vSkeletonData[0]._2dPoint[11].y);
	myKeyframe.setHeightThres(maxY-20);
	for (int i=0; i<framSize; i++)
	{
		skeletonCurrent = myReadVideo.vSkeletonData[i];
		depthCurrent    = myReadVideo.vDepthData[i];
		frameCurrent    = myReadVideo.vColorData[i];
		headPoint       = myReadVideo.headPoint3D;
		myKeyframe.pushImageData(skeletonCurrent,depthCurrent,frameCurrent);  
		while(!myKeyframe.processOver());
		int detectNum = 0;
		if (myKeyframe.segmentOver == false || myKeyframe.isThereFragment())													 
		{																													     
			while(myKeyframe.isThereFragment())																					 
			{
				State tempState;
				//Get key frame candidates.																					 
				KeyFrameSegment tempSegment = myKeyframe.getFragment();															 
				cout<<endl<<"KeyFrame: Begin ID-"<<tempSegment.BeginFrameID														 
					<<" End ID-"<<tempSegment.EndFrameID<<endl;	

				//////////////////////////////////////////////////////////////////////////	
				//Select the best key frame form key frame candidates. 														 
				//"sentenceIndex" is used for save key frames. It should be deleted later.									 
				myProbe.keyFrameSelect(tempSegment.RightImages,tempSegment.LeftImages,tempSegment.BothImages,				 
					tempSegment.RightNum, tempSegment.LeftNum, tempSegment.BothNum,												 
					tempSegment.BeginFrameID, tempSegment.EndFrameID,sentenceIndex);											 

				//Compute its feature.																						 
				myProbe.featureCalPosture();
				tempState.l = tempSegment.LeftLabel;
				tempState.r = tempSegment.RightLabel;
				tempState.b = tempSegment.BothLabel;
// 				tempState.L = 
// 				tempState.R = 
// 				tempState.B = 
// 				tempState.PL = 
// 				tempState.PR = 
// 				tempState.TL = 
// 				tempState.TR = 
				myState.push_back(tempState);
				
				//Generate the states here. It is the output of myProbe. 
				myKeyframe.releaseFragment(tempSegment);	
			}																													 
		}
	}

	for (int i=0; i<myState.size(); i++)
	{
		cout<<myState[i].l<<" "<<myState[i].r<<" "<<myState[i].b<<endl;
	}

	myKeyframe.setGetDataOver(true);
	while(!myKeyframe.getSegmentOver());
	myKeyframe.releaseMemory();
	
	cvReleaseImage(&frameCurrent);
	cout<<"done"<<endl;
	getchar();
	return 0;
}

DWORD WINAPI RecvProc(LPVOID lpParameter)
{
	myKeyframe.KeyframeExtractionOnline();
	return 0;
}