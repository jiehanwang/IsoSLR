#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <atlstr.h>
#include <direct.h>
#include <list>
#include <map>
#include <hash_map>
#include <opencv2\opencv.hpp>
using namespace std;
//#define useTra
#define testGallery 2
#define Word_num 1000//370
#define MaxCombine 100
#define maxClassNum 2000
#define LRB 3
#define HOG_dimension 324
#define readProbeFromDat
#define EP 0.00001
const int traNumDes = 200;

#define ZHIHAOPC
//#define CURVE2D
#define CURVE3D

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
	CvPoint3D32f Head;  //For hand trajectory label
	vector<CvPoint3D32f> TL;            
	vector<CvPoint3D32f> TR;
	double frequency;  //Frequency of this state in the combined gallery.
	// 	int previous;      //-1 is the start.
	// 	int next;          //-2 is the end. 
};
//////////////////////////////////////////////////////////////////////////
const int K = 15; ///K for KNN
#define MaxKeyPostureNum 10
#define Posture_num_add 384
#define Theda  0.0000001
const int num=5;
const int dim=3;//3D
const int WordsNum=370;
const int probenum=0;
const double PI= 3.1415;
const double eps=0.0001;
const int F=1500;
const int rsnum=1000;
//////////////////////////////////////////////////////////////////////////
struct HandSegment
{
	int keyframe_num;
	int* keyframe_no;
	IplImage*** keyframe_pic;
	int** keyframe_ID;
	int** keyframe_coor;
};

struct KeyFrameSegment
{
	int BeginFrameID;		//关键片段开始帧号
	int EndFrameID;			//关键片段jie'shu帧号

	int BothLabel;			//0-无	1-有
	int LeftLabel;			//0-无	1-有
	int RightLabel;			//0-无	1-有

	int BothNum;			//双手图片数目
	int LeftNum;			//左手图片数目
	int RightNum;			//右手图片数目

	int* BothID;			//每张双手分割图片的帧号
	int* LeftID;			//每张左手分割图片的帧号
	int* RightID;			//每张右手分割图片的帧号

	int* BothCoor;			//每张双手分割图片左上角点坐标
	int* LeftCoor;			//每张左手分割图片左上角点坐标
	int* RightCoor;			//每张右手分割图片左上角点坐标

	IplImage** BothImages;	//双手图片
	IplImage** LeftImages;	//双手图片
	IplImage** RightImages;	//双手图片
};

enum HANDTYPE
{
	LEFT,
	RIGHT,
	BOTH,
};
//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct Result
/// @brief recognition result struct
//////////////////////////////////////////////////////////////////////////
struct Result
{
	double posturePro[5];  ///< probability of posture recognition result 未用
	int postureWord[5];    ///< index of posture recognition result  未用
	double curvePro[5];    ///< probability of curve recognition result  未用
	int curveWord[5];      ///< index of curve recognition result  未用
	double fusionPro[5];    ///< probability of curve recognition result
	int fusionWord[5];      ///< index of curve recognition result
};

//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct Score
/// @brief word index and score, used to sort by score
//////////////////////////////////////////////////////////////////////////
struct Score
{
	int index;    ///< index of recognition result
	double score; ///< probability of recognition result

	bool operator < (const Score& s) const
	{
		return score < s.score;
	}
	bool operator > (const Score& s) const
	{
		return score > s.score;
	}
};

//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct Posture
/// @brief score left and right hand posture, hand and wrist point
//////////////////////////////////////////////////////////////////////////
struct Posture
{
	IplImage *leftHandImg;    ///< left hand image
	IplImage *rightHandImg;   ///< right hand image
	CvPoint leftHandPt;       ///< left hand point
	CvPoint leftWristPt;      ///< left wrist point
	CvPoint rightHandPt;      ///< right hand point
	CvPoint rightWristPt;     ///< right wrist point

	Posture():leftHandImg(NULL),rightHandImg(NULL) {};
};

//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct ColorModel
/// @brief skin color model, mean and variance of cr cb
//////////////////////////////////////////////////////////////////////////
struct ColorModel
{
	double mean_cr;     ///< mean of cr
	double mean_cb;     ///< mean of cb
	double d_cr;        ///< variance of cr
	double d_cb;        ///< variance of cb
	ColorModel():mean_cr(0),mean_cb(0),d_cr(0),d_cb(0){};
};

//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct _Vector2i
/// @brief skeleton data transformed data
//////////////////////////////////////////////////////////////////////////
struct _Vector2i
{
	int x;
	int y;
};

//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct SLR_ST_Skeleton
/// @brief skeleton data  real data
//////////////////////////////////////////////////////////////////////////
struct _Vector4f
{
	float x;
	float y;
	float z;
	float w;
};

//////////////////////////////////////////////////////////////////////////
/// @author xu zhihao
/// @struct SLR_ST_Skeleton
/// @brief skeleton data  real data and transformed data
//////////////////////////////////////////////////////////////////////////
struct SLR_ST_Skeleton
{
	_Vector4f _3dPoint[20];    ///< real point
	_Vector2i _2dPoint[20];    ///< pix in color image
}; 

struct Fragment{
	int fragmentCount;
	map<int, float> classWeightHit;
	multimap<float, int> weightHitClass;
	Fragment() : fragmentCount(0) {};
};
struct SignWordDescriptor{
	int id;
	vector<int> hand[2];
};

enum STAT_TYPE{
	E_AVER,   
	E_MIN,
	E_MEDIUM
};

struct KeyFrameUnit
{
	int StartFrame;
	int EndFrame;
};

typedef multimap<double, int, greater<double> > RecogValueIDPair;

#define SIZEs 64