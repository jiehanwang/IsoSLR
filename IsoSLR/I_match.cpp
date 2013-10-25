#include "StdAfx.h"
#include "I_match.h"


CI_match::CI_match(void)
{
	
}


CI_match::~CI_match(void)
{
}

double CI_match::states_similar(State myState1, State myState2, 
	float postureMatrix[][maxClassNum][maxClassNum])
{
	double similarity = 0.0;
	if (myState1.l == myState2.l 
		&& myState1.r == myState2.r
		&& myState1.b == myState2.b)
	{
		float disRe = 1.0; 
		int left1 = myState1.L<myState2.L?myState1.L:myState2.L;
		int left2 = myState1.L>myState2.L?myState1.L:myState2.L;

		int right1 = myState1.R<myState2.R?myState1.R:myState2.R;
		int right2 = myState1.R>myState2.R?myState1.R:myState2.R;
#ifdef useTra
		CvPoint3D32f head1;
		CvPoint3D32f head2;
		vector<CvPoint3D32f> leftHand1;
		vector<CvPoint3D32f> leftHand2;
		vector<CvPoint3D32f> rightHand1;
		vector<CvPoint3D32f> rightHand2;

		head1.x = myState1.Head.x;
		head1.y = myState1.Head.y;
		head1.z = myState1.Head.z;

		head2.x = myState2.Head.x;
		head2.y = myState2.Head.y;
		head2.z = myState2.Head.z;

		for (int i=0; i<traNumDes; i++)
		{
			CvPoint3D32f leftTemp1 = myState1.TL[i];
			CvPoint3D32f rightTemp1 = myState1.TR[i];
			CvPoint3D32f leftTemp2 = myState2.TL[i];
			CvPoint3D32f rightTemp2 = myState2.TR[i];
			leftHand1.push_back(leftTemp1);
			rightHand1.push_back(rightTemp1);
			leftHand2.push_back(leftTemp2);
			rightHand2.push_back(rightTemp2);
		}
		disRe = Sentence_match(head1,leftHand1,rightHand1,head2,leftHand2,rightHand2);
		disRe = 1/pow(2.7183,0.07*disRe);
#endif
		//if ()   //Position is used here.
		{
			if(myState1.l == 1 && myState1.r == 1)
			{
				

				similarity = sqrt(postureMatrix[0][left1][left2]*postureMatrix[1][right1][right2]);
			}
			else if (myState1.l == 1 && myState1.r == 0)
			{
				int left1 = myState1.L<myState2.L?myState1.L:myState2.L;
				int left2 = myState1.L>myState2.L?myState1.L:myState2.L;
				similarity = postureMatrix[0][left1][left2];
			}
			else if (myState1.l == 0 && myState1.r == 1)
			{
				int right1 = myState1.R<myState2.R?myState1.R:myState2.R;
				int right2 = myState1.R>myState2.R?myState1.R:myState2.R;
				similarity = postureMatrix[1][right1][right2];
			}
			else if (myState1.b == 1)
			{
				int both1 = myState1.B<myState2.B?myState1.B:myState2.B;
				int both2 = myState1.B>myState2.B?myState1.B:myState2.B;
				similarity = postureMatrix[2][both1][both2];
			}
		}

		similarity *= disRe;
	}


	return similarity;

}

