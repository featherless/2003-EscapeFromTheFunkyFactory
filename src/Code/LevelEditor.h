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

#ifndef __LEVELEDITOR_H__
#define __LEVELEDITOR_H__

#include "common.h"
#include "obstacle.h"
#include "Levels.h"
#include "Characters.h"

struct Editor
{
	Mouse* mouse;
	int PlaceMode;
};

extern char GlobalDir[512];

enum {OBJECT,TILE,PLAYERSTART};

class LevelEditor
{
	friend class cObstacle;
private:
	Editor Data;
	GLuint mouseBMP;

	iCOORD Select[2];
	int Tile;
	int LastVal;
	bool Editing,Info,Menu,ShowGrid,PlacedChar,FillGrid,ColDet,CreatingObj,DropPos,SelectTile,Selected;
	int FillTile;
	char TileName[64];
	char ObjeName[64];
	char PlayName[64];
	char TransName[64];
	char NextLevel[64];
	int selectedObj;

	ObstacleSave ObstData[MAX_OBSTACLES];
	ObstacleSave ObstacleTemp;

	iCOORD PlayerStart;
	Level LevelMap;
	cObstacleManager ObsMan;
	cObstacle* obs;

	Character MainChar;

	bool LoadMouseBMP();
	void DrawMouse();
	void HandleMouse();
	void HandleClicks();
	void DrawInfo();
	void DrawMenu();
	bool HandleMenu();
	void DrawGrid();
	void Fill();
	void SetColDet();
	void CreateObs();
	void EditDropPos();
	void SelectTiles();

	bool tempOn;

	void SaveFile(char* FileName, char* Filter, char* Ex);
	void LoadFile(char* FileName, char* Filter, char* Ex);
	void SaveLevel();
	void LoadLevel();
	
	void Debug();
public:
	LevelEditor();
	~LevelEditor();

	bool InitEditor()
	{
		main.SetNumTexts(10);		// 3 textures should be fine for now

		sColor Alpha;
		Alpha.r=255;				// Set the pink transparent color
		Alpha.g=0;
		Alpha.b=255;
		
		main.SetAlpha(Alpha);		// Set the Alpha color
		
		SetCurrentDirectory(GlobalDir);
		main.LoadTexture(PlayName,0);
		main.LoadTexture(TileName,1);
		main.LoadTexture(ObjeName,2);

		LevelMap.Init(1);

		for(int a=0;a<DEFAULTILEX;a++)
		{
			for(int b=0;b<DEFAULTILEY;b++)
				LevelMap.Data.TilesInfo[a*DEFAULTILEY+b]=LevelMap.Data.ColDetect[a*DEFAULTILEY+b]=0;
		}

		iCOORD pos;

		pos.X=40;
		pos.Y=40;

		MainChar.Init(0,pos);		// Must initialize the characters AFTER the textures have been loaded
		MainChar.SetFrameSpeed(3);
		MainChar.SetSpeed(1);

		ObsMan.Clear();

		return true;
	};

	bool InitEditor(Mouse* pMouse)
	{
		main.SetNumTexts(10);		// 3 textures should be fine for now

		sColor Alpha;
		Alpha.r=255;				// Set the pink transparent color
		Alpha.g=0;
		Alpha.b=255;
		
		main.SetAlpha(Alpha);		// Set the Alpha color
		
		SetCurrentDirectory(GlobalDir);
		main.SetFiltering(false);
		main.LoadTexture(PlayName,0);
		main.LoadTexture(TileName,1);
		main.LoadTexture(ObjeName,2);
		char temp[1024];
		sprintf(temp,"%s\\Images",GlobalDir);
		SetCurrentDirectory(temp);
		main.LoadTexture("Arrows.bmp",3);

		Data.mouse=pMouse;
		if(!LoadMouseBMP())
			return false;
		LevelMap.Init(1);

		for(int a=0;a<DEFAULTILEX;a++)
		{
			for(int b=0;b<DEFAULTILEY;b++)
				LevelMap.Data.TilesInfo[a*DEFAULTILEY+b]=LevelMap.Data.ColDetect[a*DEFAULTILEY+b]=0;
		}

		iCOORD pos;

		pos.X=40;
		pos.Y=40;

		MainChar.Init(0,pos);		// Must initialize the characters AFTER the textures have been loaded
		MainChar.SetFrameSpeed(3);
		MainChar.SetSpeed(1);

		ObsMan.Clear();

		return true;
	};

	bool RunLevelEditor();

	void SetMode(bool mode){ObsMan.Editor=mode;};
};

#endif