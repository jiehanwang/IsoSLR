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

void CI_probe::ReadshapeDataFromGallery(CString route)
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
			stateGenerate(ikeyFrameNo[g][w],label[g][w],indicator[g][w],myState[g][w],myTra[g][w]);
		}
	}

	return 0;
}

void CI_probe::stateGenerate(int keyFrameNo, int label[][LRB],int indicator[][LRB], State myState[], Tra myTra)
{
// 	for (int i=0; i<keyFrameNo; i++)
// 	{
// 		myState[i].l = indicator[i][0];
// 		myState[i].r = indicator[i][1];
// 		myState[i].b = indicator[i][2];
// 
// 		myState[i].L = label[i][0];
// 		myState[i].R = label[i][1];
// 		myState[i].B = label[i][2];
// 	}
	//cout<<"keyFrameNo: "<<keyFrameNo<<" myTra: "<<myTra.segNum<<endl;
	for (int i=0; i<keyFrameNo; i++)
	{
		myState[i].l = indicator[i][0];
		myState[i].r = indicator[i][1];
		myState[i].b = indicator[i][2];

		myState[i].L = label[i][0];
		myState[i].R = label[i][1];
		myState[i].B = label[i][2];

		myState[i].Head.x = myTra.hx;
		myState[i].Head.y = myTra.hy;
		myState[i].Head.z = myTra.hz;

		//const int traNumDes = 200;
#ifdef useTra
		CvPoint3D32f leftHand[traNumDes];
		CvPoint3D32f righHand[traNumDes];
		int traNumsrc = myTra.frameNum;

		traNormalize(myTra, traNumDes, leftHand, righHand);

		for (int j=0; j<traNumDes; j++)
		{
			//cout<<j<<" "<<leftHand[j].x<<" "<<leftHand[j].y<<" "<<leftHand[j].z<<", "
			//<<righHand[j].x<<" "<<righHand[j].y<<" "<<righHand[j].z<<endl;
			myState[i].TL.push_back(leftHand[j]);
			myState[i].TR.push_back(righHand[j]);
		}
#endif

	}
}

void CI_probe::ReadTrajectoryFromDat(CString route, Tra myTra[])
{
	//////////////////////////////////////////////////////////////////////////
	//Read the head position
	int* headPosition;
	int headPosition_size = Word_num*3; //3 is for the x,y,z ordinate.
	headPosition = new int[headPosition_size];
	ifstream infile_headPosition;
	infile_headPosition.open(route+"\\HeadPosition.dat",ios::binary);
	infile_headPosition.read((char*)headPosition, headPosition_size*sizeof(int));//将Gallery_Label中的数据读到数组Label_sequence1中
	infile_headPosition.close();

	for (int i=0;i<Word_num; i++)
	{
		//cout<<"ID: "<<i<<*(headPosition + 3*i)<<" "<<*(headPosition + 3*i+1)<<" "<<*(headPosition + 3*i+2)<<endl;
		myTra[i].exist = 1;
		myTra[i].hx = *(headPosition + 3*i + 0);
		myTra[i].hy = *(headPosition + 3*i + 1);
		myTra[i].hz = *(headPosition + 3*i + 2);
	}
	delete[] headPosition;
	//////////////////////////////////////////////////////////////////////////
	//Read the label
	int* label;
	int label_size = Word_num * 20;   //At most 20 key frames in each sign.
	label = new int[label_size];
	ifstream infile_label;
	infile_label.open(route+"\\LabelForTra.dat",ios::binary);
	infile_label.read((char*)label,label_size*sizeof(int));

	int pointer_label = 0;
	for (int w=0; w<Word_num; w++)
	{
		myTra[w].signID = *(label + pointer_label + 0);
		myTra[w].frameNum = *(label + pointer_label + 1);
		myTra[w].segNum = *(label + pointer_label + 2);

		//cout<<myTra[w].signID<<" "<<myTra[w].frameNum<<" "<<myTra[w].segNum<<endl;
		for (int s=0; s<myTra[w].segNum; s++)
		{
			int frameID = *(label + pointer_label + 3 + s);
			myTra[w].segFrameID.push_back(frameID);
			//cout<<frameID<<" ";
		}
		//cout<<endl;
		pointer_label += 3+myTra[w].segNum;
	}
	delete[] label;
	//////////////////////////////////////////////////////////////////////////
	//Read the hand positions
	int* trajectory;
	int trajectory_size = Word_num * 6 * 500; //At most 500 frames average for each sign. 
	trajectory = new int[trajectory_size];
	ifstream infile_trajectory;
	infile_trajectory.open(route+"\\Trajectory.dat",ios::binary);
	infile_trajectory.read((char*)trajectory,trajectory_size*sizeof(int));

	int pointer_tra = 0;
	for (int w=0; w<Word_num; w++)
	{
		for (int s=0; s<myTra[w].frameNum; s++)
		{
			int lx = *(trajectory + pointer_tra + 0);
			int ly = *(trajectory + pointer_tra + 1);
			int lz = *(trajectory + pointer_tra + 2);
			int rx = *(trajectory + pointer_tra + 3);
			int ry = *(trajectory + pointer_tra + 4);
			int rz = *(trajectory + pointer_tra + 5);

			myTra[w].lx.push_back(lx);
			myTra[w].ly.push_back(ly);
			myTra[w].lz.push_back(lz);
			myTra[w].rx.push_back(rx);
			myTra[w].ry.push_back(ry);
			myTra[w].rz.push_back(rz);

			pointer_tra += 6;
		}
	}

	delete[] trajectory;
}

