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
	for (int w=0; w<Word_num; w++)
	{
		cout<<"word"<<w<<endl;
		int keyNo_G = galleryKeyFrameNo[w];
		vector<int> myStack;
		vector<int> maxPro;
		myStack.push_back(0);
		
		int* posi = new int[keyNo_G+2];
		for (int k=0; k<keyNo_G+2; k++)
		{
			posi[k] = k;
		}
		//currentP.push_back(-1);
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
						for (int l=0; l<myStack.size(); l++)
						{
							//compute the maxPro;
							cout<<myStack[l]<<'\t';
						}
						cout<<endl;
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


	}
	return 0;
}
