#include "StdAfx.h"
#include "I_probe.h"


CI_probe::CI_probe(void)
{
	keyFrames = cvCreateImage(cvSize(SIZE,SIZE),8,1);
	keyFeatureStreamArr = new double[HOG_dimension];
}


CI_probe::~CI_probe(void)
{
	cvReleaseImage(&keyFrames);
	delete[] keyFeatureStreamArr;
}

void CI_probe::keyFrameSelect(IplImage** rightKeyFrames, IplImage** leftKeyFrames, IplImage** bothKeyFrames,
	int rightKeyNum, int leftKeyNum, int bothKeyNum,int frameIndexStart, int frameIndexEnd, int folderIndex)
{   
	int m, n;


	choose_pic.clear();
	leftImage = keyFrameSelect_sub(leftKeyFrames,leftKeyNum);
	choose_pic.clear();
	rightImage = keyFrameSelect_sub(rightKeyFrames,rightKeyNum);
	choose_pic.clear();
	bothImage = keyFrameSelect_sub(bothKeyFrames,bothKeyNum);


	IplImage* LRImage = cvCreateImage(cvSize(SIZE*2,SIZE),8,1);
	if (leftImage == NULL && rightImage!=NULL)
	{
		for (m=0;m<SIZE;m++)
		{ 
			uchar* src_right = (uchar*)(rightImage->imageData + m*rightImage->widthStep);
			uchar* src_LR = (uchar*)(LRImage->imageData + m*LRImage->widthStep);
			for (n=0;n<SIZE;n++)
			{
				src_LR[n] = 0;
			}
			for (n=SIZE;n<2*SIZE;n++)
			{
				src_LR[n] = src_right[n-SIZE];
			}

		}
		LRImage = Resize(LRImage);
		cvCopy(LRImage,keyFrames);
	}
	else if (leftImage != NULL && rightImage==NULL)
	{
		for (m=0;m<SIZE;m++)
		{ 
			uchar* src_left = (uchar*)(leftImage->imageData + m*leftImage->widthStep);
			uchar* src_LR = (uchar*)(LRImage->imageData + m*LRImage->widthStep);
			for (n=0;n<SIZE;n++)
			{
				src_LR[n] = src_left[n];
			}
			for (n=SIZE;n<2*SIZE;n++)
			{
				src_LR[n] = 0;
			}

		}
		LRImage = Resize(LRImage);
		cvCopy(LRImage,keyFrames);
	}
	else if (leftImage != NULL && rightImage!=NULL)
	{
		for (m=0;m<SIZE;m++)
		{ 
			uchar* src_left = (uchar*)(leftImage->imageData + m*leftImage->widthStep);
			uchar* src_right = (uchar*)(rightImage->imageData + m*rightImage->widthStep);
			uchar* src_LR = (uchar*)(LRImage->imageData + m*LRImage->widthStep);
			for (n=0;n<SIZE;n++)
			{
				src_LR[n] = src_left[n];
			}
			for (n=SIZE;n<2*SIZE;n++)
			{
				src_LR[n] = src_right[n-SIZE];
			}

		}
		LRImage = Resize(LRImage);
		cvCopy(LRImage,keyFrames);
	}
	else if (leftImage == NULL && rightImage==NULL)
	{
		cvCopy(bothImage, keyFrames);
	}

	cvReleaseImage(&LRImage);
	//Output key frames.
	//#ifdef saveFiles
// 	if (saveFiles_va == 1)
// 	{
// 		saveFrames(folderIndex,keyFrames,0,frameIndexStart, frameIndexEnd);
// 	}
	//#endif

}


void CI_probe::featureCalPosture(void)
{
	int i,j,k;

	memset(keyFeatureStreamArr, 0, sizeof(double)*HOG_dimension);

	//If key frames in this channel is empty, make keyFeatureStreamArr all 0.0;
	if (keyFrames == NULL)
	{
		for (j=0; j<HOG_dimension; j++)
		{
			keyFeatureStreamArr[j] = 0.0;
		}
	}
	else
	{
		P_myFeature.GetHOGHistogram_Patch(keyFrames,keyFeatureStreamArr);
	}


}


double CI_probe::img_distance(IplImage* dst1, IplImage* dst2)
{
	int i,j;
	uchar *ptr1;
	uchar *ptr2;

	double result=0.0;////////////
	for(i=0;i<dst1->height;i++)
	{
		ptr1=(uchar *)(dst1->imageData+i*dst1->widthStep);
		ptr2=(uchar *)(dst2->imageData+i*dst2->widthStep);

		for(j=0;j<dst1->width;j++)
			result+=(ptr1[j*dst1->nChannels]-ptr2[j*dst2->nChannels])*(ptr1[j*dst1->nChannels]-ptr2[j*dst2->nChannels]);
	}
	result=sqrt(result);
	return result;
}


IplImage* CI_probe::Resize(IplImage* _img)    //Memory leak?
{
	IplImage *_dst=cvCreateImage(cvSize(SIZE,SIZE),_img->depth,_img->nChannels);
	cvResize(_img,_dst);
	return _dst;
}


bool CI_probe::cmp(Std pp, Std qq)
{
	return pp.std_distance<qq.std_distance;
}


int CI_probe::bestFrame(vector<Std> choose_pic)
{
	int i;
	double min_dis = 100000;   //I think this is large enough.
	int chooseOne;
	for (i=0; i<choose_pic.size(); i++)
	{
		if (choose_pic[i].std_distance < min_dis)
		{
			min_dis = choose_pic[i].std_distance;
			chooseOne = i;
		}

	}
	return chooseOne;
}


IplImage* CI_probe::keyFrameSelect_sub(IplImage** KeyFrames, int KeyNum)
{
	if (KeyNum == 0)
	{
		return NULL;
	}
	int i,j,m,n,k;
	int Sum[105][105];//Store sum of gray level
	uchar *pp;
	uchar *qq;
	Std   p_std;
	IplImage *T_avg=cvCreateImage(cvSize(SIZE,SIZE),8,1);//Mean images

	memset(Sum,0,sizeof(Sum));
	for (i=0; i<KeyNum; i++)
	{
		*(KeyFrames+i) = Resize(*(KeyFrames+i));
		cvSmooth(*(KeyFrames+i),*(KeyFrames+i),CV_GAUSSIAN,5,3);
		for(m=0 ; m<(*(KeyFrames+i))->height ; m++)
		{
			pp=(uchar *)((*(KeyFrames+i))->imageData+m*(*(KeyFrames+i))->widthStep);
			for(n=0;n<(*(KeyFrames+i))->width;n++)
			{
				Sum[m][n]+=pp[n*(*(KeyFrames+i))->nChannels];
			}
		}
		p_std.pic_route = *(KeyFrames+i);
		choose_pic.push_back(p_std);
	}

	if(choose_pic.size() > 0)
	{
		for(m=0;m<SIZE;m++)
		{
			qq=(uchar *)(T_avg->imageData+m*T_avg->widthStep);
			for(n=0;n<SIZE;n++)
			{
				Sum[m][n]=Sum[m][n]/choose_pic.size();
				qq[n*T_avg->nChannels]=Sum[m][n];
			}
		}
		for(k=0;k<choose_pic.size();k++)
		{
			choose_pic[k].std_distance=img_distance(choose_pic[k].pic_route,T_avg);
		}

		cvReleaseImage(&T_avg);
		return choose_pic[bestFrame(choose_pic)].pic_route;
	}
	else
	{
		cvReleaseImage(&T_avg);
		return NULL;
	}

}