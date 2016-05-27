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

#ifndef __AVIPLAYER_H__
#define __AVIPLAYER_H__

#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <vfw.h>

#pragma comment( lib, "vfw32.lib" )	// For the AVI player

class AVIPlayer
{
private:
	int frame;
	AVISTREAMINFO psi;
	PAVISTREAM pavi;
	PGETFRAME pgf;
	BITMAPINFOHEADER bmih;
	long lastframe;
	int width;
	int height;
	char *pdata;
	int mpf;

	int next;

	HDRAWDIB hdd;
	HBITMAP hBitmap;
	HDC hdc;
	unsigned char* data;

	DWORD Oldtick;

	bool repeat;

	void SwapByte(void* buff);
	void GrabAVIFrame(int frame);
	void HandleAVI(DWORD milliseconds);
public:
	AVIPlayer() {};

	bool OpenAVI(LPCSTR szFile);

	void SetRepeat(bool rep) { repeat = rep; };
	bool EndOfAVI() { if(frame>=lastframe) return true; else return false; };

	int GetFrame() { return frame; };

	void SetTicker() {  Oldtick=GetTickCount(); };

	void DrawAVI();

	void CloseAVI();
};

#endif