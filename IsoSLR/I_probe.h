#pragma once
#include "S_Feature.h"
#include "globalDefine.h"
#include <string>
#include <atlstr.h>
#include <direct.h>
#include <fstream>
#define GalleryNum 5
#define MaxKeyNo 25
struct Std
{
	IplImage * pic_route;//Pointer to the key frame sequence.
	double std_distance; //Distance to the mean image.
};
class CI_probe
{
public:
	CI_probe(void);
	~CI_probe(void);
public:
	double         *keyFeatureStreamArr;//[feature_dimension];
	IplImage*      keyFrames;             //The best key frame.
	S_CFeature     P_myFeature;          //The object of CFeature class.
	vector<Std>    choose_pic;           //Store the best picture.
	int leftClass;
	int rightClass;
	int bothClass;

	IplImage* leftImage;
	IplImage* rightImage;
	IplImage* bothImage;
	//////////////////////////////////////////////////////////////////////////
	//vector<IplImage*> Route[GalleryNum][Word_num][LRB];   //Picture in file folder p50
	//vector<double> HOG[GalleryNum][Word_num][LRB][MaxKeyNo];    //HOG feature for each key frame. 3 channels are merged into 1.
	vector<double> HOG_LRB[GalleryNum][Word_num][LRB][MaxKeyNo];    //HOG label for each key frame, 3 channels.
	int            indicator[GalleryNum][Word_num][MaxKeyNo][LRB];//Indicate whether left, right, both images are existing. 
	int            label[GalleryNum][Word_num][MaxKeyNo][LRB];    //L, R, B's class label in each state.
	//State
	int            ikeyFrameNo[GalleryNum][Word_num];     //All the three channels have the same key frame number.
	State          myState[GalleryNum][Word_num][MaxKeyNo];         //This is the state before gallery generating.  
	//////////////////////////////////////////////////////////////////////////

public:
	//Select the best key frame from key frame candidates.
	void keyFrameSelect(IplImage** rightKeyFrames, IplImage** leftKeyFrames, IplImage** bothKeyFrames,
		int rightKeyNum, int leftKeyNum, int bothKeyNum,int frameIndexStart, int frameIndexEnd, int folderIndex);
	IplImage* keyFrameSelect_sub(IplImage** KeyFrames, int KeyNum);
	//Feature calculation for the best key frame
	void featureCalPosture(void);
	//
	double img_distance(IplImage* dst1, IplImage* dst2);
	IplImage* Resize(IplImage* _img);
	bool cmp(Std pp, Std qq);
	int bestFrame(vector<Std> choose_pic);
	void postureClassification(int classNum[], float postureC[][maxClassNum][HOG_dimension]);
	void saveFrames(int folderIndex, IplImage* image, int lrb, int frameIndexStart, int frameIndexEnd);
	void ReadDataFromGallery(CString route);
	void labelPosture(                                      //To get the "label" in this function
		vector<double> HOG_LRB[][LRB][MaxKeyNo], 
		int            label[][MaxKeyNo][LRB],
		int			   classNum[], 
		int            keyFrameNo[], 
		int            indicator[][MaxKeyNo][LRB],
		float          postureC[][maxClassNum][HOG_dimension]);
	double Histogram_minD(vector<double>vec1,vector<double>vec2);
	int generateProbeStateFromDat(int classNum[],float postureC[][maxClassNum][HOG_dimension]);
	void stateGenerate(int keyFrameNo, int label[][LRB],int indicator[][LRB], State myState[]);
};

