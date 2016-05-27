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

// Don't forget this site!!
// http://www.thedevelopersalliance.com/vault/start.html

#include "OpenGl.h"
#include <fstream.h>
extern OpenGL main;

extern bool done;

#include "LevelEditor.h"

LevelEditor::LevelEditor()
{
	Data.PlaceMode=0;
	Editing=Info=Menu=PlacedChar=FillGrid=ShowGrid=ColDet=CreatingObj=DropPos=SelectTile=Selected=false;
	Tile=0;
	LastVal=0;
	FillTile=0;
	Select[0].X=Select[0].Y=Select[1].X=Select[1].Y=-1;
	strcpy(TileName,"tiles.bmp");
	strcpy(ObjeName,"objects.bmp");
	strcpy(PlayName,"sprites.bmp");
	strcpy(TransName,"");
	strcpy(NextLevel,"");
}

LevelEditor::~LevelEditor()
{
	ObsMan.Clear();
}

bool LevelEditor::RunLevelEditor()
{
	// SelectTile::Pressing T to select a square of tiles
	// CreatingObj::When the user is modifying an object
	// ColDet::Drawing the collision detection lines
	// FillGrid::The fill grid box
	DrawMouse();
//	Debug();
	
	ObsMan.DrawObstacles();

	if(main.keys[VK_RETURN] && !main.lkeys[VK_RETURN])
		Info=!Info;
	if(main.keys[VK_ESCAPE] && !main.lkeys[VK_ESCAPE] && !FillGrid && !CreatingObj && !SelectTile)
		Menu=!Menu;

	if(Menu)
	{
		DrawMenu();
		if(!HandleMenu())
			return false;
	}
	else if(!FillGrid && !ColDet && !CreatingObj && !SelectTile)
	{
		HandleClicks();
		HandleMouse();
	}

	if(ShowGrid)
		DrawGrid();

	if(!Menu && Info && !FillGrid)
		DrawInfo();

	LevelMap.DrawLevel();

	if(PlacedChar)
	{
		MainChar.DrawCharToScreen();
		MainChar.MoveFrames();
	}

	if(main.keys['S'])
	{
		main.keys['S']=false;
		ObsMan.SynchAll();
	}

	if(main.keys['T'] && !CreatingObj && !DropPos)
	{
		Menu=false;
		main.keys['T']=false;
		SelectTile=!SelectTile;
		if(!SelectTile)
			Select[0].X=Select[0].Y=Select[1].X=Select[1].Y=-1;
	}

	if(!CreatingObj && !DropPos && !SelectTile)
	{
		if(FillGrid)
			Fill();

		if(ColDet)
			SetColDet();

		if(main.keys['G'])
		{
			Menu=false;
			main.keys['G']=false;
			ShowGrid=!ShowGrid;
		}
		
		if(main.keys['F'])
		{
			Menu=false;
			main.keys['F']=false;
			FillGrid=!FillGrid;
		}
		
		if(main.keys['C'])
		{
			Menu=false;
			main.keys['C']=false;
			ColDet=!ColDet;
		}
		
		if(main.keys['S'] && main.keys[VK_CONTROL])
		{
			Menu=false;
			main.keys[VK_CONTROL]=main.keys['S']=false;
			SaveLevel();
		}
		
		if(main.keys['L'] && main.keys[VK_CONTROL])
		{
			Menu=false;
			main.keys[VK_CONTROL]=main.keys['L']=false;
			LoadLevel();
		}
		
		if(main.keys['1'])
		{
			main.keys['1']=false;
			Data.PlaceMode=0;
		}
		
		if(main.keys['2'])
		{
			main.keys['2']=false;
			Data.PlaceMode=1;
		}
		
		if(main.keys['3'])
		{
			main.keys['3']=false;
			Data.PlaceMode=2;
		}
	}
	else if(SelectTile)
		SelectTiles();
	else if(!DropPos && !SelectTile)
		CreateObs();
	else if(DropPos)
		EditDropPos();
	
	if(!DropPos && !SelectTile)
	{
		if(ObsMan.GetNumObs()>0)
		{
			if(main.keys['N'])
			{
				main.keys['N']=false;
				selectedObj++;
				if(selectedObj>=ObsMan.GetNumObs())
					selectedObj=0;
				obs=ObsMan.GetObstacle(selectedObj);
				CreatingObj=true;
			}
			if(main.keys['M'])
			{
				main.keys['M']=false;
				selectedObj--;
				if(selectedObj<0)
					selectedObj=ObsMan.GetNumObs()-1;
				obs=ObsMan.GetObstacle(selectedObj);
				CreatingObj=true;
			}
		}
	}
	return true;
}

void LevelEditor::Debug()
{
	glColor3d(0,0,1);
	main.glPrint(0,0,0,"%dx%d",Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE);
	main.glPrint(0,16,0,"Editing Mode: %d  Editing? %d",Data.PlaceMode,Editing);
	main.glPrint(0,540,0,"EFFF Level Editor by Jeff Verkoeyen %s",CURRVERSION);
	glColor3d(1,1,1);
}

void LevelEditor::SelectTiles()
{
	if(!Selected)
	{
		if(Select[0].X==-1 && Select[0].Y==-1)
		{
			if(Data.mouse->Left)
			{
				Select[0].X=Data.mouse->Pos.X;
				Select[0].Y=Data.mouse->Pos.Y;
			}
		}
		if(!(Select[0].X==-1 && Select[0].Y==-1))
		{
			if(Data.mouse->Left)
			{
				Select[1].X=Data.mouse->Pos.X;
				Select[1].Y=Data.mouse->Pos.Y;
			}
			else
				Selected=true;

			glLoadIdentity();
			glTranslated(0,0,6);

			glColor3d(0,1,1);

			main.Texture2D(FALSE);
			glLineWidth(1);
			main.Culling(FALSE);

			glBegin(GL_LINE_LOOP);
				glVertex2d(Select[0].X, Select[0].Y);
				glVertex2d(Select[1].X, Select[0].Y);
				glVertex2d(Select[1].X, Select[1].Y);
				glVertex2d(Select[0].X, Select[1].Y);
			glEnd();

			main.Culling(TRUE);

			main.Texture2D(TRUE);
		}
	}
	else
	{
		glLoadIdentity();
		glTranslated(400,300,6);

		main.Texture2D(FALSE);

		glBegin(GL_QUADS);
			glColor3d(0,0,1);
			glVertex2d(-100,50);
			glVertex2d( 100,50);
			glVertex2d( 100,-50);
			glVertex2d(-100,-50);
		glEnd();

		main.Texture2D(TRUE);

		glColor3d(1,0,0);
		main.glPrint(335,250,0,"Fill Grid");
		main.glPrint(330,320,0,"%d",FillTile);

		if(Data.mouse->Pos.X>370 && Data.mouse->Pos.X<428 && Data.mouse->Pos.Y>310 && Data.mouse->Pos.Y<326)
		{
			glColor3d(1,1,1);
			if(Data.mouse->Left && !Data.mouse->hLeft)
			{
				Data.mouse->hLeft=true;
				int x1=(int)(Select[0].X/32),x2=(int)(Select[1].X/32);
				int y1=(int)(Select[0].Y/32),y2=(int)(Select[1].Y/32);

				if(Select[0].X>Select[1].X)
				{
					int temp=x1;
					x1=x2;
					x2=temp;
				}
				if(Select[0].Y>Select[1].Y)
				{
					int temp=y1;
					y1=y2;
					y2=temp;
				}

				for(int a=x1;a<=x2;a++)
				{
					for(int b=y1;b<=y2;b++)
						LevelMap.SetTileData(a,b,FillTile);
				}
				SelectTile=false;
			}
		}
		else
			glColor3d(0,1,0);

		main.glPrint(370,310,0,"Fill");

		if(Data.mouse->Pos.X>360 && Data.mouse->Pos.X<445 && Data.mouse->Pos.Y>326 && Data.mouse->Pos.Y<342)
		{
			glColor3d(1,1,1);
			if(Data.mouse->Left && !Data.mouse->hLeft)
			{
				Data.mouse->hLeft=true;
				SelectTile=false;
			}
		}
		else
			glColor3d(0,1,0);

		main.glPrint(360,326,0,"Cancel");

		glColor3d(1,1,1);

		LevelMap.DrawTile(340,300,9,FillTile);

		main.SetTexture(3);

		glLoadIdentity();
		glTranslated(460,280,8);

		if(Data.mouse->Pos.X>444 && Data.mouse->Pos.X<476 && Data.mouse->Pos.Y>264 && Data.mouse->Pos.Y<296)
		{
			glColor3d(0,1,1);
			if(Data.mouse->Left && !Data.mouse->hLeft)
			{
				Data.mouse->hLeft=true;
				FillTile++;
				if(FillTile>LevelMap.GetMaxTiles())
					FillTile=0;
			}
		}
		else
			glColor3d(1,0,0);

		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2d( 16,-16);
			glTexCoord2f(1,1);	glVertex2d(-16,-16);
			glTexCoord2f(1,0);	glVertex2d(-16, 16);
			glTexCoord2f(0,0);	glVertex2d( 16, 16);
		glEnd();

		glLoadIdentity();
		glTranslated(460,320,8);

		if(Data.mouse->Pos.X>444 && Data.mouse->Pos.X<476 && Data.mouse->Pos.Y>304 && Data.mouse->Pos.Y<336)
		{
			glColor3d(0,1,1);
			
			if(Data.mouse->Left && !Data.mouse->hLeft)
			{
				Data.mouse->hLeft=true;
				FillTile--;
				if(FillTile<0)
					FillTile=LevelMap.GetMaxTiles();
			}
		}
		else
			glColor3d(1,0,0);

		glBegin(GL_QUADS);
			glTexCoord2f(0,0);	glVertex2d( 16,-16);
			glTexCoord2f(1,0);	glVertex2d(-16,-16);
			glTexCoord2f(1,1);	glVertex2d(-16, 16);
			glTexCoord2f(0,1);	glVertex2d( 16, 16);
		glEnd();

		if(main.keys[VK_ESCAPE])
			SelectTile=false;

		glColor3d(1,1,1);

		if(!SelectTile)
		{
			Select[0].X=Select[1].X=Select[0].Y=Select[1].Y=-1;
			Selected=false;
		}
	}
	if(!(Select[0].X==-1 && Select[0].Y==-1))
	{
		glLoadIdentity();
		glTranslated(0,0,6);

		glColor3d(0,1,1);

		main.Texture2D(FALSE);
		glLineWidth(1);
		main.Culling(FALSE);

		glBegin(GL_LINE_LOOP);
			glVertex2d(Select[0].X, Select[0].Y);
			glVertex2d(Select[1].X, Select[0].Y);
			glVertex2d(Select[1].X, Select[1].Y);
			glVertex2d(Select[0].X, Select[1].Y);
		glEnd();

		main.Culling(TRUE);

		main.Texture2D(TRUE);
	}
}

