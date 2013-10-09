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

void CI_probe::ReadDataFromGallery(CString route)
{
	int i, j, k, galleryIndex, m;
	int* Label_sequence;     //Original label sequence.
	int* Label_sequence_locate;
	double* p_gallery;           //Gallery HOG

	int labelSize = GalleryNum*Word_num*LRB;  //Label_size=5*370*3
	Label_sequence = new int[labelSize];
	Label_sequence_locate = new int[labelSize];

	ifstream infile1;
	infile1.open(route+"\\Gallery_Label.dat",ios::binary);
	infile1.read( (char *)Label_sequence, labelSize*sizeof(int) );//将Gallery_Label中的数据读到数组Label_sequence1中
	infile1.close();

	int keyFrameIntotal = 0;
	*(Label_sequence_locate+0) = *(Label_sequence + 0);
	for(i=0;i<labelSize;i++)
	{
		keyFrameIntotal += *(Label_sequence + i);
		if (i>0)
		{
			*(Label_sequence_locate+i) = *(Label_sequence_locate+i-1) + *(Label_sequence + i);
		}
	}
	cout<<"Label has been read into memory"<<endl;
	int HOGsize=keyFrameIntotal * HOG_dimension;//HOG_size
	p_gallery=new double[HOGsize];                         //p_gallery

	ifstream infile2;
	infile2.open(route+"\\Gallery_Data.dat",ios::binary);
	infile2.read((char*)p_gallery,HOGsize * sizeof(double));
	infile2.close();
	cout<<"Gallery has been read into the memory"<<endl;

	for (galleryIndex = 0; galleryIndex<GalleryNum; galleryIndex++)
	{
		for(i=0; i<Word_num; i++)                             //Posture
		{
			for(j=0;j<LRB;j++)                                         //Left, right, both
			{
				ikeyFrameNo[galleryIndex][i] = *(Label_sequence + galleryIndex*LRB*Word_num + i*LRB + j);
				int frameLocation;
				if (galleryIndex == 0 && i == 0 && j == 0)
				{
					frameLocation = 0;
				}
				else
				{
					frameLocation = *(Label_sequence_locate + galleryIndex*LRB*Word_num + i*LRB + j-1);
				}

				for(k=0; k<ikeyFrameNo[galleryIndex][i]; k++)            //Key frame
				{
					double sumV = 0.0;
					for (m=0; m<HOG_dimension; m++)
					{
						sumV += *(p_gallery + HOG_dimension*(frameLocation+k) + m);
						HOG_LRB[galleryIndex][i][j][k].push_back(*(p_gallery + HOG_dimension*(frameLocation+k) + m));
					}
					if (sumV == 0)
					{
						indicator[galleryIndex][i][k][j] = 0;
					}
					else
					{
						indicator[galleryIndex][i][k][j] = 1;
					}
				}
			}
		}
		cout<<"Gallery "<<galleryIndex<<" has been read into array"<<endl;
	}



	delete[] p_gallery;
	delete[] Label_sequence;
}

void CI_probe::labelPosture(                                      //To get the "label" in this function
	vector<double> HOG_LRB[][LRB][MaxKeyNo], 
	int            label[][MaxKeyNo][LRB],
	int			   classNum[], 
	int            keyFrameNo[], 
	int            indicator[][MaxKeyNo][LRB],
	float          postureC[][maxClassNum][HOG_dimension])
{
	for (int p=0; p<Word_num; p++)
	{
		int keyFrameNum = keyFrameNo[p];
		for (int k=0; k<keyFrameNum; k++)
		{
			for (int lrb=0; lrb<LRB; lrb++)
			{
				int classNum_in = classNum[lrb];
				int maxClass = 0;
				float maxSimilar = 1000.0;
				int indicator_in = indicator[p][k][lrb];
				if (indicator_in == 1)
				{
					for (int c=0; c<classNum_in; c++)
					{
						vector<double> classCenter;
						for (int i=0; i<HOG_dimension; i++)
						{
							classCenter.push_back(postureC[lrb][c][i]);
						}
						//float temp = Histogram(classCenter, HOG_LRB[p][lrb][k]);
						float temp = Histogram_minD(classCenter, HOG_LRB[p][lrb][k]);

						if (maxSimilar > temp)
						{
							maxSimilar = temp;
							maxClass = c;
						}
					}
					label[p][k][lrb] = maxClass;
				}
				else
				{
					label[p][k][lrb] = -1;  //-1 means no images. Actually, it will not be visited under the control of "indicator".
				}

			}
		}
	}
}

double CI_probe::Histogram_minD(vector<double>vec1,vector<double>vec2)
{
	double mat_score=0.0;
	int i;
	int _Size=vec1.size();
	for(i=0;i<_Size;i++)
	{
		mat_score+=(vec1[i]-vec2[i])*(vec1[i]-vec2[i]);
	}
	return  sqrt(mat_score);
}

int CI_probe::generateProbeStateFromDat(int classNum[],float postureC[][maxClassNum][HOG_dimension])
{
	//Label the posture.
	cout<<"Label the posture..."<<endl;
	//for (int i=0; i<GalleryNum; i++)
	int i = testGallery;
	{
		cout<<"Sample: "<<i<<endl;
		labelPosture(HOG_LRB[i], label[i], classNum, ikeyFrameNo[i], indicator[i], postureC);
	}

	//Generate the state. Memory "myState[GalleryNum][Word_num][MaxKeyNo]". 
	cout<<"Generate the state..."<<endl;
	//for (int g=0; g<GalleryNum; g++)
	int g = testGallery;
	{
		for (int w=0; w<Word_num; w++)
		{
			//cout<<ikeyFrameNo[g][w]<<endl;
			stateGenerate(ikeyFrameNo[g][w],label[g][w],indicator[g][w],myState[g][w]);
		}
	}

	return 0;
}

void CI_probe::stateGenerate(int keyFrameNo, int label[][LRB],int indicator[][LRB], State myState[])
{
	for (int i=0; i<keyFrameNo; i++)
	{
		myState[i].l = indicator[i][0];
		myState[i].r = indicator[i][1];
		myState[i].b = indicator[i][2];

		myState[i].L = label[i][0];
		myState[i].R = label[i][1];
		myState[i].B = label[i][2];
	}
}