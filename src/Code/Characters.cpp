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
extern int SCREENX,SCREENY;

#include "Characters.h"

void Character::Init(unsigned int TEXID, iCOORD pos)
{
	GlobalCol.r=1.0f;
	GlobalCol.g=1.0f;
	GlobalCol.b=1.0f;
	GlobalCol.a=1.0f;
	Data.Pos=pos;
	Data.Speed=1;
	Data.TilePos.X=(int)((float)pos.X/DEFAULTILESIZE);
	Data.TilePos.Y=(int)((float)pos.Y/DEFAULTILESIZE);

	iCOORD temp;
	temp.X=0;
	temp.Y=0;
	Data.StartPos=temp;
	Data.TEXID=TEXID;
	
	glBindTexture(GL_TEXTURE_2D, main.texture[TEXID]);
	
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&Data.BmpSize.X);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&Data.BmpSize.Y);

	// This is all default stuff
	Data.Facing=DEFAULTFACE;
	Data.Frame=0;
	Data.FrameSpeed=DEFAULTFRAMESPEED;
	Data.FrameCount=0;
	Data.up=true;
}

void Character::DrawCharToScreen()
{
	glColor3f(GlobalCol.r,GlobalCol.g,GlobalCol.b);
	glLoadIdentity();			// Reset our drawing matrix
	glTranslated(Data.Pos.X,Data.Pos.Y+3,4);	// Move to the central position of the character
	float size=(float)DEFAULTILESIZE/2;
	float u=(float)Data.Frame*DEFAULTILESIZE+Data.StartPos.X,v=(float)Data.Facing*DEFAULTILESIZE+Data.StartPos.Y;

	float eu=u+DEFAULTDIM,ev=v+DEFAULTDIM;	// Extended x/y values in the texture

	u/=(float)Data.BmpSize.X;	// Get the exact texture coordinates of the current sprite frame
	v/=(float)Data.BmpSize.Y;
	eu/=(float)Data.BmpSize.X;
	ev/=(float)Data.BmpSize.Y;

	main.Texture2D(TRUE);		// Make sure texturing is ON

	glBindTexture(GL_TEXTURE_2D, main.texture[Data.TEXID]);
	glBegin(GL_QUADS);
		glTexCoord2f( u,1-ev);	glVertex2f(-size, 0);
		glTexCoord2f(eu,1-ev);	glVertex2f( size, 0);
		glTexCoord2f(eu,1-v);	glVertex2f( size,-DEFAULTILESIZE);
		glTexCoord2f( u,1-v);	glVertex2f(-size,-DEFAULTILESIZE);
	glEnd();
}

void Character::MoveFrames()
{
	if(Data.up)				// Test which direction to increment
		Data.FrameCount+=Data.FrameSpeed;
	else
		Data.FrameCount-=Data.FrameSpeed;

	if(Data.FrameCount>60)	// If we hit the end of one frame sequence,
	{
		Data.Frame++;		// Increase the current frame
		Data.FrameCount=0;	// Reset the counter
	}
	if(Data.FrameCount<0)	// Same thing here as above ^^
	{
		Data.Frame--;
		Data.FrameCount=60;
	}
	if(Data.Frame>2 || Data.Frame<0)	// If we hit the end of either frame set
	{
		Data.up=!Data.up;	// Flip the up switch
		if(Data.Frame<0)	// Move our texture back up so we don't have to wait 2x as long
			Data.Frame=0;
		if(Data.Frame>2)
			Data.Frame=2;
	}
}

void Character::DisDir(char DIR)	// This function is used mainly for conveyor belts
{
	switch((int)DIR)
	{
	case UP:
		disWalk.UP=false;
		disWalk.DOWN=true;
		disWalk.RIGHT=true;
		disWalk.LEFT=true;
		break;
	case DOWN:
		disWalk.DOWN=false;
		disWalk.UP=true;
		disWalk.RIGHT=true;
		disWalk.LEFT=true;
		break;
	case RIGHT:
		disWalk.RIGHT=false;
		disWalk.DOWN=true;
		disWalk.UP=true;
		disWalk.LEFT=true;
		break;
	case LEFT:
		disWalk.LEFT=false;
		disWalk.DOWN=true;
		disWalk.RIGHT=true;
		disWalk.UP=true;
		break;
	default:
		disWalk.UP=true;
		disWalk.DOWN=true;
		disWalk.RIGHT=true;
		disWalk.LEFT=true;
		break;
	}
}

