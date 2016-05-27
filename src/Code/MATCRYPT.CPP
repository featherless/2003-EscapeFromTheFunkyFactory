///////////////////////////////////////////////
/*                                           */
/* MATCRYPT.CPP - SOURCE FILE FOR MATCRYPT.H */
/*     MATRIX ENCRYPTION ALGORITHM OBJECT    */
/*                                           */
/*           CREATED DAV YUST, 2002          */
/*            SNIPERDAV@FP2K2.COM            */
/*                                           */
/*     DO NOT MODIFY IN ANY WAY WITHOUT      */
/*    WRITTEN PERMISSION FROM THE AUTHOR.    */
/*                                           */
///////////////////////////////////////////////

#include "matcrypt.h"

mtxEncryptor::mtxEncryptor()
{
	for(int i = 0; i < 256; i++)
	{
		xvalue[i] = (char)(i % 16);
		yvalue[i] = (char)((i - (i % 16)) / 16);
		characters[(int)xvalue[i]][(int)yvalue[i]] = (char)i;
	}

	x1 = x2 = y1 = y2 = 0;
	curPair = 0;

}

void mtxEncryptor::encrypt(char iStr[], char destStr[])
{
	char* returnStr;
	int tmpVar = 0;
	int length = strlen(iStr);

	//extend string if needed to make it an even # of characters
	if(length % 2)
	{
		returnStr = new char[++length + 1];
		strcpy(returnStr, iStr);
		strcat(returnStr, " ");
	}
	else
	{
		returnStr = new char[length + 1];
		strcpy(returnStr, iStr);
	}

	//loop through pairs and encrypt/decrypt

	for(curPair = 0; curPair < (length >> 1); curPair++)
	{
		x1 = xvalue[returnStr[curPair * 2]];
		y1 = yvalue[returnStr[curPair * 2]];
		x2 = xvalue[returnStr[(curPair * 2)+1]];
		y2 = yvalue[returnStr[(curPair * 2)+1]];

		if(x1 == x2 && y1 == y2) //same character - flip x and y
		{
			x1 = x2 = 16 - x1;
			y1 = y2 = 16 - y1;
		}
		else if(x1 == x2) // same column
		{
			y1 = (y1 + 1) % 16;
			y2 = (y2 + 1) % 16;
		}
		else if(y1 == y2) //same row
		{
			x1 = (x1 + 1) % 16;
			x2 = (x2 + 1) % 16;
		}
		else
		{
			tmpVar = x1; //flip x
			x1 = x2;
			x2 = tmpVar;
		}

		returnStr[curPair * 2] = characters[x1][y1];
		returnStr[(curPair * 2)+1] = characters[x2][y2];

	}

	strcpy(destStr, returnStr);
	return;

}

void mtxEncryptor::decrypt(char iStr[], char destStr[])
{
	char* returnStr;
	int tmpVar = 0;
	int length = strlen(iStr);

	//extend string if needed to make it an even # of characters
	if(length % 2)
	{
		returnStr = new char[++length + 1];
		strcpy(returnStr, iStr);
		returnStr[length - 1] = ' ';
	}
	else
	{
		returnStr = new char[length + 1];
		strcpy(returnStr, iStr);
	}

	//loop through pairs and encrypt/decrypt

	for(curPair = 0; curPair < (length >> 1); curPair++)
	{
		x1 = xvalue[returnStr[curPair * 2]];
		y1 = yvalue[returnStr[curPair * 2]];
		x2 = xvalue[returnStr[(curPair * 2)+1]];
		y2 = yvalue[returnStr[(curPair * 2)+1]];

		if(x1 == x2 && y1 == y2) //same character - flip x and y
		{
			x1 = x2 = 16 - x1;
			y1 = y2 = 16 - y1;
		}
		else if(x1 == x2) // same column
		{
			y1 = (y1 + 15) % 16;
			y2 = (y2 + 15) % 16;
		}
		else if(y1 == y2) //same row
		{
			x1 = (x1 + 15) % 16;
			x2 = (x2 + 15) % 16;
		}
		else
		{
			tmpVar = x1; //flip x
			x1 = x2;
			x2 = tmpVar;
		}

		returnStr[curPair * 2] = characters[x1][y1];
		returnStr[(curPair * 2)+1] = characters[x2][y2];

	}

	strcpy(destStr, returnStr);
	return;

}

int mtxEncryptor::fencrypt(ifstream &fin, ofstream &fout)
{
	char curStr[256];
	char destStr[256];
	
	if(fin && fout)
	{
		while(!fin.eof())
		{
			fin.getline(curStr, 256, 0);
			encrypt(curStr, destStr);
			fout << destStr;
		}
		return 0;
	}
	else return 1;
}

int mtxEncryptor::fdecrypt(ifstream &fin, ofstream &fout)
{
	char curStr[256];
	char destStr[256];
	
	if(fin && fout)
	{
		while(!fin.eof())
		{
			fin.getline(curStr, 256, 0);
			decrypt(curStr, destStr);
			fout << destStr;
		}
		return 0;
	}
	else return 1;
}