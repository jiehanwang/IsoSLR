#pragma once
#include "S_Feature.h"
#include "globalDefine.h"
#include <string>
#include <atlstr.h>
#include <direct.h>
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
};