void Character::MoveChar(Level level, bool forward, int dir, bool moveFrames)	// Move the character relative to the currently faced direction
{
	if(moveFrames)
		MoveFrames();
	int size=(int)((float)DEFAULTILESIZE/2);
	bool ok=true;

	// The code below handles both the movement, AND the collision detection

	switch(forward ? Data.Facing : dir)
	{
	case UP:
		if(Data.Pos.Y-size>0 && disWalk.UP)
		{
			if(Data.TilePos.Y>0)
			{
				if(Data.Pos.Y%DEFAULTILESIZE-size<0)
				{
					if(level.IsOn(Data.TilePos.X,Data.TilePos.Y-1))
						ok=false;
					if(Data.Pos.X%DEFAULTILESIZE<size-8 && level.IsOn(Data.TilePos.X-1,Data.TilePos.Y-1))
						ok=false;
					if(Data.Pos.X%DEFAULTILESIZE>size+8 && level.IsOn(Data.TilePos.X+1,Data.TilePos.Y-1))
						ok=false;
				}
			}
		}
		else
			ok=false;

		if(ok)
			Data.Pos.Y-=Data.Speed;
		break;
	case DOWN:
		if(Data.Pos.Y+size<SCREENY-16 && disWalk.DOWN)
		{
			if(Data.TilePos.Y<DEFAULTILEY-1)
			{
				if(Data.Pos.Y%DEFAULTILESIZE+size/2-4>=DEFAULTILESIZE)
				{
					if(level.IsOn(Data.TilePos.X,Data.TilePos.Y+1))
						ok=false;
					if(Data.Pos.X%DEFAULTILESIZE<size-8 && level.IsOn(Data.TilePos.X-1,Data.TilePos.Y+1))
						ok=false;
					if(Data.Pos.X%DEFAULTILESIZE>size+8 && level.IsOn(Data.TilePos.X+1,Data.TilePos.Y+1))
						ok=false;
				}
			}
		}
		else
			ok=false;
		if(ok)
			Data.Pos.Y+=Data.Speed;
		break;
	case RIGHT:
		if(Data.Pos.X+size<SCREENX && disWalk.RIGHT)
		{
			if(Data.TilePos.X<DEFAULTILEX-1)
			{
				if(Data.Pos.X%DEFAULTILESIZE+size>=DEFAULTILESIZE)
				{
					if(level.IsOn(Data.TilePos.X+1,Data.TilePos.Y))
						ok=false;
					if(Data.Pos.Y%DEFAULTILESIZE<size-8 && level.IsOn(Data.TilePos.X+1,Data.TilePos.Y-1))
						ok=false;
					if(Data.Pos.Y%DEFAULTILESIZE>size*2 && level.IsOn(Data.TilePos.X+1,Data.TilePos.Y+1))
						ok=false;
				}
			}
		}
		else
			ok=false;
		if(ok)
			Data.Pos.X+=Data.Speed;
		break;
	case LEFT:
		if(Data.Pos.X-size>0 && disWalk.LEFT)
		{
			if(Data.TilePos.X>0)
			{
				if(Data.Pos.X%DEFAULTILESIZE-size<0)
				{
					if(level.IsOn(Data.TilePos.X-1,Data.TilePos.Y))
						ok=false;
					if(Data.Pos.Y%DEFAULTILESIZE<size-8 && level.IsOn(Data.TilePos.X-1,Data.TilePos.Y-1))
						ok=false;
					if(Data.Pos.Y%DEFAULTILESIZE>size*2 && level.IsOn(Data.TilePos.X-1,Data.TilePos.Y+1))
						ok=false;
				}
			}
		}
		else
			ok=false;
		if(ok)
			Data.Pos.X-=Data.Speed;
		break;
	}
	Data.TilePos.X=(int)((float)Data.Pos.X/DEFAULTILESIZE);
	Data.TilePos.Y=(int)((float)Data.Pos.Y/DEFAULTILESIZE);
}

void Character::Debug(Level level)
{
	main.glPrint(0,DEFAULTILESIZE,0,"Posx: %d Posy: %d",Data.Pos.X,Data.Pos.Y);
	main.glPrint(0,48,0,"Tilex: %d Tiley: %d",Data.TilePos.X,Data.TilePos.Y);
	main.glPrint(0,64,0,"Tile Posx: %d Tile Posy: %d",Data.Pos.X%DEFAULTILESIZE,Data.Pos.Y%DEFAULTILESIZE);
	main.glPrint(0,80,0,"Tile On? %d",level.IsOn(Data.TilePos.X,Data.TilePos.Y));
	main.glPrint(0,96,0,"Facing: %i",Data.Facing);
}