void CI_probe::ReadProbeGallery(void)
{
	CString root;
	for (int g=0; g<5; g++)
	{
		cout<<"Read trajectory data P5"<<g<<"..."<<endl;
		root.Format("..\\input\\20130925\\trajectory\\P5%d",g);
		ReadTrajectoryFromDat(root, myTra[g]);
	}

	cout<<"Read shape data..."<<endl;
	root="..\\input\\20130925";
	ReadshapeDataFromGallery(root);
}

void CI_probe::ReSample(float x[],float y[],float z[],int n,int m)//n: srcNodeNum. m: desNodeNum
{
	const int F = 512;
	double len=0.0;
	double D=0.0;
	double d=0.0;
	int k=1;
	float pointx[F];
	float pointy[F];
	float pointz[F];
	for (int i=0;i<n;i++)
	{
		pointx[i]=x[i];
		pointy[i]=y[i];
		pointz[i]=z[i];
	}

	for (int j=1;j<n;j++)
	{
		len+=sqrt((pointx[j]-pointx[j-1])*(pointx[j]-pointx[j-1])+(pointy[j]-pointy[j-1])*(pointy[j]-pointy[j-1])+(pointz[j]-pointz[j-1])*(pointz[j]-pointz[j-1]));	
	}
	double I=len/(m-1);

	if (/*I==0*/I<0.001)
	{
		for (int k=0;k<m;k++)
		{
			x[k]=pointx[0];
			y[k]=pointy[0];
			z[k]=pointz[0];
		}
	}
	else
	{
		for (int i=1;i<n;i++)
		{
			d=sqrt((pointx[i]-pointx[i-1])*(pointx[i]-pointx[i-1])+(pointy[i]-pointy[i-1])*(pointy[i]-pointy[i-1])+(pointz[i]-pointz[i-1])*(pointz[i]-pointz[i-1]));
			if(D+d>=I)
			{
				if (d!=0)//事实上，由于I!=0,d在这里不会等于0
				{
					x[k]=pointx[i-1]+((I-D)/d)*(pointx[i]-pointx[i-1]);
					y[k]=pointy[i-1]+((I-D)/d)*(pointy[i]-pointy[i-1]);
					z[k]=pointz[i-1]+((I-D)/d)*(pointz[i]-pointz[i-1]);
				}
				else//此时I==0，无需计算（之前对I==0的情况已经计算过了）
				{
					break;
				}
				pointx[i-1]=x[k];
				pointy[i-1]=y[k];
				pointz[i-1]=z[k];

				k++;
				i=i-1;
				D=0.0;
			}
			else
			{
				D=D+d;
			}
		}
	}

	//由于舍入的原因，D+d>=I的等号不能确保最后一个点被记录，因此要特地记录下来
	x[m-1]=pointx[n-1];
	y[m-1]=pointy[n-1];
	z[m-1]=pointz[n-1];
}

void CI_probe::traNormalize(Tra myTra, int nodeNumDes, CvPoint3D32f left[], CvPoint3D32f right[])
{
	int nodeNumSrc = myTra.frameNum;
	const int F = 512;
	float l_x[F];
	float l_y[F];
	float l_z[F];

	float r_x[F];
	float r_y[F];
	float r_z[F];

	for (int i=0; i<nodeNumSrc; i++)
	{
		l_x[i] = (float)myTra.lx[i];
		l_y[i] = (float)myTra.ly[i];
		l_z[i] = (float)myTra.lz[i];
		r_x[i] = (float)myTra.rx[i];
		r_y[i] = (float)myTra.ry[i];
		r_z[i] = (float)myTra.rz[i];
	}
	ReSample(l_x, l_y, l_z, nodeNumSrc, nodeNumDes);
	ReSample(r_x, r_y, r_z, nodeNumSrc, nodeNumDes);

	for (int i=0; i<nodeNumDes; i++)
	{
		left[i].x = l_x[i];
		left[i].y = l_y[i];
		left[i].z = l_z[i];

		right[i].x = r_x[i];
		right[i].y = r_y[i];
		right[i].z = r_z[i];
	}

}