int CI_match::run(int galleryKeyFrameNo[], State galleryState[][MaxCombine], 
	double galleryTranfer[][MaxCombine+2][MaxCombine+2],
	float postureMatrix[][maxClassNum][maxClassNum],
	vector<State> probeState)
{
	int keyNo_P = probeState.size();
	double wordScore[Word_num];
	int    wordRouteNo[Word_num];
	for (int w=0; w<Word_num; w++)
	{ 
		//if (testFlag[w] == 1)
		{
			wordScore[w] = 0;
			wordRouteNo[w] = 0;
			int keyNo_G = galleryKeyFrameNo[w];
			vector<int> myStack;
			myStack.push_back(0);

			int* posi = new int[keyNo_G+2];
			for (int k=0; k<keyNo_G+2; k++)
			{
				posi[k] = k;
			}
			while (myStack.size()>0)
			{
				int TopV = myStack[myStack.size()-1];
				for (int k=0; k<keyNo_G+2; k++)
				{
					if (galleryTranfer[w][TopV][k]>0 && k>posi[TopV])
					{
						myStack.push_back(k);
						int tempSe = -1;
						if (k == keyNo_G+1)
						{
							wordRouteNo[w]++;
							//////////////////////////////////////////////////////////////////////////
							int keyNo_G_ = myStack.size()-2;
							double** likeli;
							likeli = new double*[keyNo_G_];
							for (int l=0; l<keyNo_G_; l++)
							{
								likeli[l] = new double[keyNo_P];
							}
							for (int m=0; m<keyNo_G_; m++)
							{
								for (int n=0; n<keyNo_P; n++)
								{
									int galleryIndex = myStack[m+1]-1;
									likeli[m][n] = states_similar(galleryState[w][galleryIndex], probeState[n],postureMatrix);
									//cout<<galleryIndex<<" "<<galleryState[w][galleryIndex].B<<'\t';
								}
								//cout<<endl;
							}
							//
							CvPoint p0, p1;
							p0.x = 0;
							p0.y = 0;
							p1.x = keyNo_G_-1;
							p1.y = keyNo_P-1;

							double myMaxSimilar = maxSimilar(p0,p1,likeli,0);
							myMaxSimilar = pow(myMaxSimilar,(double)(1.0/(keyNo_G_+EP)));
							if (myMaxSimilar > wordScore[w])
							{
								wordScore[w] = myMaxSimilar;
							}

// 							int pairNum = 0;
// 							double myMaxSimilar = maxSimilarSum(p0,p1,likeli,pairNum);
// 							wordScore[w] += myMaxSimilar/(pairNum+EP);//sum them.
// 							//Probability is added here. 
// 							for (int l=0; l<keyNo_G_-1; l++)
// 							{
// 								int pre = myStack[l];
// 								int cur = myStack[l+1];
// 								if (cur < keyNo_G+1)
// 								{
// 									myMaxSimilar *= (galleryTranfer[w][pre][cur] * galleryState[w][cur].frequency);
// 								}
// 								else if (cur == keyNo_G+1)
// 								{
// 									myMaxSimilar *= galleryTranfer[w][pre][cur];
// 								}
// 							}
// 							//To sum them or to choose the maximum is a problem.
// 							myMaxSimilar /= (min(keyNo_G_,keyNo_P)+0.000001);
// 							cout<<"sqrt: "<<myMaxSimilar<<" "<<keyNo_G_<<" ";
// 
// 							for (int m=0; m<keyNo_G_; m++)
// 							{
// 								for (int n=0; n<keyNo_P; n++)
// 								{
// 									cout<<likeli[m][n];
// 								}
// 								cout<<endl;
// 							}
// 							cout<<"word ID: "<<w<<" Route No: "<<wordRouteNo[w]<<endl;
// 							cout<<"Pair Num: "<<pairNum<<" Gallery: "<<keyNo_G_<<" Probe: "<<keyNo_P<<endl;
// 							cout<<"myMaxSimilar"<<myMaxSimilar/(pairNum+EP)<<endl;
// 
// 							wordScore[w] -= abs(keyNo_G_-keyNo_P)*0.5;
// 							if (myMaxSimilar > 0)
// 							{
// 								wordRouteNo[w]++;
// 							}


							//cout<<"word: "<<w<<"myMaxSimilar: "<<myMaxSimilar<<endl;
							for (int l=0; l<keyNo_G_; l++)
							{
								delete[] likeli[l];
							}
							delete[] likeli;
							//////////////////////////////////////////////////////////////////////////
							tempSe = myStack[myStack.size()-2];
							myStack.pop_back();
							myStack.pop_back();
							k=0;
						}
						TopV = myStack[myStack.size()-1];
						posi[TopV] = tempSe;
					}
				}

				int tempSe2;
				if (myStack.size()>0)
				{
					tempSe2 = myStack[myStack.size()-1];
				}
				myStack.pop_back();
				if (myStack.size()>0)
				{
					TopV = myStack[myStack.size()-1];
					posi[TopV] = tempSe2;
				}
			}

			delete[] posi;
			//wordScore[w] /= (wordRouteNo[w]+EP);
			//cout<<"word: "<<w<<"S:-----"<<wordScore[w]<<endl;
		}
		
	}


	RANK myRank[Word_num];
	for (int w=0; w<Word_num; w++)
	{
		//if (testFlag[w] == 1)
		{
			outfile_detail<<w<<","<<wordScore[w]<<endl;
			myRank[w].signID = w;
			myRank[w].score = wordScore[w];
		}
		
	}

	sort(myRank,myRank+Word_num,comparison);

	return myRank[0].signID;
}

