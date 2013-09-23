#include "StdAfx.h"
#include "I_probe.h"


CI_probe::CI_probe(void)
{
	keyFrames = cvCreateImage(cvSize(SIZEs,SIZEs),8,1);
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


	IplImage* LRImage = cvCreateImage(cvSize(SIZEs*2,SIZEs),8,1);
	if (leftImage == NULL && rightImage!=NULL)
	{
		for (m=0;m<SIZEs;m++)
		{ 
			uchar* src_right = (uchar*)(rightImage->imageData + m*rightImage->widthStep);
			uchar* src_LR = (uchar*)(LRImage->imageData + m*LRImage->widthStep);
			for (n=0;n<SIZEs;n++)
			{
				src_LR[n] = 0;
			}
			for (n=SIZEs;n<2*SIZEs;n++)
			{
				src_LR[n] = src_right[n-SIZEs];
			}

		}
		LRImage = Resize(LRImage);
		cvCopy(LRImage,keyFrames);
	}
	else if (leftImage != NULL && rightImage==NULL)
	{
		for (m=0;m<SIZEs;m++)
		{ 
			uchar* src_left = (uchar*)(leftImage->imageData + m*leftImage->widthStep);
			uchar* src_LR = (uchar*)(LRImage->imageData + m*LRImage->widthStep);
			for (n=0;n<SIZEs;n++)
			{
				src_LR[n] = src_left[n];
			}
			for (n=SIZEs;n<2*SIZEs;n++)
			{
				src_LR[n] = 0;
			}

		}
		LRImage = Resize(LRImage);
		cvCopy(LRImage,keyFrames);
	}
	else if (leftImage != NULL && rightImage!=NULL)
	{
		for (m=0;m<SIZEs;m++)
		{ 
			uchar* src_left = (uchar*)(leftImage->imageData + m*leftImage->widthStep);
			uchar* src_right = (uchar*)(rightImage->imageData + m*rightImage->widthStep);
			uchar* src_LR = (uchar*)(LRImage->imageData + m*LRImage->widthStep);
			for (n=0;n<SIZEs;n++)
			{
				src_LR[n] = src_left[n];
			}
			for (n=SIZEs;n<2*SIZEs;n++)
			{
				src_LR[n] = src_right[n-SIZEs];
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
 		saveFrames(folderIndex,keyFrames,0,frameIndexStart, frameIndexEnd);
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
	IplImage *_dst=cvCreateImage(cvSize(SIZEs,SIZEs),_img->depth,_img->nChannels);
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
	IplImage *T_avg=cvCreateImage(cvSize(SIZEs,SIZEs),8,1);//Mean images

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
		for(m=0;m<SIZEs;m++)
		{
			qq=(uchar *)(T_avg->imageData+m*T_avg->widthStep);
			for(n=0;n<SIZEs;n++)
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

void CI_probe::postureClassification(int classNum[], float postureC[][maxClassNum][HOG_dimension])
{
	double *tempFeature = new double[HOG_dimension];;
	double minDis;
	int minLabel;
	//////////////////////////////////////////////////////////////////////////
	//Left hand
	memset(tempFeature, 0, sizeof(double)*HOG_dimension);
	if (leftImage!=NULL)
	{
		P_myFeature.GetHOGHistogram_Patch(leftImage,tempFeature);
		minDis = 10000;
		minLabel = 0;
		for (int i=0; i<classNum[0]; i++)
		{
			double sumD = 0;
			for (int h=0; h<HOG_dimension; h++)
			{
				sumD += pow((tempFeature[h] - postureC[0][i][h]),2);
			}
			if (sumD < minDis)
			{
				minDis = sumD;
				minLabel = i;
			}
		}
		leftClass = minLabel;
	}
	else
	{
		leftClass = -1;
	}
	
	//////////////////////////////////////////////////////////////////////////
	//Right hand
	memset(tempFeature, 0, sizeof(double)*HOG_dimension);
	if (rightImage!=NULL)
	{
		P_myFeature.GetHOGHistogram_Patch(rightImage,tempFeature);
		minDis = 10000;
		minLabel = 0;
		for (int i=0; i<classNum[1]; i++)
		{
			double sumD = 0;
			for (int h=0; h<HOG_dimension; h++)
			{
				sumD += pow((tempFeature[h] - postureC[1][i][h]),2);
			}
			if (sumD < minDis)
			{
				minDis = sumD;
				minLabel = i;
			}
		}
		rightClass = minLabel;
	}
	else
	{
		rightClass = -1;
	}
	
	//////////////////////////////////////////////////////////////////////////
	memset(tempFeature, 0, sizeof(double)*HOG_dimension);
	if (bothImage!=NULL)
	{
		P_myFeature.GetHOGHistogram_Patch(bothImage,tempFeature);
		minDis = 10000;
		minLabel = 0;
		for (int i=0; i<classNum[2]; i++)
		{
			double sumD = 0;
			for (int h=0; h<HOG_dimension; h++)
			{
				sumD += pow((tempFeature[h] - postureC[2][i][h]),2);
			}
			if (sumD < minDis)
			{
				minDis = sumD;
				minLabel = i;
			}
		}
		bothClass = minLabel;
	}
	else
	{
		bothClass = -1;
	}
	
	delete[] tempFeature;
}
void CI_probe::saveFrames(int folderIndex, IplImage* image, int lrb, int frameIndexStart, int frameIndexEnd)
{
	int i,j,k;
	CString s_filefolder;
	s_filefolder.Format("..\\output\\%d",folderIndex);
	_mkdir(s_filefolder);
	CString s_ImgFileName;
	s_ImgFileName.Format("..\\output\\%d\\LRB_%d_%d.jpg",folderIndex,frameIndexStart, frameIndexEnd);

	cvSaveImage(s_ImgFileName, image);
}