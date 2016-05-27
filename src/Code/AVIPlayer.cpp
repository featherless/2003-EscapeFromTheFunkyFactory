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

#include "OpenGL.h"
extern OpenGL main;

#include "AVIPlayer.h"

void AVIPlayer::SwapByte(void* buff)						// Flips The Red And Blue Bytes (256x256)
{
	void* b = buff;						// Pointer To The Buffer
	__asm								// Assembler Code To Follow
	{
		mov ecx, 256*256				// Set Up A Counter (Dimensions Of Memory Block)
		mov ebx, b						// Points ebx To Our Data (b)
		label:							// Label Used For Looping
			mov al,[ebx+0]				// Loads Value At ebx Into al
			mov ah,[ebx+2]				// Loads Value At ebx+2 Into ah
			mov [ebx+2],al				// Stores Value In al At ebx+2
			mov [ebx+0],ah				// Stores Value In ah At ebx
			
			add ebx,3					// Moves Through The Data By 3 Bytes
			dec ecx						// Decreases Our Loop Counter
			jnz label					// If Not Zero Jump Back To Label
	}
}

bool AVIPlayer::OpenAVI(LPCSTR szFile)		// Opens An AVI File (szFile)
{
	main.Texture2D(TRUE);
	main.SetTexture(9);

	data = 0;
	hdc = CreateCompatibleDC(0);
	hdd = DrawDibOpen();
	repeat=false;
	frame=0;

	AVIFileInit();							// Opens The AVIFile Library

	// Opens The AVI Stream
	if(AVIStreamOpenFromFile(&pavi, szFile, streamtypeVIDEO, 0, OF_READ, NULL) !=0)
	{
		main.Error("Error",MB_OK,"Failed To Open The AVI Stream.");
		return false;
	}

	AVIStreamInfo(pavi, &psi, sizeof(psi));			// Reads Information About The Stream Into psi
	width=psi.rcFrame.right-psi.rcFrame.left;		// Width Is Right Side Of Frame Minus Left
	height=psi.rcFrame.bottom-psi.rcFrame.top;		// Height Is Bottom Of Frame Minus Top

	lastframe=AVIStreamLength(pavi);				// The Last Frame Of The Stream

	mpf=AVIStreamSampleToTime(pavi,lastframe)/lastframe;

	bmih.biSize			= sizeof (BITMAPINFOHEADER);// Size Of The BitmapInfoHeader
	bmih.biPlanes		= 1;						// Bitplanes
	bmih.biBitCount		= 24;						// Bits Format We Want (24 Bit, 3 Bytes)
	bmih.biWidth		= 256;						// Width We Want (256 Pixels)
	bmih.biHeight		= 256;						// Height We Want (256 Pixels)
	bmih.biCompression	= BI_RGB;					// Requested Mode = RGB

	hBitmap = CreateDIBSection (hdc, (BITMAPINFO*)(&bmih), DIB_RGB_COLORS, (void**)(&data), NULL, NULL);
	SelectObject (hdc, hBitmap);

	pgf=AVIStreamGetFrameOpen(pavi, NULL);				// Create The PGETFRAME Using Our Request Mode
	if (pgf==NULL)
	{
		main.Error("Error",MB_OK,"Failed To Open The AVI Frame.");
		return false;
	}
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	// Set Texture Max Filter
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);	// Set Texture Min Filter

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	return true;
}

void AVIPlayer::GrabAVIFrame(int frame)						// Grabs A Frame From The Stream
{
	main.SetTexture(9);
	LPBITMAPINFOHEADER lpbi;					// Holds The Bitmap Header Information
	lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(pgf, frame);	// Grab Data From The AVI Stream
	pdata=(char *)lpbi+lpbi->biSize+lpbi->biClrUsed * sizeof(RGBQUAD);	// Pointer To Data Returned By AVIStreamGetFrame

	// Convert Data To Requested Bitmap Format
	DrawDibDraw(hdd, hdc, 0, 0, 256, 256, lpbi, pdata, 0, 0, width, height, 0);

	SwapByte(data);

	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, data);
}

void AVIPlayer::CloseAVI()				// Properly Closes The Avi File
{
	DeleteObject(hBitmap);					// Delete The Device Dependant Bitmap Object
	DrawDibClose(hdd);						// Closes The DrawDib Device Context
	AVIStreamGetFrameClose(pgf);			// Deallocates The GetFrame Resources
	AVIStreamRelease(pavi);					// Release The Stream
	AVIFileExit();							// Release The File
}

void AVIPlayer::HandleAVI(DWORD milliseconds)
{
	next += milliseconds;						// Increase next Based On Timer (Milliseconds)
	frame = next/mpf;							// Calculate The Current Frame

	if(frame>=lastframe && repeat)
		frame = next = 0;
}

void AVIPlayer::DrawAVI()
{
	GrabAVIFrame(frame);

	main.Texture2D(TRUE);

	glLoadIdentity();
	glTranslated(400,300,0);
	glColor3d(1,1,1);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2d(-width/2,-height/2);
		glTexCoord2f(0.0f, 0.0f); glVertex2d(-width/2, height/2);
		glTexCoord2f(1.0f, 0.0f); glVertex2d( width/2, height/2);
		glTexCoord2f(1.0f, 1.0f); glVertex2d( width/2,-height/2);
	glEnd();

	DWORD tick = GetTickCount();
	HandleAVI(tick - Oldtick);
	Oldtick = tick;
}