bool CI_match::comparison(RANK a,RANK b)
{
	return a.score > b.score;
}

double CI_match::maxSimilar(CvPoint p0, CvPoint p1, double** likeli, double preMaxV)
{
	if (p0.x == p1.x && p0.y == p1.y)
	{
//  		if (preMaxV != 0 && likeli[p0.x][p0.y] == 0)
//  		{
// 			return 1;
//  		}
//  		else
// 		{
			return likeli[p0.x][p0.y];
//		}

		
	}
	else
	{
		double maxV = 0.0;
		CvPoint maxP;
		for (int i=p0.x; i<=p1.x; i++)
		{
			for (int j=p0.y; j<=p1.y; j++)
			{
				if (likeli[i][j]>=maxV)
				{
					maxV = likeli[i][j];
					maxP.x = i;
					maxP.y = j;
				}
			}
		}
		
		CvPoint maxPPre;
		CvPoint maxPLat;
		maxPPre.x = maxP.x-1 > p0.x ? maxP.x-1 : p0.x; 
		maxPPre.y = maxP.y-1 > p0.y ? maxP.y-1 : p0.y;
		maxPLat.x = maxP.x+1 < p1.x ? maxP.x+1 : p1.x; 
		maxPLat.y = maxP.y+1 < p1.y ? maxP.y+1 : p1.y;
// 		cout<<maxP.x<<'\t'<<maxP.y<<endl;
// 		cout<<maxPPre.x<<'\t'<<maxPPre.y<<endl;
// 		cout<<maxPLat.x<<'\t'<<maxPLat.y<<endl;
// 		cout<<endl;
		double pre, lat;
		if (maxPPre.x == maxP.x || maxPPre.y == maxP.y)
		{
// 			if (preMaxV>0)
// 			{
// 				pre = 1;
// 			}
// 			else
// 			{
// 				pre = 0;
// 			}
			pre = maxSimilar(p0,maxPPre,likeli,maxV);//1
		}
		else
		{
			pre = maxSimilar(p0,maxPPre,likeli,maxV);
		}
		if (maxPLat.x == maxP.x || maxPLat.y == maxP.y)
		{
// 			if (preMaxV>0)
// 			{
// 				pre = 1;
// 			}
// 			else
// 			{
// 				pre = 0;
// 			}
			lat = maxSimilar(maxPLat,p1,likeli,maxV);//1
		}
		else
		{
			lat = maxSimilar(maxPLat,p1,likeli,maxV);
		}

		//return maxV + pre + lat;
		if (maxV > 0.0)
		{
			return maxV * pre * lat;
		}
		else
		{
			return pre * lat;
		}
		
	}
}
double CI_match::maxSimilarSum(CvPoint p0, CvPoint p1, double** likeli, int &pairNum)
{
	if (p0.x == p1.x && p0.y == p1.y)
	{
		if (likeli[p0.x][p0.y]>0)
		{
			//cout<<"Pair: "<<p0.x<<" "<<p0.y<<endl;
			pairNum+=1;
		}
		return likeli[p0.x][p0.y];
	}
	else
	{
		double maxV = 0.0;
		CvPoint maxP;
		for (int i=p0.x; i<=p1.x; i++)
		{
			for (int j=p0.y; j<=p1.y; j++)
			{
				if (likeli[i][j]>=maxV)
				{
					maxV = likeli[i][j];
					maxP.x = i;
					maxP.y = j;
				}
			}
		}

		CvPoint maxPPre;
		CvPoint maxPLat;
		maxPPre.x = maxP.x-1 > p0.x ? maxP.x-1 : p0.x; 
		maxPPre.y = maxP.y-1 > p0.y ? maxP.y-1 : p0.y;
		maxPLat.x = maxP.x+1 < p1.x ? maxP.x+1 : p1.x; 
		maxPLat.y = maxP.y+1 < p1.y ? maxP.y+1 : p1.y;

		if (maxV < EP)
		{
			return 0;
		}
		else
		{
			//cout<<"Pair: "<<maxP.x<<" "<<maxP.y<<endl;
			pairNum+=1;
		}

		double pre, lat;
		if (maxPPre.x == maxP.x || maxPPre.y == maxP.y)
		{
			pre = 0;
		}
		else
		{
			pre = maxSimilarSum(p0,maxPPre,likeli,pairNum);
		}
		if (maxPLat.x == maxP.x || maxPLat.y == maxP.y)
		{
			lat = 0;
		}
		else
		{
			lat = maxSimilarSum(maxPLat,p1,likeli,pairNum);
		}
		return maxV + pre + lat;
	}
}

