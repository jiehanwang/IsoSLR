#include "StdAfx.h"
#include "S_Feature.h"


S_CFeature::S_CFeature(void)
{
}


S_CFeature::~S_CFeature(void)
{
}


bool S_CFeature::GetHOGHistogram_Patch(IplImage* img, double hog_hist[])
{
	if (img == NULL)
	{
		return NULL;
	}
	else
	{
		//HOGDescriptor *hog=new HOGDescriptor(cvSize(SIZEs,SIZEs),cvSize(16,16),cvSize(8,8),cvSize(8,8),9);
		//(cvSize(SIZEs,SIZEs),cvSize(16,16),cvSize(8,8),cvSize(8,8),9)
		HOGDescriptor *hog=new HOGDescriptor(cvSize(SIZEs,SIZEs),cvSize(32,32),cvSize(16,16),cvSize(16,16),9); //dimension 324
		/////////////////////window: 64*64£¬block: 8*8£¬block step:4*4£¬cell: 4*4
		cvNormalize(img,img,255,0,CV_MINMAX,0); //Add by Hanjie Wang. 2013-03.
		//LBP(img,img);
		Mat handMat(img);

		vector<float> *descriptors = new std::vector<float>();

		hog->compute(handMat, *descriptors,Size(0,0), Size(0,0));
		////////////////////window: 0*0
		double total=0;
		int i;
		for(i=0;i<descriptors->size();i++)
			total+=abs((*descriptors)[i]);
		//	total=sqrt(total);
		for(i=0;i<descriptors->size();i++)
		{
			//hog_hist.push_back((*descriptors)[i]/total);
			hog_hist[i] = (*descriptors)[i]/total;
		}
			
		return true;
	}
	
}


double S_CFeature::Histogram(double vec1[], double vec2[])
{
	double mat_score=0.0;//mat_score: similarity
	int i;
	int _Size=HOG_dimension;
	for(i=0;i<_Size;i++)
	{
		mat_score+=vec1[i]<vec2[i] ? vec1[i] : vec2[i];
	}
	return  mat_score;
}
