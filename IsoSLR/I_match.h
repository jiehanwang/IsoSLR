#pragma once
#include "globalDefine.h"
class CI_match
{
public:
	CI_match(void);
	~CI_match(void);
public:
	double states_similar(State myState1, State myState2, 
		float postureMatrix[][maxClassNum][maxClassNum]);
	int run(int galleryKeyFrameNo[], State galleryState[][MaxCombine], 
		double galleryTranfer[][MaxCombine+2][MaxCombine+2],
		float postureMatrix[][maxClassNum][maxClassNum],
		vector<State> probeState);
	double maxSimilar(CvPoint p0, CvPoint p1, double** likeli);
};

