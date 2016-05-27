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

#include "Objects.h"

void Object::Init(unsigned int TEXID, iCOORD StartPos, iCOORD Pos, bool Bounce, unsigned int numAnims, int Face, unsigned int FrameSpeed)
{
	Data.StartPos=StartPos;
	Data.TEXID=TEXID;
	Data.Pos=Pos;
	Data.Bounce=Bounce;
	Data.numAnims=numAnims;
	Data.Face=Face;
	Data.Frame=0;
	Data.FrameCounter=0;
	Data.up=false;
	Data.FrameSpeed=FrameSpeed;

	glBindTexture(GL_TEXTURE_2D, main.texture[TEXID]);

	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&Data.BmpSize.X);
	glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&Data.BmpSize.Y);
}

void Object::MoveFrames()
{
	if(Data.numAnims>1)
	{
		if(Data.up)
			Data.FrameCounter+=Data.FrameSpeed;
		else
			Data.FrameCounter-=Data.FrameSpeed;

		if(Data.FrameCounter>120)
		{
			Data.Frame++;
			Data.FrameCounter=0;
		}
		if(Data.FrameCounter<0)
		{
			Data.Frame--;
			Data.FrameCounter=120;
		}
		if(Data.Frame<0)
		{
			Data.Frame=0;
			Data.up=true;
		}
		if(Data.Frame>=(int)Data.numAnims)
		{
			Data.Frame=Data.numAnims-1;
			if(Data.Bounce)
				Data.up=false;
			else
				Data.Frame=0;
		}
	}
}

void Object::DrawObjectToPos(int x, int y, int z)
{
	glLoadIdentity();			// Reset our drawing matrix
	glTranslated(x,y,z);	// Move to the central position of the character

	Draw();
}

void Object::DrawObject()
{
	glLoadIdentity();			// Reset our drawing matrix
	glTranslated(Data.Pos.X,Data.Pos.Y,2);

	Draw();
}

void Object::Draw()
{
	if(Data.StartPos.X==160 && Data.StartPos.Y==0)
	{
		float size=(float)DEFAULTILESIZE/2;
		float u=(float)Data.StartPos.X,v=(float)Data.Frame*32+Data.StartPos.Y;
		
		float eu=u+DEFAULTDIM*2,ev=v+DEFAULTDIM;	// Extended x/y values in the texture
		
		u/=(float)Data.BmpSize.X;	// Get the exact texture coordinates of the current sprite frame
		v/=(float)Data.BmpSize.Y;
		eu/=(float)Data.BmpSize.X;
		ev/=(float)Data.BmpSize.Y;
		
		main.Texture2D(TRUE);		// Make sure texturing is ON
		
		int rot=0;
		
		switch(Data.Face)
		{
		case DOWN:
			{
				float temp=ev;
				ev=v;
				v=temp;
			}
			break;
		case RIGHT:
			glRotated(90,0,0,1);
			break;
		case LEFT:
			glRotated(270,0,0,1);
			break;
		};
		
		glBindTexture(GL_TEXTURE_2D, main.texture[Data.TEXID]);
		glBegin(GL_QUADS);
			glTexCoord2f( u,1-ev);	glVertex2f(-size, size);
			glTexCoord2f(eu,1-ev);	glVertex2f( size*2+size, size);
			glTexCoord2f(eu,1-v);	glVertex2f( size*2+size,-size);
			glTexCoord2f( u,1-v);	glVertex2f(-size,-size);
		glEnd();
	}
	else if(Data.StartPos.X>-1 && Data.StartPos.Y>-1)
	{
		float size=(float)DEFAULTILESIZE/2;
		float u=(float)Data.Frame*32+Data.StartPos.X,v=(float)Data.StartPos.Y;
		
		float eu=u+DEFAULTDIM,ev=v+DEFAULTDIM;	// Extended x/y values in the texture
		
		u/=(float)Data.BmpSize.X;	// Get the exact texture coordinates of the current sprite frame
		v/=(float)Data.BmpSize.Y;
		eu/=(float)Data.BmpSize.X;
		ev/=(float)Data.BmpSize.Y;
		
		main.Texture2D(TRUE);		// Make sure texturing is ON
		
		int rot=0;
		
		switch(Data.Face)
		{
		case DOWN:
			{
				float temp=ev;
				ev=v;
				v=temp;
			}
			break;
		case RIGHT:
			glRotated(90,0,0,1);
			break;
		case LEFT:
			glRotated(270,0,0,1);
			break;
		};
		
		glBindTexture(GL_TEXTURE_2D, main.texture[Data.TEXID]);
		glBegin(GL_QUADS);
			glTexCoord2f( u,1-ev);	glVertex2f(-size, size);
			glTexCoord2f(eu,1-ev);	glVertex2f( size, size);
			glTexCoord2f(eu,1-v);	glVertex2f( size,-size);
			glTexCoord2f( u,1-v);	glVertex2f(-size,-size);
		glEnd();
	}
	else if(Data.StartPos.X==-1 && Data.StartPos.Y==-1)
	{
		main.Texture2D(FALSE);
		
		glBegin(GL_QUADS);

		glColor3d(0,0,0);
			glVertex2d(-16, 16);
			glVertex2d( 16, 16);
		glColor3f(0.5f,0.5f,0.5f);
			glVertex2d( 16,-16);
		glColor3d(1,1,1);
			glVertex2d(-16,-16);

		glEnd();
		
		main.Texture2D(TRUE);
	}
	else if(Data.StartPos.X==-2 && Data.StartPos.Y==-2)
	{
		main.Texture2D(FALSE);
		
		glBegin(GL_QUADS);

		glColor3f(0.5f,0.0f,0.0f);
			glVertex2d(-16, 16);
			glVertex2d( 16, 16);
		glColor3f(0.75f,0.5f,0.5f);
			glVertex2d( 16,-16);
		glColor3f(1.0f,1.0f,1.0f);
			glVertex2d(-16,-16);

		glEnd();
		
		main.Texture2D(TRUE);
	}
}