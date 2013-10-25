#include "StdAfx.h"
#include "I_galleryCreate.h"


CI_galleryCreate::CI_galleryCreate(void)
{
}


CI_galleryCreate::~CI_galleryCreate(void)
{
}

void CI_galleryCreate::readInPostureC(CString route, int lrb)
{
	FILE *filein;
	char oneline[HOG_dimension*10];
	int itemNumber;
	filein = fopen(route, "rt");

	readstr(filein,oneline);
	sscanf(oneline, "NUMBER %d\n", &itemNumber);
	classNum[lrb] = itemNumber;
	for (int loop = 0; loop < itemNumber; loop++)
	{
		readstr(filein,oneline);
		char* sp = oneline; 
		float num; 
		int read; 
		int dimensionIndex = 0;
		while( sscanf(sp, "%f %n", &num, &read)!=EOF )
		{ 
			postureC[lrb][loop][dimensionIndex++] = num;
			sp += read-1; 
		} 
	}
	fclose(filein);
}
void CI_galleryCreate::readstr(FILE *f,char *string)
{
	do
	{
		fgets(string, HOG_dimension*10, f);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}
void CI_galleryCreate::readstr2(FILE *f,char *string)
{
	do
	{
		fgets(string, maxClassNum*10, f);
	} while ((string[0] == '/') || (string[0] == '\n'));
	return;
}
void CI_galleryCreate::readInPostureMatrix(CString route, int lrb)
{
	FILE *filein;
	char oneline[maxClassNum*10];
	int itemNumber;
	filein = fopen(route, "rt");

	itemNumber = classNum[lrb];
	for (int loop = 0; loop < itemNumber; loop++)
	{
		readstr2(filein,oneline);
		char* sp = oneline; 
		float num; 
		int read; 
		int classIndex = 0;
		while( sscanf(sp, "%f %n", &num, &read)!=EOF )
		{ 
			postureMatrix[lrb][loop][classIndex++] = num;
			sp += read-1; 
		} 
	}
	fclose(filein);
	//delete[] oneline;
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
	int length = 16 + 6*traNumDes;
	float* state_sequence;
	int stateSize = totalKeyFrame*length;
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
				location = (0 + m)*length;
			}
			else
			{
				location = (*(keyFrame_sequence_locate + w - 1) + m)*length;
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
			myState[w][m].Head.x = (float)(*(state_sequence + location + 12));
			myState[w][m].Head.y = (float)(*(state_sequence + location + 13));
			myState[w][m].Head.z = (float)(*(state_sequence + location + 14));

#ifdef useTra
			for (int i=0; i<traNumDes; i++)
			{
				CvPoint3D32f leftHand;
				CvPoint3D32f rightHand;
				leftHand.x = (float)(*(state_sequence + location + 15 + i*6 + 0));
				leftHand.y = (float)(*(state_sequence + location + 15 + i*6 + 1));
				leftHand.z = (float)(*(state_sequence + location + 15 + i*6 + 2));
				rightHand.x = (float)(*(state_sequence + location + 15 + i*6 + 3));
				rightHand.y = (float)(*(state_sequence + location + 15 + i*6 + 4));
				rightHand.z = (float)(*(state_sequence + location + 15 + i*6 + 5));
				myState[w][m].TL.push_back(leftHand);
				myState[w][m].TR.push_back(rightHand);
				//cout<<i<<endl;
				//cout<<myState[w][m].TL[i].x<<" "<<myState[w][m].TL[i].y<<" "<<myState[w][m].TL[i].z<<endl;
				//cout<<myState[w][m].TR[i].x<<" "<<myState[w][m].TR[i].y<<" "<<myState[w][m].TR[i].z<<endl;

			}
#endif
			//myState[w][m].TL = (int)(*(state_sequence + location + 12));
			//myState[w][m].TR = (int)(*(state_sequence + location + 13));
			myState[w][m].frequency = (double)(*(state_sequence + location + 15 + traNumDes*6));
			//cout<<myState[w][m].frequency<<endl;

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


void CI_galleryCreate::readGallery(CString route)
{
	readInPostureC(route + "\\postureC_0.txt",0); //Left posture
	readInPostureC(route + "\\postureC_1.txt",1); //Right posture
	readInPostureC(route + "\\postureC_2.txt",2); //Both posture
	readInPostureMatrix(route + "\\postureMatrix_0.txt",0);
	readInPostureMatrix(route + "\\postureMatrix_1.txt",1);
	readInPostureMatrix(route + "\\postureMatrix_2.txt",2);

	galleryReadFromDat(route);
}
