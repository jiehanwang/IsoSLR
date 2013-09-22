#include "StdAfx.h"
#include "I_galleryCreate.h"


CI_galleryCreate::CI_galleryCreate(void)
{
}


CI_galleryCreate::~CI_galleryCreate(void)
{
}


void CI_galleryCreate::galleryReadFromDat(CString route)
{
	int* keyFrame_sequence; 
	int* keyFrame_sequence_locate;
	keyFrame_sequence = new int[Word_num];
	keyFrame_sequence_locate = new int[Word_num];

	ifstream infile1;
	infile1.open(route+"\\keyFrameNo.dat",ios::binary);
	infile1.read( (char *)keyFrame_sequence, Word_num*sizeof(int) );
	infile1.close();

	*(keyFrame_sequence_locate+0) = *(keyFrame_sequence + 0);
	for(int i=1;i<Word_num;i++)
	{
		*(keyFrame_sequence_locate+i) = *(keyFrame_sequence_locate+i-1) + *(keyFrame_sequence + i);
	}

	int totalKeyFrame = 0;
	for (int w=0; w<Word_num; w++)
	{
		keyFrameNo[w] = *(keyFrame_sequence + w);
		totalKeyFrame += keyFrameNo[w];
	}
	//////////////////////////////////////////////////////////////////////////
	float* state_sequence;
	int stateSize = totalKeyFrame*15;
	state_sequence = new float[stateSize];

	ifstream infileState;
	infileState.open(route+"\\myState.dat",ios::binary);
	infileState.read( (char *)state_sequence, stateSize*sizeof(float) );
	infileState.close();

	int location;
	for (int w=0; w<Word_num; w++)
	{
		for (int m=0; m<keyFrameNo[w]; m++)
		{
			if (w==0)
			{
				location = (0 + m)*15;
			}
			else
			{
				location = (*(keyFrame_sequence_locate + w - 1) + m)*15;
			}
			
			myState[w][m].r = (int)(*(state_sequence + location + 0));
			myState[w][m].l = (int)(*(state_sequence + location + 1));
			myState[w][m].b = (int)(*(state_sequence + location + 2));
			myState[w][m].R = (int)(*(state_sequence + location + 3));
			myState[w][m].L = (int)(*(state_sequence + location + 4));
			myState[w][m].B = (int)(*(state_sequence + location + 5));
			myState[w][m].PL.x = (float)(*(state_sequence + location + 6));
			myState[w][m].PL.y = (float)(*(state_sequence + location + 7));
			myState[w][m].PL.z = (float)(*(state_sequence + location + 8));
			myState[w][m].PR.x = (float)(*(state_sequence + location + 9));
			myState[w][m].PR.y = (float)(*(state_sequence + location + 10));
			myState[w][m].PR.z = (float)(*(state_sequence + location + 11));
			myState[w][m].TL = (int)(*(state_sequence + location + 12));
			myState[w][m].TR = (int)(*(state_sequence + location + 13));
			myState[w][m].frequency = (double)(*(state_sequence + location + 14));

		}
	}
	//////////////////////////////////////////////////////////////////////////
	double* transfer_sequence;
	int transferSize = Word_num*(MaxCombine+2)*(MaxCombine+2);
	transfer_sequence = new double[transferSize];
	ifstream infileTransfer;
	infileTransfer.open(route+"\\tranfer.dat",ios::binary);
	infileTransfer.read( (char *)transfer_sequence, transferSize*sizeof(double) );
	infileTransfer.close();

	//BUG!!!!!
	for (int w=0; w<Word_num; w++)
	{
		for (int k=0; k<MaxCombine+2; k++)
		{
			for (int l=0; l<MaxCombine+2; l++)
			{
				tranfer[w][k][l] = *(transfer_sequence + w*(MaxCombine+2)*(MaxCombine+2) + k*(MaxCombine+2) + l);

			}
		}
	}


	delete[] keyFrame_sequence;
    delete[] keyFrame_sequence_locate;
	delete[] state_sequence;
	delete[] transfer_sequence;
	
}
