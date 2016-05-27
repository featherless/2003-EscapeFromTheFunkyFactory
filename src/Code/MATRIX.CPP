/*
 Copyright 2003 The Developers Alliance. All Rights Reserved.
 Copyright 2003, 2016 Jeff Verkoeyen. All Rights Reserved.

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include "matcrypt.h"
#include <fstream.h>

int main() {

	char returnStr[256];
	int choice;
	ifstream fin;
	ofstream fout;

	mtxEncryptor cryptor;

	char cryptStr[256];

	system("CLS");

	cout << "MatrixCrypt 1.0" << endl;
	cout << "--------------------------------" << endl;
	cout << "1: Encrypt Cmd Line" << endl;
	cout << "2: Decrypt Cmd Line" << endl;
	cout << "3: Encrypt File" << endl;
	cout << "4: Decrypt File" << endl;
	cout << endl;
	cout << "Choose item: ";

	cin >> choice;
	cin.getline(cryptStr, 2);

	if(choice == 1)
	{
		system("CLS");
		cout << "Please enter string to encrypt," << endl;
		cin.getline(cryptStr, 256);
		cryptor.encrypt(cryptStr,returnStr);
		system("CLS");
		
		cout << "Input:" << endl;
		cout << cryptStr << endl << endl;
	
		cout << "Output:" << endl;
		cout << returnStr << endl << endl;
	}
	else if(choice == 2)
	{
		system("CLS");
		
		cout << "Enter string to decrypt:" << endl;
		cin.getline(cryptStr, 256);

		cryptor.decrypt(cryptStr,returnStr);
		system("CLS");
		
		cout << "Input:" << endl;
		cout << cryptStr << endl << endl;
	
		cout << "Output:" << endl;
		cout << returnStr << endl << endl;
	}
	else if(choice == 3)
	{
		system("CLS");

		cout << "Enter file to encrypt:" << endl;
		cin.getline(cryptStr, 256);

		fin.open(cryptStr);

		cout << "Enter file to output to:" << endl;
		cin.getline(cryptStr, 256);

		fout.open(cryptStr);

		if(!cryptor.fencrypt(fin, fout)) cout << endl << "Encryption successful." << endl;
		else cout << endl << "Encryption failed." << endl;
	}
	else if(choice == 4)
	{
		system("CLS");

		cout << "Enter file to decrypt:" << endl;
		cin.getline(cryptStr, 256);

		fin.open(cryptStr);

		cout << "Enter file to output to:" << endl;
		cin.getline(cryptStr, 256);

		fout.open(cryptStr);

		if(!cryptor.fdecrypt(fin, fout)) cout << endl << "Decryption successful." << endl;
		else cout << endl << "Decryption failed." << endl;
	}

	return 0;
}

