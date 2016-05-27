///////////////////////////////////////////////
/*                                           */
/* MATCRYPT.H - HEADER FILE FOR MATCRYPT.CPP */
/*     MATRIX ENCRYPTION ALGORITHM OBJECT    */
/*                                           */
/*           CREATED DAV YUST, 2002          */
/*            SNIPERDAV@FP2K2.COM            */
/*                                           */
/*     DO NOT MODIFY IN ANY WAY WITHOUT      */
/*    WRITTEN PERMISSION FROM THE AUTHOR.    */
/*                                           */
///////////////////////////////////////////////

#ifndef __matcrypt_h__
#define __matcrypt_h__

#include <iostream.h>
#include <fstream.h>
#include <string.h>

class mtxEncryptor {

	char characters[17][17]; //16x16 array of characters
	char xvalue[257]; //holds the x-coord of that character in the array
	char yvalue[257]; //holds the y-coord of that character in the array
	char x1, x2, y1, y2; // holds x and y coords of current pair
	int curPair; //holds location of current pair en(/de)crypting

public:

	mtxEncryptor(); //constructor

	void encrypt(char iStr[], char destStr[]); //encryptor function
	void decrypt(char iStr[], char destStr[]);
	int fencrypt(ifstream & fin, ofstream & fout); //encrypt from file function
	int fdecrypt(ifstream & fin, ofstream & fout); //decrypt from file function
};

#endif