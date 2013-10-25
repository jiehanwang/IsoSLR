#pragma once
#include "globalDefine.h"
#include "S_TrajectoryRecognition.h"
#include <algorithm>
using namespace std;

struct RANK
{
	int signID;
	double score;
};

class CI_match
{
public:
	CI_match(void);
	~CI_match(void);
public:
	ofstream outfile_detail;
	int      testFlag[Word_num];
	
	double states_similar(State myState1, State myState2, 
		float postureMatrix[][maxClassNum][maxClassNum]);
	int run(int galleryKeyFrameNo[], State galleryState[][MaxCombine], 
		double galleryTranfer[][MaxCombine+2][MaxCombine+2],
		float postureMatrix[][maxClassNum][maxClassNum],
		vector<State> probeState);
	double maxSimilar(CvPoint p0, CvPoint p1, double** likeli, double preMaxV);
	double maxSimilarSum(CvPoint p0, CvPoint p1, double** likeli, int &pairNum);
	void initial(int signIndex);
	void release(void);
	void readInMask(CString maskRoute);
	static bool comparison(RANK a,RANK b);
	int run_single(int galleryKeyFrameNo[], State galleryState[][MaxCombine], 
		double galleryTranfer[][MaxCombine+2][MaxCombine+2],
		float postureMatrix[][maxClassNum][maxClassNum],
		vector<State> probeState);
};