void CI_match::initial(int signIndex)
{
	CString s_filefolder;
	s_filefolder.Format("..\\output\\%d",signIndex);
	_mkdir(s_filefolder);
	CString s_ImgFileName;
	s_ImgFileName.Format("..\\output\\%d\\detail.csv",signIndex);
	//s_ImgFileName.Format("..\\output\\detail.csv",signIndex);
	outfile_detail.open(s_ImgFileName,ios::out);
}


void CI_match::release(void)
{
	outfile_detail.close();
}


void CI_match::readInMask(CString maskRoute)
{
	ifstream  infileMask; 
	infileMask.open(maskRoute,ios::in);
	for (int i=0; i<Word_num; i++)
	{
		infileMask>>testFlag[i];
	}
}


int CI_match::run_single(int galleryKeyFrameNo[], State galleryState[][MaxCombine], 
	double galleryTranfer[][MaxCombine+2][MaxCombine+2],
	float postureMatrix[][maxClassNum][maxClassNum],
	vector<State> probeState)
{
	int keyNo_P = probeState.size();
	int keyNo_G;
	double wordScore[Word_num];
	int    wordRouteNo[Word_num];

	for (int w=0; w<Word_num; w++)
	{ 
		wordScore[w] = 0;
		wordRouteNo[w] = 0;
		keyNo_G = galleryKeyFrameNo[w];

		double** likeli;
		likeli = new double*[keyNo_G];
		for (int l=0; l<keyNo_G; l++)
		{
			likeli[l] = new double[keyNo_P];
		}
		for (int m=0; m<keyNo_G; m++)
		{
			for (int n=0; n<keyNo_P; n++)
			{
				likeli[m][n] = states_similar(galleryState[w][m], probeState[n],postureMatrix);
				cout<<likeli[m][n]<<" ";
			}
			cout<<endl;
		}
		cout<<endl;

	
		CvPoint p0, p1;
		p0.x = 0;
		p0.y = 0;
		p1.x = keyNo_G-1>0 ? keyNo_G-1:0;
		p1.y = keyNo_P-1>0 ? keyNo_P-1:0;

		int pairNum = 0;
		double myMaxSimilar = maxSimilarSum(p0,p1,likeli,pairNum);
		wordScore[w] += myMaxSimilar/(pairNum+EP);//sum them.
		if (myMaxSimilar > 0)
		{
			wordRouteNo[w]++;
		}

// 		double myMaxSimilar = maxSimilarSum(p0,p1,likeli, pairNum);
// 		myMaxSimilar = pow(myMaxSimilar,(double)(1.0/(keyNo_G_+EP)));
// 		if (myMaxSimilar > wordScore[w])
// 		{
// 			wordScore[w] = myMaxSimilar;
// 		}

		for (int l=0; l<keyNo_G; l++)
		{
			delete[] likeli[l];
		}
		delete[] likeli;
		wordScore[w] /= (wordRouteNo[w]+EP);
	}


	RANK myRank[Word_num];
	for (int w=0; w<Word_num; w++)
	{
		outfile_detail<<w<<","<<wordScore[w]<<endl;
		myRank[w].signID = w;
		myRank[w].score = wordScore[w];
	}

	sort(myRank,myRank+Word_num,comparison);

	return myRank[0].signID;
}