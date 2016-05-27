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
// Objects.h
// 
// By Jeff Verkoeyen
// 
// Purpose:
//		Allows easy creating/drawing of characters to the screen
// 
// Version Number:  v0.1
// 
// Versions:
//		v0.1, 5/2/2003:
//			First release, includes drawing and frame-moving functions
// 
// ©2003 TDA (The Developer's Alliance)
// 

/***********************\
*	  Tutorial, YAY!	*
\***********************/

// Instructions for creating an Object:
// 
// To create a Object is very simple.  Just create a class of Object, like so:
// 
// Object Conveyor;
// 
// And place that at the top of your main code as a global variable (or you
// can play a game of catch and pass it between functions (which gets annoying).
// 
// To initialize your Object (which you must ALWAYS do, or the program will
// probably crash), you just need to call the Init function, like so:
// 
// Conveyor.Init(<Texture ID>,<Texture's Start Pos>,<Where to draw the texture>,
//               <Bounce the animation>,<Number of animations in the set>,
//               <Facing Direction>,<Speed of the frames>);
//
// Texture ID:  This must be the ID number of the texture that you have preloaded
// using the OpenGL.h img loader or your own.  This ID should be unique
// 
// Texture's Start Pos: Where the texture starts in the bmp, using pixel coords.
// If you want to have any animations, all of the animations must go HORIZONTAL
// to the right, with the first texture on the left being the first frame.
// Here's an example (each box is 32x32 pixels):
// 
//  _ _ _ _ _
// |1|2|3|4|5|
// |-|-|-|-|-|
// |6|7|8|9|0|
//  - - - - -
// 
// Box 1 would be at 0,0, box 2 would be at 32x0, and box 6 would be at 0x32.
// If you wanted an animation that had four steps, you would probably want to
// place the first frame of the animation in box 1, and continue on through
// until you hit four, so your frames would be in boxes 1, 2, 3, and 4
// This just makes the engine much easier to handle itself than having to jump
// from box to box, trying to find the next frame of the animation :)
// 
// Where to draw the texture: Screen coordinates of where to draw the Object,
// Screen coordinates are from 0-800,0-600.  0,0 being top left, 800,600 being
// bottom right.
// 
// Bounce the animation: Send true to this if you want the animation to bounce
// back and forth through the frames.
// 
// Number of animations in the set:  This allows the engine to know how many
// frames are in the set, and will display that number frames.  Use 0 or one to
// signify no frames.  You may also use the variable NOANIMATIONS to signify
// that you don't want any.
// 
// Facing Direction:  Which direction the object is facing.  Use the enumerated
// UP/DOWN/LEFT/RIGHT here, or you might draw the object in the wrong direction.
// 
// Speed of the frames:  This is the speed at which your frames will change.
// This variable is here mainly for easy changing, in case the frame rate of a
// user is really low/high.
// 

#include "common.h"

#ifndef __OBJECTS_H__
#define __OBJECTS_H__

class Object
{
private:
	ObjectInfo Data;
public:
	Object(){};

	void Init(unsigned int TEXID, iCOORD StartPos, iCOORD Pos, bool Bounce, unsigned int numAnims, int Face, unsigned int FrameSpeed);

	void HandleObject(){DrawObject();MoveFrames();};

	void DrawObject();
	void DrawObjectToPos(int x, int y, int z);
	void Draw();

	void MoveFrames();

	int GetFace(){return Data.Face;};
	void SetFace(int Face){Data.Face=Face;};
	void SetFrame(int Frame){Data.Frame=Frame;};

	iCOORD GetbmpSize(){return Data.BmpSize;};
	iCOORD GetPos(){return Data.Pos;};
	unsigned int GetID(){return Data.TEXID;};

	void Reset() { Data.Frame=Data.FrameCounter=0; };
};

#endif