void LevelEditor::EditDropPos()
{
	glColor3d(1,0,0);
	main.glPrint(0,128,0,"Drop Pos X: %d  Drop Pos Y:  %d",(int)(Data.mouse->Pos.X/32),(int)(Data.mouse->Pos.Y/32));
	if(Data.mouse->Left && !Data.mouse->hLeft)
	{
		Data.mouse->hLeft=true;
		obs->iDirection=0;
		obs->iDirection=(int)(Data.mouse->Pos.X/32) | ((int)(Data.mouse->Pos.Y/32) << 8);
		DropPos=false;
	}
	glColor3d(1,1,1);
}

void LevelEditor::CreateObs()
{
	if(tempOn)
	{
		if(!main.keys[VK_SHIFT])
		{
			obs->SetProps(Data.mouse->Pos, ObstacleTemp.Dir, ObstacleTemp.State, ObstacleTemp.Type);
			obs->Init(2, Data.mouse->Pos, ObstacleTemp.Face);
		}
		else
		{
			iCOORD tempPos;
			tempPos.X=Data.mouse->Pos.X-Data.mouse->Pos.X%32+16;
			tempPos.Y=Data.mouse->Pos.Y-Data.mouse->Pos.Y%32+16;
			obs->SetProps(tempPos, ObstacleTemp.Dir, ObstacleTemp.State, ObstacleTemp.Type);
			obs->Init(2, tempPos, ObstacleTemp.Face);
		}

		if(!Data.mouse->Left)
			tempOn=false;
	}
	else
	{
		glLoadIdentity();
		glTranslated(400,300,6);
		
		main.Texture2D(FALSE);
		
		glEnable(GL_BLEND);

		glBegin(GL_QUADS);
		glColor4f(0.0f,0.0f,1.0f,0.7f);
			glVertex2d(-200,200);
			glVertex2d( 200,200);
			glVertex2d( 200,-200);
			glVertex2d(-200,-200);
		glEnd();

		glLoadIdentity();
		glTranslated(obs->pPosition.X,obs->pPosition.Y-16,7);
		glColor4f(1,1,1,0.5f);
		glBegin(GL_TRIANGLES);
			glVertex2d( 0, 16);
			glVertex2d( 16,-16);
			glVertex2d(-16,-16);
		glEnd();
		
		glDisable(GL_BLEND);

		main.Texture2D(TRUE);
		
		glColor3d(1,1,1);

		main.glPrint(100,0,"Edit Object");
		main.glPrint(470,150,0,"Type");

		if(Info)		// Debugging info
			main.glPrint(116,0,"ID #: %d",selectedObj);

		if(obs->iType!=OBS_LASER)
			obs->Disp.DrawObjectToPos(250,150,9);
		else
			obs->DrawLaserToPos(250,150,9);

		glColor3d(1,1,1);

		if(Data.mouse->Pos.X>360 && Data.mouse->Pos.X<445 && Data.mouse->Pos.Y>480 && Data.mouse->Pos.Y<496)
		{
			glColor3d(1,1,1);
			if(Data.mouse->Left && !Data.mouse->hLeft)
			{
				Data.mouse->hLeft=true;
				CreatingObj=false;
				ObstacleTemp.Dir=obs->iDirection;
				ObstacleTemp.State=obs->bState;
				ObstacleTemp.Type=obs->iType;
				ObstacleTemp.Face=obs->Disp.GetFace();
			}
		}
		else
			glColor3d(0,1,0);
		
		main.glPrint(360,480,0,"Done");
		
		if(Data.mouse->Pos.X>356 && Data.mouse->Pos.X<441 && Data.mouse->Pos.Y>460 && Data.mouse->Pos.Y<476)
		{
			glColor3d(1,1,1);
			if(Data.mouse->Left && !Data.mouse->hLeft)
			{
				Data.mouse->hLeft=true;
				CreatingObj=false;
				ObsMan.RemObstacle(selectedObj);
				return;
			}
		}
		else
			glColor3d(0,1,0);

		if(main.keys[VK_DELETE])
		{
			CreatingObj=false;
			ObsMan.RemObstacle(selectedObj);
			return;
		}
		
		main.glPrint(356,460,0,"Delete");

		if(obs->iType!=OBS_BUTTON && obs->iType!=OBS_MULTI && obs->iType!=OBS_TIMER  && obs->iType!=OBS_GOAL)
		{
			if(Data.mouse->Pos.X>350 && Data.mouse->Pos.X<441 && Data.mouse->Pos.Y>440 && Data.mouse->Pos.Y<456)
			{
				glColor3d(1,1,1);
				if(Data.mouse->Left && !Data.mouse->hLeft)
				{
					Data.mouse->hLeft=true;
					obs->ToggleState();
				}
			}
			else
				glColor3d(0,1,0);
			
			if(obs->bState)
				main.glPrint(350,440,0,"Disable");
			else
				main.glPrint(356,440,0,"Enable");
		}

		glLoadIdentity();
		glTranslated(550,140,8);
		
		if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>124 && Data.mouse->Pos.Y<156)
		{
			glColor3d(0,1,1);
			if(Data.mouse->Left && !Data.mouse->hLeft)
			{
				Data.mouse->hLeft=true;
				obs->iType++;
				if(obs->iType>=OBS_NUMOBS)
					obs->iType=0;
				obs->bState=true;
				int face;
				switch(obs->iType)
				{
				case OBS_CONVEYOR:
					obs->iDirection=DIR_NS;
					obs->bState=true;
					face=UP;
					break;
				case OBS_PIT:
					obs->iDirection=0;
					obs->bState=true;
					face=UP;
					break;
				case OBS_TURN:
					obs->iDirection=DIR_CC;
					obs->bState=true;
					face=DOWN;
					break;
				case OBS_BUTTON:
					obs->iDirection=0;
					obs->bState=false;
					face=UP;
					break;
				case OBS_MULTI:
					obs->iDirection=2<<24;
					obs->bState=false;
					face=UP;
					break;
				case OBS_HAMMER:
					obs->bState=true;
					face=UP;
					break;
				case OBS_LASER:
					obs->iDirection=2;
					obs->bState=true;
					face=DIR_N;
					break;
				case OBS_CRANE:
					obs->iDirection=0x00000A0A;
					obs->bState=true;
					face=DIR_N;
					break;
				case OBS_TIMER:
					obs->iDirection=0;
					obs->Disp.SetFace(0);
					obs->bState=true;
					face=0;
					break;
				}
				obs->SetProps(obs->pPosition, obs->iDirection, obs->bState, obs->iType);
				obs->Init(2, obs->pPosition, face);
			}
		}
		else
			glColor3d(1,0,0);

		main.SetTexture(3);

		glBegin(GL_QUADS);
			glTexCoord2f(0,1);	glVertex2d( 16,-16);
			glTexCoord2f(1,1);	glVertex2d(-16,-16);
			glTexCoord2f(1,0);	glVertex2d(-16, 16);
			glTexCoord2f(0,0);	glVertex2d( 16, 16);
		glEnd();
		
		glLoadIdentity();
		glTranslated(550,172,8);
		
		if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>156 && Data.mouse->Pos.Y<188)
		{
			glColor3d(0,1,1);
			
			if(Data.mouse->Left && !Data.mouse->hLeft)
			{
				Data.mouse->hLeft=true;
				obs->iType--;
				if(obs->iType<0)
					obs->iType=OBS_NUMOBS-1;
				obs->bState=true;
				int face;
				switch(obs->iType)
				{
				case OBS_CONVEYOR:
					obs->iDirection=DIR_NS;
					obs->bState=true;
					face=UP;
					break;
				case OBS_PIT:
					obs->iDirection=0;
					obs->bState=true;
					face=UP;
					break;
				case OBS_TURN:
					obs->iDirection=DIR_CC;
					obs->bState=true;
					face=DOWN;
					break;
				case OBS_BUTTON:
					obs->iDirection=0;
					obs->bState=false;
					face=UP;
					break;
				case OBS_MULTI:
					obs->iDirection=0;
					obs->bState=false;
					face=UP;
					break;
				case OBS_HAMMER:
					obs->bState=true;
					face=UP;
					break;
				case OBS_LASER:
					obs->iDirection=2;
					obs->bState=true;
					face=DIR_N;
					break;
				case OBS_CRANE:
					obs->iDirection=0x00000A0A;
					obs->bState=true;
					face=DIR_N;
					break;
				case OBS_TIMER:
					obs->iDirection=0;
					obs->bState=true;
					obs->Disp.SetFace(0);
					face=0;
					break;
				}
				obs->SetProps(obs->pPosition, obs->iDirection, obs->bState, obs->iType);
				obs->Init(2, obs->pPosition, face);
			}
		}
		else
			glColor3d(1,0,0);

		main.SetTexture(3);

		glBegin(GL_QUADS);
			glTexCoord2f(0,0);	glVertex2d( 16,-16);
			glTexCoord2f(1,0);	glVertex2d(-16,-16);
			glTexCoord2f(1,1);	glVertex2d(-16, 16);
			glTexCoord2f(0,1);	glVertex2d( 16, 16);
		glEnd();
		
		if(main.keys[VK_ESCAPE])
		{
			CreatingObj=false;
			ObstacleTemp.Dir=obs->iDirection;
			ObstacleTemp.State=obs->bState;
			ObstacleTemp.Type=obs->iType;
			ObstacleTemp.Face=obs->Disp.GetFace();
		}
		
		glColor3d(1,1,1);

		switch(obs->iType)
		{
		case OBS_CONVEYOR:
			main.glPrint(210,180,0,"Conveyor");
			main.glPrint(300,220,0,"Move Direction");

			if(Data.mouse->Pos.X>520 && Data.mouse->Pos.X<580 && Data.mouse->Pos.Y>220 && Data.mouse->Pos.Y<236)
			{
				glColor3d(1,1,1);
				if(Data.mouse->Left && !Data.mouse->hLeft)
				{
					Data.mouse->hLeft=true;
					int temp=obs->Disp.GetFace();
					temp++;
					if(temp>=NUM_DIRECTIONS)
						temp=0;
					if(temp==RIGHT || temp==UP)
						obs->bState=true;
					else if(temp==LEFT || temp==DOWN)
						obs->bState=false;
					if(temp==RIGHT || temp==LEFT)
						obs->SetProps(obs->pPosition, DIR_EW, obs->bState, obs->iType);
					else if(temp==UP || temp==DOWN)
						obs->SetProps(obs->pPosition, DIR_NS, obs->bState, obs->iType);
					obs->Init(2, obs->pPosition, temp);
				}
			}
			else
				glColor3d(0,1,0);
			
			if(obs->bState)
			{
				if(obs->iDirection==DIR_NS)
					main.glPrint(520,220,0,"Up");
				else if(obs->iDirection==DIR_EW)
					main.glPrint(520,220,0,"Right");
			}
			else
			{
				if(obs->iDirection==DIR_NS)
					main.glPrint(520,220,0,"Down");
				else if(obs->iDirection==DIR_EW)
					main.glPrint(520,220,0,"Left");
			}
			break;
		case OBS_LASER:
			main.glPrint(210,180,0,"Laser");
			main.glPrint(250,220,0,"Direction");
			main.glPrint(250,290,0,"Tile Length");

			if(Data.mouse->Pos.X>520 && Data.mouse->Pos.X<580 && Data.mouse->Pos.Y>220 && Data.mouse->Pos.Y<236)
			{
				glColor3d(1,1,1);
				if(Data.mouse->Left && !Data.mouse->hLeft)
				{
					Data.mouse->hLeft=true;
					int temp=obs->Disp.GetFace();
					temp++;
					if(temp>DIR_W)
						temp=DIR_N;
					obs->SetProps(obs->pPosition, obs->iDirection, obs->bState, obs->iType);
					obs->Init(2, obs->pPosition, temp);
				}
			}
			else
				glColor3d(0,1,0);
			
			if(obs->Disp.GetFace()==DIR_N)
				main.glPrint(520,220,0,"Up");
			else if(obs->Disp.GetFace()==DIR_S)
				main.glPrint(520,220,0,"Down");
			else if(obs->Disp.GetFace()==DIR_E)
				main.glPrint(520,220,0,"Right");
			else if(obs->Disp.GetFace()==DIR_W)
				main.glPrint(520,220,0,"Left");

			glLoadIdentity();
			glTranslated(550,290,8);
			
			if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>274 && Data.mouse->Pos.Y<306)
			{
				glColor3d(0,1,1);
				if(Data.mouse->Left && !Data.mouse->hLeft)
				{
					Data.mouse->hLeft=true;
					int temp=obs->iDirection & 0x000000FF;
					temp++;
					if(temp>=DEFAULTILEX)
						temp=DEFAULTILEX-1;
					obs->iDirection&=0xFFFFFF00;
					obs->iDirection|=temp;
				}
			}
			else
				glColor3d(1,0,0);
			
			main.SetTexture(3);
			
			glBegin(GL_QUADS);
				glTexCoord2f(0,1);	glVertex2d( 16,-16);
				glTexCoord2f(1,1);	glVertex2d(-16,-16);
				glTexCoord2f(1,0);	glVertex2d(-16, 16);
				glTexCoord2f(0,0);	glVertex2d( 16, 16);
			glEnd();

			glLoadIdentity();
			glTranslated(550,322,8);
			
			if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>306 && Data.mouse->Pos.Y<338)
			{
				glColor3d(0,1,1);
				if(Data.mouse->Left && !Data.mouse->hLeft)
				{
					Data.mouse->hLeft=true;
					int temp=obs->iDirection & 0x000000FF;
					temp--;
					if(temp<1)
						temp=1;
					obs->iDirection&=0xFFFFFF00;
					obs->iDirection|=temp;
				}
			}
			else
				glColor3d(1,0,0);
			
			main.SetTexture(3);
			
			glBegin(GL_QUADS);
				glTexCoord2f(0,0);	glVertex2d( 16,-16);
				glTexCoord2f(1,0);	glVertex2d(-16,-16);
				glTexCoord2f(1,1);	glVertex2d(-16, 16);
				glTexCoord2f(0,1);	glVertex2d( 16, 16);
			glEnd();

			break;
		case OBS_TURN:
			main.glPrint(210,180,0,"Turn-Table");
			main.glPrint(250,220,0,"Turn Direction");

			if(Data.mouse->Pos.X>460 && Data.mouse->Pos.X<600 && Data.mouse->Pos.Y>220 && Data.mouse->Pos.Y<236)
			{
				glColor3d(1,1,1);
				if(Data.mouse->Left && !Data.mouse->hLeft)
				{
					Data.mouse->hLeft=true;

					obs->ToggleState();

					obs->SetProps(obs->pPosition, obs->iDirection, obs->bState, obs->iType);
					obs->Init(2, obs->pPosition, obs->Disp.GetFace());
				}
			}
			else
				glColor3d(0,1,0);
			
			if(!obs->bState)
				main.glPrint(460,220,0,"CounterCW");
			else
				main.glPrint(460,220,0,"CW");
			break;
		case OBS_HAMMER:
			main.glPrint(210,180,0,"Hammer");
			main.glPrint(250,220,0,"Face Direction");

			if(Data.mouse->Pos.X>460 && Data.mouse->Pos.X<600 && Data.mouse->Pos.Y>220 && Data.mouse->Pos.Y<236)
			{
				glColor3d(1,1,1);
				if(Data.mouse->Left && !Data.mouse->hLeft)
				{
					Data.mouse->hLeft=true;

					int temp=obs->Disp.GetFace()+1;

					if(temp>=NUM_DIRECTIONS)
						temp=0;

					obs->SetProps(obs->pPosition, obs->iDirection, obs->bState, obs->iType);
					obs->Init(2, obs->pPosition, temp);
				}
			}
			else
				glColor3d(0,1,0);
			
			main.glPrint(460,220,0,"Rotate");
			break;
		case OBS_BUTTON:
			{
				main.glPrint(210,180,0,"Button");
				main.glPrint(200,220,0,"Affecting Obstacle");

				glLoadIdentity();
				glTranslated(550,220,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>204 && Data.mouse->Pos.Y<236)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						obs->iDirection++;
						if(obs->iDirection>=ObsMan.GetNumObs())
							obs->iDirection=0;
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,1);	glVertex2d( 16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16, 16);
					glTexCoord2f(0,0);	glVertex2d( 16, 16);
				glEnd();

				glLoadIdentity();
				glTranslated(550,252,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>236 && Data.mouse->Pos.Y<268)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						obs->iDirection--;
						if(obs->iDirection<0)
							obs->iDirection=ObsMan.GetNumObs()-1;
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,0);	glVertex2d( 16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16, 16);
					glTexCoord2f(0,1);	glVertex2d( 16, 16);
				glEnd();

				glColor3d(1,1,1);

				if(obs->iDirection >= ObsMan.GetNumObs())
					obs->iDirection=ObsMan.GetNumObs()-1;

				cObstacle* temp=ObsMan.GetObstacle(obs->iDirection);

				if(temp->iType!=OBS_LASER)
					temp->Disp.DrawObjectToPos(500,235,9);
				else
					temp->DrawLaserToPos(500,235,9);

				main.Texture2D(FALSE);

				glColor3d(0,0,1);

				glLoadIdentity();

				iCOORD temp2=temp->pPosition;

				glTranslated(temp2.X,temp2.Y-16,7);

				glBegin(GL_TRIANGLES);
					glVertex2d( 0, 16);
					glVertex2d( 16,-16);
					glVertex2d(-16,-16);
				glEnd();

				main.Texture2D(TRUE);
			}
			break;
		case OBS_PIT:
			main.glPrint(210,180,0,"Pit");
			break;
		case OBS_CRANE:
			main.glPrint(210,180,0,"Crane");
			main.glPrint(200,250,0,"Drop X: %d  Drop Y: %d",obs->iDirection & 0x000000FF,(obs->iDirection>>8) & 0x000000FF);

			if(Data.mouse->Pos.X>300 && Data.mouse->Pos.X<480 && Data.mouse->Pos.Y>220 && Data.mouse->Pos.Y<236)
			{
				glColor3d(1,1,1);
				if(Data.mouse->Left && !Data.mouse->hLeft)
				{
					Data.mouse->hLeft=true;
					DropPos=true;
				}
			}
			else
				glColor3d(0,1,0);
			
			main.glPrint(300,220,0,"Drop Position");
			break;
		case OBS_WALL:
			main.glPrint(210,180,0,"Wall");
			break;
		case OBS_GOAL:
			main.glPrint(210,180,0,"Goal");
			break;
		case OBS_TIMER:
			{
				main.glPrint(210,180,0,"Timer");
				main.glPrint(210,226,0,"Affecting Obstacle");

				glLoadIdentity();
				glTranslated(550,220,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>204 && Data.mouse->Pos.Y<236)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						int temp=obs->iDirection & 0x000000FF;
						temp++;
						if(temp>=ObsMan.GetNumObs())
							temp=0;
						obs->iDirection=obs->iDirection & 0xFFFFFF00;
						obs->iDirection=obs->iDirection | temp;
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,1);	glVertex2d( 16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16, 16);
					glTexCoord2f(0,0);	glVertex2d( 16, 16);
				glEnd();

				glLoadIdentity();
				glTranslated(550,252,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>236 && Data.mouse->Pos.Y<268)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						int temp=obs->iDirection & 0x000000FF;
						temp--;
						if(temp<0)
							temp=ObsMan.GetNumObs()-1;
						obs->iDirection=obs->iDirection & 0xFFFFFF00;
						obs->iDirection=obs->iDirection | temp;
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,0);	glVertex2d( 16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16, 16);
					glTexCoord2f(0,1);	glVertex2d( 16, 16);
				glEnd();

				glColor3d(1,1,1);

				if((obs->iDirection & 0x000000FF) >= ObsMan.GetNumObs())
				{
					obs->iDirection&=0xFFFFFF00;
					obs->iDirection|=(ObsMan.GetNumObs()-1);
				}

				cObstacle* temp=ObsMan.GetObstacle(obs->iDirection & 0x000000FF);

				if(temp->iType!=OBS_LASER)
					temp->Disp.DrawObjectToPos(500,235,9);
				else
					temp->DrawLaserToPos(500,235,9);

				main.Texture2D(FALSE);

				glColor3d(0,0,1);

				glLoadIdentity();

				iCOORD temp2=temp->pPosition;

				glTranslated(temp2.X,temp2.Y-16,7);

				glBegin(GL_TRIANGLES);
					glVertex2d( 0, 16);
					glVertex2d( 16,-16);
					glVertex2d(-16,-16);
				glEnd();

				main.Texture2D(TRUE);


				glLoadIdentity();
				glTranslated(550,300,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>284 && Data.mouse->Pos.Y<316)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left)
					{
						int temp=(obs->iDirection & 0x00FFFF00) >> 8;
						temp++;
						if(temp>=65535)	// Yes, I know, odd number, but it's the max 2 bytes can hold
							temp=0;
						obs->iDirection&=0xFF0000FF;
						obs->iDirection|=(temp << 8);
					}
					if(Data.mouse->Right && !Data.mouse->hRight)
					{
						Data.mouse->hRight=true;
						int temp=(obs->iDirection & 0x00FFFF00) >> 8;
						temp++;
						if(temp>=65535)	// Yes, I know, odd number, but it's the max 2 bytes can hold
							temp=0;
						obs->iDirection&=0xFF0000FF;
						obs->iDirection|=(temp << 8);
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,1);	glVertex2d( 16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16, 16);
					glTexCoord2f(0,0);	glVertex2d( 16, 16);
				glEnd();

				glLoadIdentity();
				glTranslated(550,332,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>316 && Data.mouse->Pos.Y<348)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						int temp=(obs->iDirection & 0x00FFFF00) >> 8;
						temp--;
						if(temp<0)
							temp=65534;
						obs->iDirection&=0x000000FF;
						obs->iDirection|=(temp<<8);
					}
					if(Data.mouse->Right && !Data.mouse->hRight)
					{
						Data.mouse->hRight=true;
						int temp=(obs->iDirection & 0x00FFFF00) >> 8;
						temp--;
						if(temp<0)
							temp=65534;
						obs->iDirection&=0x000000FF;
						obs->iDirection|=(temp<<8);
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,0);	glVertex2d( 16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16, 16);
					glTexCoord2f(0,1);	glVertex2d( 16, 16);
				glEnd();

				glColor3d(1,1,1);

				main.glPrint(210,310,0,"Delay (1/10 ms) %d",(obs->iDirection & 0x00FFFF00) >> 8);


				glLoadIdentity();
				glTranslated(550,364,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>348 && Data.mouse->Pos.Y<380)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						int temp=obs->Disp.GetFace();
						temp++;
						if(temp>=65535)
							temp=0;
						obs->Disp.SetFace(temp);
					}
					if(Data.mouse->Right && !Data.mouse->hRight)
					{
						Data.mouse->hRight=true;
						int temp=obs->Disp.GetFace();
						temp++;
						if(temp>=65535)
							temp=0;
						obs->Disp.SetFace(temp);
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,1);	glVertex2d( 16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16, 16);
					glTexCoord2f(0,0);	glVertex2d( 16, 16);
				glEnd();

				glLoadIdentity();
				glTranslated(550,396,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>380 && Data.mouse->Pos.Y<412)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						int temp=obs->Disp.GetFace();
						temp--;
						if(temp<0)
							temp=65534;
						obs->Disp.SetFace(temp);
					}
					if(Data.mouse->Right && !Data.mouse->hRight)
					{
						Data.mouse->hRight=true;
						int temp=obs->Disp.GetFace();
						temp--;
						if(temp<0)
							temp=65534;
						obs->Disp.SetFace(temp);
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,0);	glVertex2d( 16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16, 16);
					glTexCoord2f(0,1);	glVertex2d( 16, 16);
				glEnd();

				glColor3d(1,1,1);

				main.glPrint(210,370,0,"Offset (1/10 ms) %d",obs->Disp.GetFace());
			}
			break;
		case OBS_MULTI:
			{
				main.glPrint(210,180,0,"MultiObs");
				glColor3d(0,0,1);
				main.glPrint(200,220,0,"Obstacle #1 (blue)");

				glLoadIdentity();
				glTranslated(550,220,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>204 && Data.mouse->Pos.Y<236)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						int temp=obs->iDirection & 0x000000FF;
						temp++;
						if(temp>=ObsMan.GetNumObs())
							temp=0;
						obs->iDirection=obs->iDirection & 0xFFFFFF00;
						obs->iDirection=obs->iDirection | temp;
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,1);	glVertex2d( 16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16, 16);
					glTexCoord2f(0,0);	glVertex2d( 16, 16);
				glEnd();

				glLoadIdentity();
				glTranslated(550,252,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>236 && Data.mouse->Pos.Y<268)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						int temp=obs->iDirection & 0x000000FF;
						temp--;
						if(temp<0)
							temp=ObsMan.GetNumObs()-1;
						obs->iDirection=obs->iDirection & 0xFFFFFF00;
						obs->iDirection=obs->iDirection | temp;
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,0);	glVertex2d( 16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16, 16);
					glTexCoord2f(0,1);	glVertex2d( 16, 16);
				glEnd();

				glColor3d(1,0,0);
				main.glPrint(200,290,0,"Obstacle #2 (red)");

				glLoadIdentity();
				glTranslated(550,294,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>278 && Data.mouse->Pos.Y<310)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						int temp2=obs->iDirection & 0x0000FF00;
						obs->iDirection=obs->iDirection & 0xFFFF00FF;
						temp2=temp2>>8;
						temp2++;
						if(temp2>=ObsMan.GetNumObs())
							temp2=0;
						obs->iDirection=obs->iDirection | (temp2<<8);
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,1);	glVertex2d( 16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16, 16);
					glTexCoord2f(0,0);	glVertex2d( 16, 16);
				glEnd();

				glLoadIdentity();
				glTranslated(550,326,8);
				
				if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>310 && Data.mouse->Pos.Y<342)
				{
					glColor3d(0,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						int temp2=obs->iDirection & 0x0000FF00;
						obs->iDirection=obs->iDirection & 0xFFFF00FF;
						temp2=temp2>>8;
						temp2--;
						if(temp2<0)
							temp2=ObsMan.GetNumObs()-1;
						obs->iDirection=obs->iDirection | (temp2<<8);
					}
				}
				else
					glColor3d(1,0,0);

				main.SetTexture(3);

				glBegin(GL_QUADS);
					glTexCoord2f(0,0);	glVertex2d( 16,-16);
					glTexCoord2f(1,0);	glVertex2d(-16,-16);
					glTexCoord2f(1,1);	glVertex2d(-16, 16);
					glTexCoord2f(0,1);	glVertex2d( 16, 16);
				glEnd();

				if(GETBIT(obs->iDirection,24))
				{
					glColor3d(0,1,0);
					main.glPrint(200,360,0,"Obstacle #3 (green)");

					glLoadIdentity();
					glTranslated(550,360,8);
					
					if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>344 && Data.mouse->Pos.Y<376)
					{
						glColor3d(0,1,1);
						if(Data.mouse->Left && !Data.mouse->hLeft)
						{
							Data.mouse->hLeft=true;
							int temp2=obs->iDirection & 0x00FF0000;
							obs->iDirection=obs->iDirection & 0xFF00FFFF;
							temp2=temp2>>16;
							temp2++;
							if(temp2>=ObsMan.GetNumObs())
								temp2=0;
							obs->iDirection=obs->iDirection | (temp2<<16);
						}
					}
					else
						glColor3d(1,0,0);

					main.SetTexture(3);

					glBegin(GL_QUADS);
						glTexCoord2f(0,1);	glVertex2d( 16,-16);
						glTexCoord2f(1,1);	glVertex2d(-16,-16);
						glTexCoord2f(1,0);	glVertex2d(-16, 16);
						glTexCoord2f(0,0);	glVertex2d( 16, 16);
					glEnd();

					glLoadIdentity();
					glTranslated(550,390,8);
					
					if(Data.mouse->Pos.X>534 && Data.mouse->Pos.X<566 && Data.mouse->Pos.Y>374 && Data.mouse->Pos.Y<406)
					{
						glColor3d(0,1,1);
						if(Data.mouse->Left && !Data.mouse->hLeft)
						{
							Data.mouse->hLeft=true;
							int temp2=obs->iDirection & 0x00FF0000;
							obs->iDirection=obs->iDirection & 0xFF00FFFF;
							temp2=temp2>>16;
							temp2--;
							if(temp2<0)
								temp2=ObsMan.GetNumObs()-1;
							obs->iDirection=obs->iDirection | (temp2<<16);
						}
					}
					else
						glColor3d(1,0,0);

					main.SetTexture(3);

					glBegin(GL_QUADS);
						glTexCoord2f(0,0);	glVertex2d( 16,-16);
						glTexCoord2f(1,0);	glVertex2d(-16,-16);
						glTexCoord2f(1,1);	glVertex2d(-16, 16);
						glTexCoord2f(0,1);	glVertex2d( 16, 16);
					glEnd();
				}

				glColor3d(1,1,1);

				main.glPrint(200,430,0,"NumObstacles");

				if(Data.mouse->Pos.X>400 && Data.mouse->Pos.X<440 && Data.mouse->Pos.Y>430 && Data.mouse->Pos.Y<446)
				{
					glColor3d(1,1,1);
					if(Data.mouse->Left && !Data.mouse->hLeft)
					{
						Data.mouse->hLeft=true;
						if(GETBIT(obs->iDirection,24))
							obs->iDirection&=0x00FFFFFF;
						else
							obs->iDirection|=0x01000000;
					}
				}
				else
					glColor3d(0,1,0);
				
				if(GETBIT(obs->iDirection,24))
					main.glPrint(400,430,0,"3");
				else
					main.glPrint(400,430,0,"2");

				glColor3d(1,1,1);

				if((obs->iDirection & 0x000000FF) >= ObsMan.GetNumObs())
				{
					obs->iDirection&=0xFFFFFF00;
					obs->iDirection|=(ObsMan.GetNumObs()-1);
				}

				cObstacle* temp=ObsMan.GetObstacle(obs->iDirection & 0x000000FF);

				glColor3d(1,1,1);

				if(temp->iType!=OBS_LASER)
					temp->Disp.DrawObjectToPos(500,235,9);
				else
					temp->DrawLaserToPos(500,235,9);

				glLoadIdentity();			// Object 1

				iCOORD temp2=temp->pPosition;

				glTranslated(temp2.X,temp2.Y-16,7);

				main.Texture2D(FALSE);

				glColor3d(0,0,1);
				glBegin(GL_TRIANGLES);
					glVertex2d( 0, 16);
					glVertex2d( 16,-16);
					glVertex2d(-16,-16);
				glEnd();

				main.Texture2D(TRUE);

				if(((obs->iDirection & 0x0000FF00) >> 8) >= ObsMan.GetNumObs())
				{
					obs->iDirection&=0xFFFF00FF;
					obs->iDirection|=((ObsMan.GetNumObs()<<8)-1);
				}

				temp=ObsMan.GetObstacle((obs->iDirection & 0x0000FF00) >> 8);

				glColor3d(1,1,1);
				if(temp->iType!=OBS_LASER)
					temp->Disp.DrawObjectToPos(500,305,9);
				else
					temp->DrawLaserToPos(500,305,9);

				glLoadIdentity();			// Object 2

				temp2=temp->pPosition;

				glTranslated(temp2.X,temp2.Y-16,7);

				main.Texture2D(FALSE);

				glColor3d(1,0,0);
				glBegin(GL_TRIANGLES);
					glVertex2d( 0, 16);
					glVertex2d( 16,-16);
					glVertex2d(-16,-16);
				glEnd();

				main.Texture2D(TRUE);


				if(GETBIT(obs->iDirection,24))
				{
					if(((obs->iDirection & 0x00FF0000) >> 16) >= ObsMan.GetNumObs())
					{
						obs->iDirection&=0xFF00FFFF;
						obs->iDirection|=((ObsMan.GetNumObs()<<16)-1);
					}

					temp=ObsMan.GetObstacle((obs->iDirection & 0x00FF0000) >> 16);

					glColor3d(1,1,1);
					if(temp->iType!=OBS_LASER)
						temp->Disp.DrawObjectToPos(500,375,9);
					else
						temp->DrawLaserToPos(500,375,9);

					glLoadIdentity();			// Object 2

					temp2=temp->pPosition;

					glTranslated(temp2.X,temp2.Y-16,7);

					main.Texture2D(FALSE);

					glColor3d(0,1,0);
					glBegin(GL_TRIANGLES);
						glVertex2d( 0, 16);
						glVertex2d( 16,-16);
						glVertex2d(-16,-16);
					glEnd();
				}

				main.Texture2D(TRUE);
			}
			break;
		}
	}
	glColor4f(0,0,1,1);
}

