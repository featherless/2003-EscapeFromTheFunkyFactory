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

// 
// Levels.h
// 
// By Jeff Verkoeyen
// 
// Purpose:
//		To be able to create levels and make them scroll.
// 
// Version Number:  v0.1a
// 
// Versions:
//		v0.1, 5/1/2003:
//			Includes ability to create and draw levels.
//		v0.1a, 5/2/2003:
//			Fixed a bug in the drawing of the levels, added IsOn command.
//		v0.1b, DAV 5/3/03
//			Fixed for mulitple source files, added Levels.cpp
// 
// ©2003 TDA (The Developer's Alliance)
// 

// divisibles of 800:  10, 16, 20, 25, 32, 40
// divisibles of 600:  10, 12, 15, 20, 24, 25

#ifndef __LEVELS_H__
#define __LEVELS_H__

#include "common.h"
#include "Objects.h"

class Level					// Level class
{
private:
	sColor GlobalCol;
public:
	LevelInfo Data;			// We'll need some level data
	char NextLevel[64];
	
	unsigned long TimerBegin;
	unsigned long PausedTime;
	unsigned long OverallPausedTime;
	unsigned long FinalTime;

	void StartTimer() { TimerBegin=GetTickCount(); OverallPausedTime=0;};
	void PrintTime();
	void Pause() { PausedTime=GetTickCount(); };
	void UnPause() { OverallPausedTime+=(GetTickCount()-PausedTime); };
	void StopTimer() { FinalTime=GetTickCount()-TimerBegin-OverallPausedTime; TimerBegin=0; };
	unsigned long GetTime() { return FinalTime; };
	char* ConvertTime(unsigned long Time);

	Level(){ GlobalCol.r=1.0f; GlobalCol.g=1.0f; GlobalCol.b=1.0f;};// Default constructor
	
	void Init(int TEXID);	// Accepts a Texture ID number
	
	void DrawLevel();	// Draws the level

	int IsOn(int x,int y){return Data.ColDetect[y*DEFAULTILEX+x];};

	void SetTileData(int x, int y,int value){Data.TilesInfo[y*DEFAULTILEX+x]=value;};
	int GetTileData(int x, int y){return Data.TilesInfo[y*DEFAULTILEX+x];};

	void SetTileColDet(int x, int y,int value){Data.ColDetect[y*DEFAULTILEX+x]=value;};
	int GetTileColDet(int x, int y){return Data.ColDetect[y*DEFAULTILEX+x];};

	int GetMaxTiles(){return (int)(Data.BmpSize.X/DEFAULTILESIZE)*(int)(Data.BmpSize.Y/DEFAULTILESIZE);};

	void DrawTile(int x, int y, int z, int tile);

	void ClearData() { memset(Data.TilesInfo,0,sizeof(int)*DEFAULTILEX*DEFAULTILEY); memset(Data.ColDetect,0,sizeof(int)*DEFAULTILEX*DEFAULTILEY); };

	iCOORD GetbmpSize(){return Data.BmpSize;};

	void SetGlobalColor(sColor Global) { GlobalCol=Global; };
};

#endif