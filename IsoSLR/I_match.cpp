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
		//if ()   //Position is used here.
		{
			if(myState1.l == 1 && myState1.r == 1)
			{
				int left1 = myState1.L<myState2.L?myState1.L:myState2.L;
				int left2 = myState1.L>myState2.L?myState1.L:myState2.L;

				int right1 = myState1.R<myState2.R?myState1.R:myState2.R;
				int right2 = myState1.R>myState2.R?myState1.R:myState2.R;

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
	for (int w=0; w<Word_num; w++)
	{
		wordScore[w] = 0;
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
						//////////////////////////////////////////////////////////////////////////
						int keyNo_G_ = myStack.size()-2;
// 						for (int l=0; l<myStack.size(); l++)
// 						{
// 							cout<<myStack[l]<<'\t';
// 						}
// 						cout<<endl;

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
						CvPoint p0, p1;
						p0.x = 0;
						p0.y = 0;
						p1.x = keyNo_G_-1;
						p1.y = keyNo_P-1;
						double myMaxSimilar = maxSimilar(p0,p1,likeli);
// 						for (int l=0; l<keyNo_G_-1; l++)
// 						{
// 							int pre = myStack[l];
// 							int cur = myStack[l+1];
// 							if (cur < keyNo_G+1)
// 							{
// 								myMaxSimilar *= (galleryTranfer[w][pre][cur] * galleryState[w][cur].frequency);
// 							}
// 							else if (cur == keyNo_G+1)
// 							{
// 								myMaxSimilar *= galleryTranfer[w][pre][cur];
// 							}
// 
// 							
// 						}
							//To sum them or to choose the maximum is a problem.
						//myMaxSimilar /= 2*(keyNo_G_+0.000001);
						myMaxSimilar = pow(myMaxSimilar,(double)(1.0/(keyNo_G_+0.000001)));
						//wordScore[w] += myMaxSimilar;//sum them. 
						if (myMaxSimilar > wordScore[w])
						{
							wordScore[w] = myMaxSimilar;
						}

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

		//cout<<"word: "<<w<<"mySimilar: "<<wordScore[w]<<endl;
	}


	double maxWordScore = 0.0;
	int maxWordLabel = -1;
	for (int w=0; w<Word_num; w++)
	{
		if (wordScore[w]>maxWordScore)
		{
			maxWordScore = wordScore[w];
			maxWordLabel = w;
		}
	}
	//cout<<"maxWordScore: "<<maxWordScore<<endl;
	return maxWordLabel;
}


double CI_match::maxSimilar(CvPoint p0, CvPoint p1, double** likeli)
{
	if (p0.x == p1.x && p0.y == p1.y)
	{
		if (likeli[p0.x][p0.y] == 0)
		{
			return 1;
		}
		else
		{
			return likeli[p0.x][p0.y];
		}
		
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

		return maxV * maxSimilar(p0,maxPPre,likeli) * maxSimilar(maxPLat,p1,likeli);
	}

	return 0;
}