void LevelEditor::SaveLevel()
{
	char FileName[MAX_PATH]=" ";
	char Filt1[]="EFFF Levels(*.eff)\0*.eff\0";
	char Filt2[]="eff";
	SaveFile(FileName,Filt1,Filt2);

	if(FileName[0]!=' ')
	{
		FILE *savefile;
		savefile = fopen(FileName,"wb");
		if(savefile)
		{
			char finalize;
			ShowCursor(TRUE);
			if(MessageBox(main.hWnd,"Finalize the Level?\r\nWARNING, if you finalize a level, it will not be able to be edited again.\r\nSuggest making a backup copy that is not finalized, just in case.","Finalize?",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				finalize=1;
			else
				finalize=0;
			ShowCursor(FALSE);

			fwrite(&finalize,sizeof(char),1,savefile);
			fwrite(&CURRVERSION,sizeof(char)*10,1,savefile);
			fwrite(&TileName,sizeof(char)*64,1,savefile);
			fwrite(&ObjeName,sizeof(char)*64,1,savefile);
			fwrite(&PlayName,sizeof(char)*64,1,savefile);

			fwrite(&PlayerStart,sizeof(iCOORD),1,savefile);

			int temp=ObsMan.GetNumObs();

			fwrite(&temp,sizeof(int),1,savefile);

			for(int a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
				fwrite(&LevelMap.Data.TilesInfo[a],sizeof(int),1,savefile);

			for(a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
				fwrite(&LevelMap.Data.ColDetect[a],sizeof(int),1,savefile);

			fwrite(&NextLevel,sizeof(char)*64,1,savefile);

			for(a=0;a<temp;a++)
			{
				obs=ObsMan.GetObstacle(a);
				fwrite(&obs->pPosition,sizeof(iCOORD),1,savefile);

				fwrite(&obs->iDirection,sizeof(int),1,savefile);
				fwrite(&obs->bState,sizeof(bool),1,savefile);
				fwrite(&obs->iType,sizeof(int),1,savefile);
				int temp2=obs->Disp.GetFace();
				fwrite(&temp2,sizeof(int),1,savefile);
			}
			fwrite(&TransName,sizeof(char)*64,1,savefile);
			fclose(savefile);
			Menu=false;
		}
	}
}

void LevelEditor::LoadLevel()
{
	char FileName[MAX_PATH]=" ";
	char Filt1[]="EFFF Levels(*.eff)\0*.eff\0";
	char Filt2[]="eff";
	LoadFile(FileName,Filt1,Filt2);

	if(FileName[0]!=' ')
	{
		FILE *savefile;
		savefile = fopen(FileName,"rb");
		if(savefile)
		{
			FileData SaveData;

			fread(&SaveData.Finalized,sizeof(char),1,savefile);

			if(SaveData.Finalized!=0)
			{
				main.Error("Error Loading Level",MB_OK|MB_ICONEXCLAMATION,"Could not load file.\r\nReason:\r\nFile has been finalized.");
				return;
			}

			char array[10];

			fread(&array,sizeof(char)*10,1,savefile);
			fread(&SaveData.TileName,sizeof(char)*64,1,savefile);
			fread(&SaveData.ObjeName,sizeof(char)*64,1,savefile);
			fread(&SaveData.PlayName,sizeof(char)*64,1,savefile);

			fread(&SaveData.PlayerStart,sizeof(iCOORD),1,savefile);

			fread(&SaveData.numObstacles,sizeof(int),1,savefile);

			for(int a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
				fread(&SaveData.LevelData[a],sizeof(int),1,savefile);

			for(a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
				fread(&SaveData.LevelColDat[a],sizeof(int),1,savefile);

			fread(&SaveData.NextLevel,sizeof(char)*64,1,savefile);

			for(a=0;a<SaveData.numObstacles;a++)
			{
				fread(&SaveData.Obstacles[a].Pos,sizeof(iCOORD),1,savefile);

				fread(&SaveData.Obstacles[a].Dir,sizeof(int),1,savefile);
				fread(&SaveData.Obstacles[a].State,sizeof(bool),1,savefile);
				fread(&SaveData.Obstacles[a].Type,sizeof(int),1,savefile);
				fread(&SaveData.Obstacles[a].Face,sizeof(int),1,savefile);
			}

			strcpy(PlayName,SaveData.PlayName);
			strcpy(TileName,SaveData.TileName);
			strcpy(ObjeName,SaveData.ObjeName);
			
			sColor Alpha;
			Alpha.r=255;
			Alpha.g=0;
			Alpha.b=255;
			
			main.SetAlpha(Alpha);
			
			main.LoadTexture(PlayName,0);
			main.LoadTexture(TileName,1);
			main.LoadTexture(ObjeName,2);

			strcpy(NextLevel,SaveData.NextLevel);

			for(a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
			{
				LevelMap.Data.TilesInfo[a]=SaveData.LevelData[a];
				LevelMap.Data.ColDetect[a]=SaveData.LevelColDat[a];
			}

			PlacedChar=true;

			PlayerStart=SaveData.PlayerStart;
			
			MainChar.Init(0,PlayerStart);
			MainChar.SetFace(DOWN);
			MainChar.SetFrameSpeed(4);

			ObsMan.Clear();
			for(a=0;a<SaveData.numObstacles;a++)
			{
				obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				obs->SetProps(SaveData.Obstacles[a].Pos, SaveData.Obstacles[a].Dir, SaveData.Obstacles[a].State, SaveData.Obstacles[a].Type);
				obs->Init(2, SaveData.Obstacles[a].Pos, SaveData.Obstacles[a].Face);
// Debug		main.Error("blah",0,"%dx%d",SaveData.Obstacles[a].Pos.X,SaveData.Obstacles[a].Pos.Y);
			}
			Menu=false;

			if(array[0]=='v')
			{
				for(a=0;a<9;a++)
					array[a]=array[a+1];
			}

			double temp=atof(array);

			if(temp<0.91)	// If the level is outdated.....
			{
				if(temp<0.5)
					strcpy(TransName,"");
				else
				{
					fread(&SaveData.Transition,sizeof(char)*64,1,savefile);
					strcpy(TransName,SaveData.Transition);
				}
				fclose(savefile);

				main.Error("Old Version",0,"This is an old version number, the level will now be updated.\nOld Version: %s\nCurrent Version: %s",array,CURRVERSION);
				
				savefile = fopen(FileName,"wb");
				if(savefile)
				{
					fwrite(&SaveData.Finalized,sizeof(char),1,savefile);
					fwrite(&CURRVERSION,sizeof(char)*10,1,savefile);
					fwrite(&SaveData.TileName,sizeof(char)*64,1,savefile);
					fwrite(&SaveData.ObjeName,sizeof(char)*64,1,savefile);
					fwrite(&SaveData.PlayName,sizeof(char)*64,1,savefile);
					
					fwrite(&SaveData.PlayerStart,sizeof(iCOORD),1,savefile);
					
					fwrite(&SaveData.numObstacles,sizeof(int),1,savefile);
					
					for(int a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
						fwrite(&SaveData.LevelData[a],sizeof(int),1,savefile);
					
					for(a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
						fwrite(&SaveData.LevelColDat[a],sizeof(int),1,savefile);
					
					fwrite(&SaveData.NextLevel,sizeof(char)*64,1,savefile);
					
					for(a=0;a<SaveData.numObstacles;a++)
					{
						fwrite(&SaveData.Obstacles[a].Pos,sizeof(iCOORD),1,savefile);
						if(SaveData.Obstacles[a].Type==OBS_MULTI)
							SaveData.Obstacles[a].Dir&=0x01FFFFFF;
						if(SaveData.Obstacles[a].Type==OBS_PIT)
							SaveData.Obstacles[a].Dir=0;
						fwrite(&SaveData.Obstacles[a].Dir,sizeof(int),1,savefile);
						fwrite(&SaveData.Obstacles[a].State,sizeof(bool),1,savefile);
						fwrite(&SaveData.Obstacles[a].Type,sizeof(int),1,savefile);
						if(SaveData.Obstacles[a].Type==OBS_PIT)
							SaveData.Obstacles[a].Face=UP;
						fwrite(&SaveData.Obstacles[a].Face,sizeof(int),1,savefile);
					}

					ObsMan.Clear();
					for(a=0;a<SaveData.numObstacles;a++)
					{
						obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
						obs->SetProps(SaveData.Obstacles[a].Pos, SaveData.Obstacles[a].Dir, SaveData.Obstacles[a].State, SaveData.Obstacles[a].Type);
						obs->Init(2, SaveData.Obstacles[a].Pos, SaveData.Obstacles[a].Face);
					}
					fwrite(&TransName,sizeof(char)*64,1,savefile);
					fclose(savefile);
					Menu=false;
				}
			}
			else
			{
				fread(&SaveData.Transition,sizeof(char)*64,1,savefile);
				fclose(savefile);
			}
		}
	}
}

void LevelEditor::DrawInfo()
{
	glColor3d(0,0,1);
	switch(Data.PlaceMode)
	{
	case OBJECT:
		if(!main.keys[VK_SHIFT])
			main.glPrint(0,0,0,"Position: %dx%d",Data.mouse->Pos.X,Data.mouse->Pos.Y);
		else
			main.glPrint(0,0,0,"Position: %dx%d",Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE);
		break;
	case TILE:
		main.glPrint(0,0,0,"Tile: %dx%d",Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE);
		main.glPrint(0,16,0,"Tile Value: %d",LevelMap.GetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE));
		main.glPrint(0,32,0,"Tile ColDet Value: %d",LevelMap.GetTileColDet(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE));
		break;
	case PLAYERSTART:
		if(PlacedChar)
			main.glPrint(0,0,0,"Player Start Position: %dx%d",PlayerStart.X,PlayerStart.Y);
		break;
	}
	
	main.glPrint(0,540,0,"EFFF Level Editor by Jeff Verkoeyen %s",CURRVERSION);
	glColor3d(1,1,1);
}

void LevelEditor::SetColDet()
{
	main.Texture2D(FALSE);
	glLoadIdentity();
	glTranslated(0,0,0);
	glColor3d(1,0,0);
	glLoadIdentity();
	glTranslated(16,16,4);
	for(int a=0;a<560;a+=32)
	{
		for(int b=0;b<800;b+=32)
		{
			if(LevelMap.GetTileColDet(b/32,a/32)==0)
				main.Mode(GL_FRONT,GL_LINE);
			else
				main.Mode(GL_FRONT,GL_FILL);
			glBegin(GL_TRIANGLES);
				glVertex3d(0,8,0);
				glVertex3d(8,-8,0);
				glVertex3d(-8,-8,0);
			glEnd();
			glTranslated(32,0,0);
		}
		glTranslated(-800,32,0);
	}
	if(Data.mouse->Left && !Data.mouse->hLeft)
	{
		Data.mouse->hLeft=true;
		int temp=LevelMap.GetTileColDet(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE);
		temp++;
		if(temp>1)
			temp=0;
		LevelMap.SetTileColDet(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE,temp);
	}
	if(Data.mouse->hRight)
		LevelMap.SetTileColDet(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE,LastVal);
	if(Data.mouse->Right && !Data.mouse->hRight)
	{
		Data.mouse->hRight=true;
		int temp=LevelMap.GetTileColDet(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE);
		temp++;
		if(temp>1)
			temp=0;
		LastVal=temp;
		LevelMap.SetTileColDet(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE,temp);
	}

	main.Mode(GL_FRONT,GL_FILL);
	glColor3d(1,1,1);
	main.Texture2D(TRUE);
}

bool LevelEditor::HandleMenu()
{
	if(Data.mouse->Left && !Data.mouse->hLeft)
	{
		Data.mouse->hLeft=true;

		if(Data.mouse->Pos.X<50 && Data.mouse->Pos.Y<16)
		{
			ShowCursor(TRUE);
			if(MessageBox(main.hWnd,"Are you sure that you want to erase this level and start a new one?","New",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				Data.PlaceMode=0;
				Editing=Info=Menu=PlacedChar=FillGrid=ShowGrid=ColDet=CreatingObj=DropPos=SelectTile=Selected=false;
				Select[0].X=Select[0].Y=Select[1].X=Select[1].Y=-1;
				strcpy(TileName,"tiles.bmp");
				strcpy(ObjeName,"objects.bmp");
				strcpy(PlayName,"sprites.bmp");
				InitEditor();
			}
			ShowCursor(FALSE);
		}
		if(Data.mouse->Pos.X<50 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
			LoadLevel();
		if(Data.mouse->Pos.X<50 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
	 		SaveLevel();
		if(Data.mouse->Pos.X<50 && Data.mouse->Pos.Y>48 && Data.mouse->Pos.Y<64)
		{
			ShowCursor(TRUE);
			if(MessageBox(main.hWnd,"Are you sure you want to exit?","Exit?",MB_YESNO|MB_ICONQUESTION)==IDYES)
				done=true;
			ShowCursor(FALSE);
		}
		if(Data.mouse->Pos.X>=50 && Data.mouse->Pos.X<156 && Data.mouse->Pos.Y<16)
		{
			ShowGrid=!ShowGrid;
			Menu=false;
		}
		if(Data.mouse->Pos.X>=50 && Data.mouse->Pos.X<156 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
		{
			FillGrid=!FillGrid;
			Menu=false;
		}
		if(Data.mouse->Pos.X>=50 && Data.mouse->Pos.X<156 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		{
			SelectTile=!SelectTile;
			Menu=false;
			Data.mouse->Left=false;
		}
		if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
		{	// Sprites
			Menu=false;
			bool ok=false;
			while(!ok)
			{
				char FileName[MAX_PATH]=" ";
				char Filt1[]="Bmp Files (*.bmp)\0*.bmp\0";
				char Filt2[]="bmp";
				LoadFile(FileName,Filt1,Filt2);

				if(FileName[0]!=' ')
				{
					int stop=0;
					for(int a=strlen(FileName)-1;a>=0;a--)
					{
						if(FileName[a]=='\\')
							break;
					}
					stop=a+1;
					for(a=stop;a<(int)strlen(FileName);a++)
						FileName[a-stop]=FileName[a];
					FileName[a-stop]='\0';
					strcpy(PlayName,FileName);
					main.LoadTexture(PlayName,0);
					
					MainChar.Init(0,PlayerStart);

					iCOORD temp;
					
					temp=MainChar.GetbmpSize();
					
					if((temp.X & -temp.X)==0 || (temp.Y & -temp.Y)==0)
					{
						main.Error("Error Loading Bmp",0,"Bmp does not have valid dimensions.  Please make the dimensions are a power of two.\r\nEx:  128x64");
						ok=false;
					}
					else if(temp.X<32 || temp.Y<32)
					{
						main.Error("Error Loading Bmp",0,"Bmp must have at least ONE 32x32 tile inside of it.");
						ok=false;
					}
					else
						ok=true;

					MainChar.SetFace(DOWN);
					MainChar.SetFrameSpeed(3);
				}
				else
					ok=true;
			}
		}
		if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		{	// Objects
			Menu=false;
			bool ok=false;
			while(!ok)
			{
				char FileName[MAX_PATH]=" ";
				char Filt1[]="Bmp Files (*.bmp)\0*.bmp\0";
				char Filt2[]="bmp";
				LoadFile(FileName,Filt1,Filt2);

				if(FileName[0]!=' ')
				{
					int stop=0;
					for(int a=strlen(FileName)-1;a>=0;a--)
					{
						if(FileName[a]=='\\')
							break;
					}
					stop=a+1;
					for(a=stop;a<(int)strlen(FileName);a++)
						FileName[a-stop]=FileName[a];
					FileName[a-stop]='\0';
					strcpy(ObjeName,FileName);
					main.LoadTexture(ObjeName,2);
					ok=true;
				}
				else
					ok=true;
			}
		}
		if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>48 && Data.mouse->Pos.Y<64)
		{	// Tiles
			Menu=false;
			bool ok=false;
			while(!ok)
			{
				char FileName[MAX_PATH]=" ";
				char Filt1[]="Bmp Files (*.bmp)\0*.bmp\0";
				char Filt2[]="bmp";
				LoadFile(FileName,Filt1,Filt2);

				if(FileName[0]!=' ')
				{
					int stop=0;
					for(int a=strlen(FileName)-1;a>=0;a--)
					{
						if(FileName[a]=='\\')
							break;
					}
					stop=a+1;
					for(a=stop;a<(int)strlen(FileName);a++)
						FileName[a-stop]=FileName[a];
					FileName[a-stop]='\0';
					strcpy(TileName,FileName);
					main.LoadTexture(TileName,1);
					
					LevelMap.Init(1);

					iCOORD temp;
					
					temp=LevelMap.GetbmpSize();
					
					if((temp.X & -temp.X)==0 || (temp.Y & -temp.Y)==0)
					{
						main.Error("Error Loading Bmp",0,"Bmp does not have valid dimensions.  Please make the dimensions are a power of two.\r\nEx:  128x64");
						ok=false;
					}
					else if(temp.X<32 || temp.Y<32)
					{
						main.Error("Error Loading Bmp",0,"Bmp must have at least ONE 32x32 tile inside of it.");
						ok=false;
					}
					else
						ok=true;
					
					FillTile=0;
					for(a=0;a<DEFAULTILEY;a++)
					{
						for(int b=0;b<DEFAULTILEX;b++)
						{
							if(LevelMap.GetTileData(b,a)>=LevelMap.Data.BmpSize.X/DEFAULTILESIZE*LevelMap.Data.BmpSize.Y/DEFAULTILESIZE)
								LevelMap.SetTileData(b,a,LevelMap.Data.BmpSize.X/DEFAULTILESIZE*LevelMap.Data.BmpSize.Y/DEFAULTILESIZE-1);
						}
					}
				}
				else
					ok=true;
			}
		}
		if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>64 && Data.mouse->Pos.Y<80)
		{	// Transition
			Menu=false;
			bool ok=false;
			while(!ok)
			{
				char FileName[MAX_PATH]=" ";
				char Filt1[]="Bmp Files (*.bmp)\0*.bmp\0";
				char Filt2[]="bmp";
				LoadFile(FileName,Filt1,Filt2);

				if(FileName[0]!=' ')
				{
					int stop=0;
					for(int a=strlen(FileName)-1;a>=0;a--)
					{
						if(FileName[a]=='\\')
							break;
					}
					stop=a+1;
					for(a=stop;a<(int)strlen(FileName);a++)
						FileName[a-stop]=FileName[a];
					FileName[a-stop]='\0';
					strcpy(TransName,FileName);
					ok=true;
				}
				else
					ok=true;
			}
		}
		if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>96 && Data.mouse->Pos.Y<112)
		{
			Menu=false;
			strcpy(TileName,"tiles.bmp");
			strcpy(ObjeName,"objects.bmp");
			strcpy(PlayName,"sprites.bmp");
			strcpy(TransName,"");
		}
		if(Data.mouse->Pos.X>=275 && Data.mouse->Pos.X<390 && Data.mouse->Pos.Y<16)
		{
			Menu=false;

			char FileName[MAX_PATH]=" ";
			char Filt1[]="EFFF Levels (*.eff)\0*.eff\0";
			char Filt2[]="eff";
			LoadFile(FileName,Filt1,Filt2);
			
			if(FileName[0]!=' ')
			{
				int stop=0;
				for(int a=strlen(FileName)-1;a>=0;a--)
				{
					if(FileName[a]=='\\')
						break;
				}
				stop=a+1;
				for(a=stop;a<(int)strlen(FileName);a++)
					FileName[a-stop]=FileName[a];
				FileName[a-stop]='\0';
				strcpy(NextLevel,FileName);
			}
		}
		if(Data.mouse->Pos.X>=275 && Data.mouse->Pos.X<390 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		{
			strcpy(NextLevel,"");
			Menu=false;
		}

		if(Data.mouse->Pos.X>=390 && Data.mouse->Pos.X<520 && Data.mouse->Pos.Y<16)
		{
			Menu=false;
			ColDet=!ColDet;
		}
		if(Data.mouse->Pos.X>=390 && Data.mouse->Pos.X<520 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
		{
			Menu=false;
			for(int a=0;a<ObsMan.GetNumObs();a++)
			{
				cObstacle* temp;
				temp=ObsMan.GetObstacle(a);
				temp->Disp.Reset();
			}
		}
		if(Data.mouse->Pos.X>=390 && Data.mouse->Pos.X<520 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		{
			Menu=false;
			Info=!Info;
		}
	}
	return true;
}

void LevelEditor::DrawMenu()
{
	glLoadIdentity();
	glTranslated(0,0,8);

	main.Texture2D(FALSE);

	glBegin(GL_QUADS);
	if(Data.mouse->Pos.X<50 && Data.mouse->Pos.Y<16)
		glColor3d(0,1,1);	// New
	else
		glColor3d(0,0,1);
		glVertex2d( 0, 16);
		glVertex2d( 50,16);
		glVertex2d( 50,0);
		glVertex2d( 0, 0);

	if(Data.mouse->Pos.X<50 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
		glColor3d(0,1,1);	// Load
	else
		glColor3d(0,0,1);
		glVertex2d( 0, 32);
		glVertex2d( 50,32);
		glVertex2d( 50,16);
		glVertex2d( 0, 16);

	if(Data.mouse->Pos.X<50 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		glColor3d(0,1,1);	// Save
	else
		glColor3d(0,0,1);
		glVertex2d( 0, 48);
		glVertex2d( 50,48);
		glVertex2d( 50,32);
		glVertex2d( 0, 32);

	if(Data.mouse->Pos.X<50 && Data.mouse->Pos.Y>48 && Data.mouse->Pos.Y<64)
		glColor3d(0,1,1);	// Quit
	else
		glColor3d(0,0,1);
		glVertex2d( 0, 64);
		glVertex2d( 50,64);
		glVertex2d( 50,48);
		glVertex2d( 0, 48);

	if(Data.mouse->Pos.X>=50 && Data.mouse->Pos.X<156 && Data.mouse->Pos.Y<16)
		glColor3d(0,1,1);	// Show Grid
	else
		glColor3d(0,0,1);
		glVertex2d( 50,	16);
		glVertex2d( 156,16);
		glVertex2d( 156,0);
		glVertex2d( 50,	0);

	if(Data.mouse->Pos.X>=50 && Data.mouse->Pos.X<156 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
		glColor3d(0,1,1);	// Fill Grid
	else
		glColor3d(0,0,1);
		glVertex2d( 50,	32);
		glVertex2d( 156,32);
		glVertex2d( 156,16);
		glVertex2d( 50,	16);

	if(Data.mouse->Pos.X>=50 && Data.mouse->Pos.X<156 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		glColor3d(0,1,1);	// Fill Area
	else
		glColor3d(0,0,1);
		glVertex2d( 50,	48);
		glVertex2d( 156,48);
		glVertex2d( 156,32);
		glVertex2d( 50,	32);

		glColor3d(0,0,1);	// Change BMP
		glVertex2d( 156,16);
		glVertex2d( 275,16);
		glVertex2d( 275,0);
		glVertex2d( 156,0);

	if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
		glColor3d(0,1,1);	// Sprites
	else
		glColor3d(0,0,1);
		glVertex2d( 156,32);
		glVertex2d( 275,32);
		glVertex2d( 275,16);
		glVertex2d( 156,16);

	if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		glColor3d(0,1,1);	// Objects
	else
		glColor3d(0,0,1);
		glVertex2d( 156,48);
		glVertex2d( 275,48);
		glVertex2d( 275,32);
		glVertex2d( 156,32);
		
	if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>48 && Data.mouse->Pos.Y<64)
		glColor3d(0,1,1);	// Tiles
	else
		glColor3d(0,0,1);
		glVertex2d( 156,64);
		glVertex2d( 275,64);
		glVertex2d( 275,48);
		glVertex2d( 156,48);
		
	if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>64 && Data.mouse->Pos.Y<80)
		glColor3d(0,1,1);	// Transition
	else
		glColor3d(0,0,1);
		glVertex2d( 156,80);
		glVertex2d( 275,80);
		glVertex2d( 275,64);
		glVertex2d( 156,64);

	bool ShowDetails=false;

	if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>80 && Data.mouse->Pos.Y<96)
	{
		glColor3d(0,1,1);	// Info
		ShowDetails=true;
	}
	else
		glColor3d(0,0,1);

		glVertex2d( 156,96);	// Details
		glVertex2d( 275,96);
		glVertex2d( 275,80);
		glVertex2d( 156,80);

	if(Data.mouse->Pos.X>=156 && Data.mouse->Pos.X<275 && Data.mouse->Pos.Y>96 && Data.mouse->Pos.Y<112)
		glColor3d(0,1,1);	// Reset
	else
		glColor3d(0,0,1);
		glVertex2d( 156,112);
		glVertex2d( 275,112);
		glVertex2d( 275,96);
		glVertex2d( 156,96);

	if(ShowDetails)
	{
		glColor3d(0,0,1);
		glVertex2d( 0,264);
		glVertex2d( 400,264);
		glVertex2d( 400,200);
		glVertex2d( 0,200);
	}

	if(Data.mouse->Pos.X>=275 && Data.mouse->Pos.X<390 && Data.mouse->Pos.Y<16)
		glColor3d(0,1,1);	// Next level
	else
		glColor3d(0,0,1);
		glVertex2d( 275,16);
		glVertex2d( 390,16);
		glVertex2d( 390,0);
		glVertex2d( 275,0);

	bool ShowNext=false;

	if(Data.mouse->Pos.X>=275 && Data.mouse->Pos.X<390 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
	{						// Info
		ShowNext=true;
		glColor3d(0,1,1);
	}
	else
		glColor3d(0,0,1);
		glVertex2d( 275,32);
		glVertex2d( 390,32);
		glVertex2d( 390,16);
		glVertex2d( 275,16);
	
	if(Data.mouse->Pos.X>=275 && Data.mouse->Pos.X<390 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		glColor3d(0,1,1);	// Reset
	else
		glColor3d(0,0,1);
		glVertex2d( 275,48);
		glVertex2d( 390,48);
		glVertex2d( 390,32);
		glVertex2d( 275,32);

	if(Data.mouse->Pos.X>=390 && Data.mouse->Pos.X<520 && Data.mouse->Pos.Y<16)
		glColor3d(0,1,1);	// Collisions
	else
		glColor3d(0,0,1);
		glVertex2d( 390,16);
		glVertex2d( 520,16);
		glVertex2d( 520,0);
		glVertex2d( 390,0);

	if(Data.mouse->Pos.X>=390 && Data.mouse->Pos.X<520 && Data.mouse->Pos.Y>16 && Data.mouse->Pos.Y<32)
		glColor3d(0,1,1);	// Synchronize
	else
		glColor3d(0,0,1);
		glVertex2d( 390,32);
		glVertex2d( 520,32);
		glVertex2d( 520,16);
		glVertex2d( 390,16);

	if(Data.mouse->Pos.X>=390 && Data.mouse->Pos.X<520 && Data.mouse->Pos.Y>32 && Data.mouse->Pos.Y<48)
		glColor3d(0,1,1);	// Debug Info
	else
		glColor3d(0,0,1);
		glVertex2d( 390,48);
		glVertex2d( 520,48);
		glVertex2d( 520,32);
		glVertex2d( 390,32);

	if(ShowNext)
	{
		glColor3d(0,0,1);
		glVertex2d( 0,216);
		glVertex2d( 400,216);
		glVertex2d( 400,200);
		glVertex2d( 0,200);
	}

	glEnd();
	main.Texture2D(TRUE);

	glColor3d(1,1,1);
	if(ShowDetails)
	{
		main.glPrint(0,200,0,"Sprites: %s",PlayName);
		main.glPrint(0,216,0,"Objects: %s",ObjeName);
		main.glPrint(0,232,0,"Tiles: %s",TileName);
		main.glPrint(0,248,0,"Transition: %s",TransName);
	}
	if(ShowNext)
		main.glPrint(0,200,0,"Next Level: %s",NextLevel);

	main.glPrint(2,0,0,"New");
	main.glPrint(2,16,0,"Load");
	main.glPrint(2,32,0,"Save");
	main.glPrint(2,48,0,"Exit");
	if(ShowGrid)
		main.glPrint(52,0,0,"Hide Grid");
	else
		main.glPrint(52,0,0,"Show Grid");
	main.glPrint(52,16,0,"Fill Grid");
	main.glPrint(52,32,0,"Fill Area");

	main.glPrint(158,16,0,"Sprites");
	main.glPrint(158,32,0,"Objects");
	main.glPrint(158,48,0,"Tiles");
	main.glPrint(158,64,0,"Transition");
	main.glPrint(158,80,0,"Info");
	main.glPrint(158,96,0,"Reset");

	main.glPrint(277,0,0,"Next Level");
	main.glPrint(277,16,0,"Info");
	main.glPrint(277,32,0,"Reset");

	main.glPrint(392,0,0,"Collisions");
	main.glPrint(392,16,0,"Synchronize");
	main.glPrint(392,32,0,"Debug Info");

	glColor3d(1,0,0);
	main.glPrint(157,0,0,"Change BMP");

	glColor3d(1,1,1);
}

void LevelEditor::LoadFile(char* FileName, char* Filter, char* Ex)
{
	ShowCursor(TRUE);
	OPENFILENAME ofn;
	
	ZeroMemory(&ofn, sizeof(ofn));
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = main.hWnd;
	ofn.lpstrFilter = Filter;
	ofn.lpstrFile = FileName;

	char temp[512];
	
	GetCurrentDirectory(512, temp);	// We need to get the currdir so that we can set the default
									// directory
	ofn.lpstrInitialDir=temp;

	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = Ex;

	GetOpenFileName(&ofn);
	ShowCursor(FALSE);
}

void LevelEditor::SaveFile(char* FileName, char* Filter, char* Ex)
{
	if(!PlacedChar)
		main.Error("Error Saving File",MB_OK|MB_ICONINFORMATION,"Could not save file.\r\nReason:\r\nCould not locate player start position.\r\nAdvice: Give the player a start position");
	else
	{
		ShowCursor(TRUE);
		OPENFILENAME ofn;
		
		ZeroMemory(&ofn, sizeof(ofn));
		
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = main.hWnd;
		ofn.lpstrFilter = Filter;
		ofn.lpstrFile = FileName;
		
		char temp[512];
		
		GetCurrentDirectory(512, temp);	// We need to get the currdir so that we can set the default
										// directory
		ofn.lpstrInitialDir=temp;

		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = Ex;
		
		GetSaveFileName(&ofn);
		ShowCursor(FALSE);
	}
}

void LevelEditor::DrawGrid()
{
	main.Texture2D(FALSE);
	glLoadIdentity();
	glTranslated(0,0,3);
	glColor3d(1,0,1);
	glBegin(GL_LINES);
	for(int a=0;a<600;a+=32)
	{
		glVertex2d(0,a);
		glVertex2d(800,a);
	}
	for(a=0;a<=800;a+=32)
	{
		glVertex2d(a,0);
		glVertex2d(a,576);
	}
	glEnd();
	glColor3d(1,1,1);
	main.Texture2D(TRUE);
}

void LevelEditor::Fill()
{
	glLoadIdentity();
	glTranslated(400,300,6);

	main.Texture2D(FALSE);

	glBegin(GL_QUADS);
		glColor3d(0,0,1);
		glVertex2d(-100,50);
		glVertex2d( 100,50);
		glVertex2d( 100,-50);
		glVertex2d(-100,-50);
	glEnd();

	main.Texture2D(TRUE);

	glColor3d(1,0,0);
	main.glPrint(335,250,0,"Fill Grid");
	main.glPrint(330,320,0,"%d",FillTile);

	if(Data.mouse->Pos.X>370 && Data.mouse->Pos.X<428 && Data.mouse->Pos.Y>310 && Data.mouse->Pos.Y<326)
	{
		glColor3d(1,1,1);
		if(Data.mouse->Left && !Data.mouse->hLeft)
		{
			Data.mouse->hLeft=true;
			for(int a=0;a<DEFAULTILEY;a++)
			{
				for(int b=0;b<DEFAULTILEX;b++)
					LevelMap.SetTileData(b,a,FillTile);
			}
			FillGrid=false;
		}
	}
	else
		glColor3d(0,1,0);

	main.glPrint(370,310,0,"Fill");

	if(Data.mouse->Pos.X>360 && Data.mouse->Pos.X<445 && Data.mouse->Pos.Y>326 && Data.mouse->Pos.Y<342)
	{
		glColor3d(1,1,1);
		if(Data.mouse->Left && !Data.mouse->hLeft)
		{
			Data.mouse->hLeft=true;
			FillGrid=false;
		}
	}
	else
		glColor3d(0,1,0);

	main.glPrint(360,326,0,"Cancel");

	glColor3d(1,1,1);

	LevelMap.DrawTile(340,300,9,FillTile);

	main.SetTexture(3);

	glLoadIdentity();
	glTranslated(460,280,8);

	if(Data.mouse->Pos.X>444 && Data.mouse->Pos.X<476 && Data.mouse->Pos.Y>264 && Data.mouse->Pos.Y<296)
	{
		glColor3d(0,1,1);
		if(Data.mouse->Left && !Data.mouse->hLeft)
		{
			Data.mouse->hLeft=true;
			FillTile++;
			if(FillTile>LevelMap.GetMaxTiles())
				FillTile=0;
		}
	}
	else
		glColor3d(1,0,0);

	glBegin(GL_QUADS);
		glTexCoord2f(0,1);	glVertex2d( 16,-16);
		glTexCoord2f(1,1);	glVertex2d(-16,-16);
		glTexCoord2f(1,0);	glVertex2d(-16, 16);
		glTexCoord2f(0,0);	glVertex2d( 16, 16);
	glEnd();

	glLoadIdentity();
	glTranslated(460,320,8);

	if(Data.mouse->Pos.X>444 && Data.mouse->Pos.X<476 && Data.mouse->Pos.Y>304 && Data.mouse->Pos.Y<336)
	{
		glColor3d(0,1,1);
		
		if(Data.mouse->Left && !Data.mouse->hLeft)
		{
			Data.mouse->hLeft=true;
			FillTile--;
			if(FillTile<0)
				FillTile=LevelMap.GetMaxTiles();
		}
	}
	else
		glColor3d(1,0,0);

	glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex2d( 16,-16);
		glTexCoord2f(1,0);	glVertex2d(-16,-16);
		glTexCoord2f(1,1);	glVertex2d(-16, 16);
		glTexCoord2f(0,1);	glVertex2d( 16, 16);
	glEnd();

	if(main.keys[VK_ESCAPE])
		FillGrid=false;

	glColor3d(1,1,1);
}

void LevelEditor::DrawMouse()
{
	if(Data.mouse->Pos.Y>575)
		Data.mouse->Pos.Y=575;
	glLoadIdentity();
	glTranslated(Data.mouse->Pos.X,Data.mouse->Pos.Y,10);

	glBindTexture(GL_TEXTURE_2D, mouseBMP);

	glColor3d(1,1,1);

	float u=0,v=0,eu=.5,ev=.5;

	if(!Editing && !Menu && !FillGrid && !ColDet && !CreatingObj && !SelectTile)
	{
		switch(Data.PlaceMode)
		{
		case TILE:
			u=.5;
			eu=1;
			break;
		case PLAYERSTART:
			v=.5;
			ev=1;
			break;
		};
	}
	else
	{
		u=v=.5;
		eu=ev=1;
	}

	glBegin(GL_QUADS);
		glTexCoord2f( u,1-ev);	glVertex2f( 0,	32);
		glTexCoord2f(eu,1-ev);	glVertex2f( 32,	32);
		glTexCoord2f(eu,1-v);	glVertex2f( 32,	0);
		glTexCoord2f( u,1-v);		glVertex2f( 0,	0);
	glEnd();
}

void LevelEditor::HandleMouse()
{
	if(Data.mouse->Right && !Data.mouse->hRight)
	{
		Data.PlaceMode++;
		if(Data.PlaceMode>2)
			Data.PlaceMode=0;
		Data.mouse->hRight=true;
	}
}

void LevelEditor::HandleClicks()
{
	if(Data.mouse->Left && !Data.mouse->hLeft)
	{
		switch(Data.PlaceMode)
		{
		case OBJECT:
		{
			Data.mouse->hLeft=true;
			CreatingObj=true;
			selectedObj=-1;
			for(int a=0;a<ObsMan.GetNumObs();a++)
			{
				cObstacle* temp;
				temp=ObsMan.GetObstacle(a);
				PIXEL tempPos=temp->GetObjPos();

				if(Data.mouse->Pos.X>=tempPos.X-16 && Data.mouse->Pos.X<=tempPos.X+16 &&
				   Data.mouse->Pos.Y>=tempPos.Y-16 && Data.mouse->Pos.Y<=tempPos.Y+16)
				{
					selectedObj=a;
					obs=ObsMan.GetObstacle(a);

					ObstacleTemp.Dir=obs->iDirection;
					ObstacleTemp.State=obs->bState;
					ObstacleTemp.Type=obs->iType;
					ObstacleTemp.Face=obs->Disp.GetFace();
					tempOn=true;
					break;
				}
			}
			if(selectedObj==-1)
			{
				if(ObsMan.GetNumObs()>=MAX_OBSTACLES)
				{
					main.Error("Error Creating Obstacle",MB_OK|MB_ICONEXCLAMATION,"Could not create obstacle:\r\nReason:\r\nToo many obstacles\r\nMax # Obstacles: %d",MAX_OBSTACLES);
					return;
				}
				obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				selectedObj=ObsMan.GetNumObs()-1;
				if(!main.keys[VK_CONTROL])
				{
					obs->SetProps(Data.mouse->Pos, DIR_NS, true, OBS_CONVEYOR);
					obs->Init(2, Data.mouse->Pos, UP);

					ObstacleTemp.Dir=DIR_NS;
					ObstacleTemp.State=true;
					ObstacleTemp.Type=OBS_CONVEYOR;
					ObstacleTemp.Face=UP;
				}
				else
				{
					obs->SetProps(Data.mouse->Pos, ObstacleTemp.Dir, ObstacleTemp.State, ObstacleTemp.Type);
					obs->Init(2, Data.mouse->Pos, ObstacleTemp.Face);
				}
				tempOn=true;
			}
		}
			break;
		case TILE:
			if(!main.keys[VK_SHIFT] && !main.keys[VK_CONTROL])
				LevelMap.SetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE,LevelMap.GetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE)+1);
			else if(main.keys[VK_SHIFT] && !main.keys[VK_CONTROL])
				LevelMap.SetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE,LevelMap.GetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE)-1);
			else
				LevelMap.SetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE,FillTile);
			if(LevelMap.GetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE)>LevelMap.GetMaxTiles())
				LevelMap.SetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE,0);
			if(LevelMap.GetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE)<0)
				LevelMap.SetTileData(Data.mouse->Pos.X/DEFAULTILESIZE,Data.mouse->Pos.Y/DEFAULTILESIZE,LevelMap.GetMaxTiles());
			Data.mouse->hLeft=true;
			break;
		case PLAYERSTART:
			PlayerStart.X=Data.mouse->Pos.X;
			PlayerStart.Y=Data.mouse->Pos.Y;
			PlacedChar=true;
			MainChar.Init(0,PlayerStart);
			MainChar.SetFace(DOWN);
			MainChar.SetFrameSpeed(4);
			break;
		};
	}
}

bool LevelEditor::LoadMouseBMP()
{
	bool Status=FALSE;
	
	AUX_RGBImageRec *TextureImage[1];

	memset(TextureImage,0,sizeof(void *)*1);

	if(TextureImage[0]=main.LoadBMP("mouse.bmp"))
	{
		Status=TRUE;
		glGenTextures(1, &mouseBMP);

		glBindTexture(GL_TEXTURE_2D, mouseBMP);

		int texture_size = TextureImage[0]->sizeX*TextureImage[0]->sizeY;
		GLubyte* RGBA_pixels = new GLubyte[TextureImage[0]->sizeX*TextureImage[0]->sizeY*4];
		for(int i=0;i<texture_size;i++)
		{
			RGBA_pixels[i*4] = TextureImage[0]->data[i*3];
			RGBA_pixels[i*4+1] = TextureImage[0]->data[i*3+1];
			RGBA_pixels[i*4+2] = TextureImage[0]->data[i*3+2];
			
			if(TextureImage[0]->data[i*3]==255 && TextureImage[0]->data[i*3+1]==0 && TextureImage[0]->data[i*3+2]==255)
				RGBA_pixels[i*4+3]=0;
			else
				RGBA_pixels[i*4+3]=255;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, RGBA_pixels);
		
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		
		if(TextureImage[0]->data)
			free(TextureImage[0]->data);

		if(RGBA_pixels)
			delete [] RGBA_pixels;
		
		free(TextureImage[0]);						// Free The Image Structure
	}
	else
		main.Error("ERROR",MB_OK|MB_ICONEXCLAMATION,"Can't find mouse.bmp");

	return Status;								// Return The Status
}