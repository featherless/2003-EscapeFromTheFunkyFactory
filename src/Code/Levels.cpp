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

#include "OpenGl.h"
extern OpenGL main;

#include "Levels.h"

void Level::PrintTime()
{
	main.glPrint(0,582,0,"%s",ConvertTime(GetTickCount()-TimerBegin-OverallPausedTime));
}

char* Level::ConvertTime(unsigned long Time)
{
	char* ReturnBuff=new char[1024];
	int Seconds=(int)(Time/1000);
	int Minutes=(int)(Seconds/60);
	Seconds-=(Minutes*60);
	if(Seconds>=10)
		sprintf(ReturnBuff,"%d:%d",Minutes,Seconds);
	else
		sprintf(ReturnBuff,"%d:0%d",Minutes,Seconds);
	return ReturnBuff;
}

void Level::Init(int TEXID)	// Accepts a Texture ID number
{
		Data.TEXID=TEXID;

		glBindTexture(GL_TEXTURE_2D, main.texture[TEXID]);

		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&Data.BmpSize.X);
		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&Data.BmpSize.Y);
}

void Level::DrawTile(int x, int y, int z, int Tile)	// Used mainly for the level editor
{
	Tile--;
	float size=DEFAULTILESIZE/2;
	int numx=Data.BmpSize.X/DEFAULTILESIZE;
	int numy=Data.BmpSize.Y/DEFAULTILESIZE;
	float u=(float)(Tile%numx);
	float v=(float)((int)(Tile/numx));

	glBindTexture(GL_TEXTURE_2D, main.texture[Data.TEXID]);
	
	float eu=u+1;
	float ev=v+1;
	
	u/=(float)numx;
	v/=(float)numy;
	eu/=(float)numx;
	ev/=(float)numy;

	glLoadIdentity();
	glTranslated(x,y,z);
	glColor3f(GlobalCol.r,GlobalCol.g,GlobalCol.b);
	
	if(Tile>=0)
	{
		glBegin(GL_QUADS);
			glTexCoord2f(eu,1-v);	glVertex2d( size,-size);
			glTexCoord2f(u,1-v);	glVertex2d(-size,-size);
			glTexCoord2f(u,1-ev);	glVertex2d(-size, size);
			glTexCoord2f(eu,1-ev);	glVertex2d( size, size);
		glEnd();
	}
	else	// Draw a white square
	{
		main.Texture2D(FALSE);
		glBegin(GL_QUADS);
			glVertex2d( size,-size);
			glVertex2d(-size,-size);
			glVertex2d(-size, size);
			glVertex2d( size, size);
		glEnd();
		main.Texture2D(TRUE);
	}

}

void Level::DrawLevel()
{
	float size=DEFAULTILESIZE/2;
	int numx=Data.BmpSize.X/DEFAULTILESIZE;
	int numy=Data.BmpSize.Y/DEFAULTILESIZE;
	int total=numx*numy;

	glBindTexture(GL_TEXTURE_2D, main.texture[Data.TEXID]);

	glLoadIdentity();

	glTranslated(size,size,0);	// Center the tiles
	glColor3f(GlobalCol.r,GlobalCol.g,GlobalCol.b);

	float u,v,eu,ev;
	int TexPos;

	for(int a=0;a<DEFAULTILEY;a++)
	{
		for(int b=0;b<DEFAULTILEX;b++)
		{
			TexPos=Data.TilesInfo[a*DEFAULTILEX+b]-1;

			if(TexPos>=total)	// Make sure we won't crash the program by trying to draw something
			{
				Data.TilesInfo[a*DEFAULTILEX+b]=total-1;	// that's over the edge of the picture
				TexPos=total-2;
			}

			if(TexPos>-1)		// Only draw the tile if there's an image to be drawn
			{
				u=(float)(TexPos%numx);
				v=(float)((int)(TexPos/numx));
				
				eu=u+1;
				ev=v+1;
				
				u/=(float)numx;
				v/=(float)numy;
				eu/=(float)numx;
				ev/=(float)numy;
				
				glBegin(GL_QUADS);
					glTexCoord2f(eu,1-v);glVertex2d( size,-size);
					glTexCoord2f(u,1-v);	glVertex2d(-size,-size);
					glTexCoord2f(u,1-ev);	glVertex2d(-size, size);
					glTexCoord2f(eu,1-ev);	glVertex2d( size, size);
				glEnd();
			}
			glTranslated(DEFAULTILESIZE,0,0);
		}
		glTranslated(-800,DEFAULTILESIZE,0);
	}
}