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
// Characters.h
// 
// By Jeff Verkoeyen
// 
// Purpose:
//		Allows easy creating/drawing of characters to the screen
// 
// Version Number:  v0.1a
// 
// Versions:
//		v0.1, 5/1/2003:
//			Includes basic abilities to create a character,
//			assign it a picture, and draw the character to the
//			screen.
//		v0.1a, 5/2/2003:
//			Added collision detection with static objects in the background
//			from all sides.  You also don't float through the tile if you
//			are slightly above/below/beside it :)
// 
// ©2003 TDA (The Developer's Alliance)
// 

#ifndef __CHARACTERS_H__
#define __CHARACTERS_H__

#include "Levels.h"
//#include "Objects.h"

#define DEFAULTFACE UP			// Default direction
#define DEFAULTFRAMESPEED 10	// Default frame speed

struct Facing
{
	bool UP,DOWN,LEFT,RIGHT;
};

class Character
{
private:
	Facing disWalk;
	sColor GlobalCol;
public:
	int flags;
	int LastObs;
	CharInfo Data;
	Character(){disWalk.UP=disWalk.DOWN=disWalk.RIGHT=disWalk.LEFT=true;};

	void Init(unsigned int TEXID, iCOORD pos);

	void DrawCharToScreen();		// Draws the character to the screen

	void MoveFrames();				// Increments the character's frames

	void SetFrameSpeed(unsigned int Speed){Data.FrameSpeed=Speed;};	// Sets the frame speed again

	void SetFace(unsigned int Face){Data.Facing=Face;};	// Sets the direction the character is facing
	int GetFace(){return Data.Facing;};

	void MoveChar(Level level, bool forward = true, int dir = 0, bool moveFrames=true);		// Moves the character relative to the direction it is facing

	void Debug(Level level);

	void SetSpeed(int Speed){Data.Speed=Speed;};

	iCOORD GetbmpSize(){return Data.BmpSize;};

	void DisDir(char DIR);

	void SetGlobalColor(sColor Global) { GlobalCol=Global; };
};

#endif