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
#include "FPS.h"
extern FPS fps;

#include "common.h"
#include "CFmod.h"
extern CFmod SFX[NUMSFX];
#include "obstacle.h"

bool OkToToggle=false;

///////////////////////////////////////////////////////////////////////////////////////

void cObstacle::Init(unsigned int TEXID, iCOORD Pos, unsigned int Face)
{
	iCOORD StartPos;
	StartPos.X=0;
	StartPos.Y=0;
	bool Bounce=false;
	unsigned int numAnims=1;
	unsigned int FrameSpeed = 25;

	TEX_ID = TEXID;

	SFX=-1;

	switch (iType)
	{
		case OBS_CONVEYOR:
			StartPos.X = 0;
			StartPos.Y = 32;
			Bounce = false;
			numAnims = 4;
			SFX=CONVEYOR;
			break;
		case OBS_TURN:
			StartPos.X = 0;
			StartPos.Y = 0;
			Bounce = false;
			numAnims = 4;
			break;
		case OBS_PIT:
			StartPos.X = 0;
			StartPos.Y = 64;
			Bounce = false;
			numAnims = 1;
			break;
		case OBS_BUTTON:
			if(!bState)
				StartPos.X = 32;
			else
				StartPos.X = 64;
			StartPos.Y = 64;
			Bounce = false;
			numAnims = 1;
			SFX=BUTTON;
			break;
		case OBS_GOAL:
			StartPos.X = 0;
			StartPos.Y = 96;
			Bounce = true;
			numAnims = 5;
			break;
		case OBS_HAMMER:
			StartPos.X = 160;
			StartPos.Y = 0;
			Bounce = false;
			numAnims = 6;
			break;
		case OBS_MULTI:
			StartPos.X = -1;
			StartPos.Y = -1;
			Bounce = false;
			numAnims = 1;
			break;
		case OBS_TIMER:
			StartPos.X = -2;
			StartPos.Y = -2;
			Bounce = false;
			numAnims = 1;
			if(Face)
				Offset=1;
			else
				Offset=0;		// 0 off, 1 is setting up, 2 is counting
			OffsetValue=0;
			break;
		case OBS_CRANE:
			StartPos.X = 64;
			StartPos.Y = 160;
			Bounce = false;
			numAnims = 1;
			break;
		case OBS_LASER:
			SFX=LASER;
			break;
		default:
			StartPos.X = 96;
			StartPos.Y = 64;
			Bounce = false;
			numAnims = 1;
			break;
	}
	ulTimer=0;
	bToggled=false;

	Disp.Init(TEXID, StartPos, Pos, Bounce, numAnims, Face, FrameSpeed);
}

///////////////////////////////////////////////////////////////////////////////////////

void cObstacle::ToggleState()
{
	int iDir = -1;
	
	bState = bState? false : true;

	if(iType==OBS_CONVEYOR || iType==OBS_TURN)
	{
		switch (iDirection)
		{
			case DIR_NS:
				if(bState) iDir = UP;
				else iDir = DOWN;
				break;
			case DIR_EW:
				if(bState) iDir = RIGHT;
				else iDir = LEFT;
				break;
			case DIR_CCW:
				if(bState) iDir = UP;
				else iDir = DOWN;
				break;
			case DIR_CC:
				if(bState) iDir = DOWN;
				else iDir = UP;
				break;
			default:
				iDir = UP;
		}
	}
	else if(iType==OBS_BUTTON)
	{
		iCOORD StartPos;
		StartPos.X=StartPos.Y=64;
		iDir=UP;
		Disp.Init(TEX_ID, StartPos, Disp.GetPos(), bState, 1, UP, 4);
	}
	else if(iType==OBS_HAMMER)
		Disp.SetFrame(0);

	if(iDir!=-1)
		this->Init(TEX_ID, pPosition, iDir);
	if(OkToToggle)
	{
		bToggled=true;
		ulToggledTimer=GetTickCount()+5000;
	}
}

///////////////////////////////////////////////////////////////////////////////////////

void cObstacle::Draw()			//draws this obstacle on the screen
{
	switch(iType)
	{
	case OBS_BUTTON:
		Disp.DrawObject();
		break;
	case OBS_HAMMER:
		if(bState)
			Disp.HandleObject();
		else
			Disp.DrawObject();
		break;
	default:
		if(iType!=OBS_LASER)
			Disp.HandleObject();
		break;
	}
	return;
}

void cObstacle::DrawLaser()
{
	if(!bState) return;

	glLoadIdentity();
	glTranslated(0,0,3);

	main.Texture2D(FALSE);   // So we don't get a funky line

	glLineWidth(2);
	main.Culling(FALSE);
	glBegin(GL_LINES);      // Start drawing line
		glVertex2d(pPosition.X,pPosition.Y);   // Vertex 1
		switch(Disp.GetFace())
		{
			case DIR_N:
				glVertex2d(pPosition.X,pPosition.Y-(iDirection & 0x000000FF)*DEFAULTILESIZE);   // Vertex 2
				break;
			case DIR_S:
				glVertex2d(pPosition.X,pPosition.Y+(iDirection & 0x000000FF)*DEFAULTILESIZE);   // Vertex 2
				break;
			case DIR_W:
				glVertex2d(pPosition.X-(iDirection & 0x000000FF)*DEFAULTILESIZE,pPosition.Y);   // Vertex 2
				break;
			case DIR_E:
				glVertex2d(pPosition.X+(iDirection & 0x000000FF)*DEFAULTILESIZE,pPosition.Y);   // Vertex 2
				break;
			default:
				glVertex2d(pPosition.X,pPosition.Y+32);   // Vertex 2
				break;
		}			
	glEnd();         // Stop drawing lines
	glLineWidth(1);
	main.Culling(TRUE);
	main.Texture2D(TRUE);   // And so we have textures again 
}

void cObstacle::DrawLaserToPos(int x, int y, int z)
{
	glLoadIdentity();
	glTranslated(0,0,z);

	main.Texture2D(FALSE);   // So we don't get a funky line
	glColor3d(1,0,0);

	glLineWidth(2);
	main.Culling(FALSE);

	glBegin(GL_LINES);      // Start drawing line
		glVertex2d(x,y);   // Vertex 1
		switch(Disp.GetFace())
		{
			case DIR_N:
				glVertex2d(x,y-DEFAULTILESIZE);   // Vertex 2
				break;
			case DIR_S:
				glVertex2d(x,y+DEFAULTILESIZE);   // Vertex 2
				break;
			case DIR_W:
				glVertex2d(x-DEFAULTILESIZE,y);   // Vertex 2
				break;
			case DIR_E:
				glVertex2d(x+DEFAULTILESIZE,y);   // Vertex 2
				break;
			default:
				glVertex2d(x,y+DEFAULTILESIZE);   // Vertex 2
				break;
		}			
	glEnd();         // Stop drawing lines
	glLineWidth(1);
	main.Culling(TRUE);
	glColor3d(1,1,1);
	main.Texture2D(TRUE);   // And so we have textures again 
}
///////////////////////////////////////////////////////////////////////////////////////

long cObstacle::GetPlayerEffect(Character* Player)
{
	if(PlayerAffected(Player) || iType == OBS_MULTI || iType == OBS_TIMER)
	{
		switch (iType)
		{
			case OBS_CONVEYOR:
				switch (iDirection)
				{
					case DIR_NS:
						if(bState) return OBS_MOVE_N;
						else return OBS_MOVE_S;
					case DIR_EW:
						if(bState) return OBS_MOVE_E;
						else return OBS_MOVE_W;
					default:
						return OBS_NONE;
				}
				break;
			case OBS_TURN:
				switch (iDirection)
				{
					case DIR_CC:
						if(bState) return OBS_TURN_CC;
						else return OBS_TURN_CCW;
					case DIR_CCW:
						if(bState) return OBS_TURN_CCW;
						else return OBS_TURN_CC;
					default:
						return OBS_NONE;
				}
				// byte 1: length
			case OBS_LASER:
				if(bState) return OBS_KILL;
				else return OBS_NONE;
			case OBS_PIT:
				if(bState) return OBS_KILL;
				else return OBS_NONE;
			case OBS_WALL:
				if(bState) return OBS_BLOCK;
				else return OBS_NONE;
			case OBS_HAMMER:
				if(bState) return OBS_KILL;
				else return OBS_NONE;
			case OBS_BUTTON:
				if(bState) return OBS_NONE;
				else return ((iDirection << 8) | OBS_TOGGLE);
			case OBS_CRANE:
				if(bState) return ((iDirection << 8) | OBS_CRANE_MOVE);
				else return OBS_NONE;
			case OBS_GOAL:
				return OBS_WIN;
			case OBS_TIMER:
				{
					switch(Offset)
					{
					case 0:
						if(!ulTimer)
						{
							ulTimer=GetTickCount();
							ulElapsedTime=0;
							ulLastTime=GetTickCount();
						}
						else if(ulElapsedTime >= (unsigned long)((iDirection & 0x00FFFF00) >> 8) * 10)
							return ((iDirection & 0x000000FF) << 8) | OBS_SET_TIMER;
						break;
					case 1:
						ulLastTime=GetTickCount();
						ulElapsedTime=0;
						OffsetValue=GetTickCount();
						Offset=2;
						break;
					case 2:
						if(ulElapsedTime>=(unsigned long)(Disp.GetFace()*10))
							Offset=0;
						break;
					}
					ulElapsedTime+=(GetTickCount()-ulLastTime);
					ulLastTime=GetTickCount();
				}
				return OBS_NONE;
			case OBS_MULTI:
				if(bState) return ((iDirection & 0x01000000)>>17 | (iDirection << 8) | OBS_MULTI_TOGGLE);
				else return OBS_NONE;
			default:
				return OBS_NONE;
		}
	}
	return OBS_NONE;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

bool cObstacle::PlayerAffected(Character* Player)
{
	//Basic "is player in tile" evaluation
	//dav: rewritten to require player center in tile

	if(iType!=OBS_LASER && iType!=OBS_HAMMER)
	{
		if( abs( Player->Data.Pos.X - pPosition.X ) <= ( (TILE_SIZE) >> 1)
		&&  abs( Player->Data.Pos.Y - pPosition.Y ) <= ( (TILE_SIZE) >> 1) ) return true;
		else return false;
	}
	else if(iType==OBS_HAMMER)
	{
		switch (Disp.GetFace())
		{
		case UP:
		case DOWN:
			if(	Player->Data.Pos.X <= pPosition.X + TILE_SIZE + (( TILE_SIZE ) >> 1)
			&&	Player->Data.Pos.X >= pPosition.X - (( TILE_SIZE ) >> 1)
			&&  abs( Player->Data.Pos.Y - pPosition.Y ) <= ( (TILE_SIZE) >> 1) ) return true;
			else return false;
		case LEFT:
			if( abs( Player->Data.Pos.X - pPosition.X ) <= ( (TILE_SIZE) >> 1 ) 
			&&	Player->Data.Pos.Y >= pPosition.Y - TILE_SIZE - (( TILE_SIZE ) >> 1)
			&&	Player->Data.Pos.Y <= pPosition.Y + (( TILE_SIZE ) >> 1)) return true;
			else return false;
		case RIGHT:
			if( abs( Player->Data.Pos.X - pPosition.X ) <= ( (TILE_SIZE) >> 1 ) 
			&&	Player->Data.Pos.Y <= pPosition.Y + TILE_SIZE + (( TILE_SIZE ) >> 1)
			&&	Player->Data.Pos.Y >= pPosition.Y - (( TILE_SIZE ) >> 1)) return true;
			else return false;
		default:
			return false;
		}
	}
	else if(iType==OBS_LASER)
	{
		switch (Disp.GetFace())
		{
		case DIR_N:
			if( abs(Player->Data.Pos.X - pPosition.X) <= 4 &&
				(pPosition.Y - Player->Data.Pos.Y) <= iDirection*DEFAULTILESIZE &&
				(pPosition.Y - Player->Data.Pos.Y) > 0) return true;
			else return false;
		case DIR_S:
			if( abs(Player->Data.Pos.X - pPosition.X) <= 4 &&
				Player->Data.Pos.Y - pPosition.Y <= iDirection*DEFAULTILESIZE &&
				Player->Data.Pos.Y - pPosition.Y > 0) return true;
			else return false;
		case DIR_W:
			if( abs(Player->Data.Pos.Y - pPosition.Y) <= 4 &&
				(pPosition.X - Player->Data.Pos.X) <= iDirection*DEFAULTILESIZE &&
				(pPosition.X - Player->Data.Pos.X) > 0) return true;
			else return false;
		case DIR_E:
			if( abs(Player->Data.Pos.Y - pPosition.Y) <= 4 &&
				(Player->Data.Pos.X - pPosition.X) <= iDirection*DEFAULTILESIZE &&
				(Player->Data.Pos.X - pPosition.X) > 0) return true;
			else return false;
		default:
			return false;
		}
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////


cObstacleManager::cObstacleManager()
{
	for(int x=0; x < MAX_OBSTACLES; x++)
		ObstacleList[x] = NULL;
	iNumObstacles = lastFrame = 0;
	icCranePos.X=0;
	icCranePos.Y=16;
	iCraneActive=-1;
	GlobalCol.r=1.0f;
	GlobalCol.g=1.0f;
	GlobalCol.b=1.0f;
	GlobalCol.a=1.0f;
	iTTSpeed=250;

	LaserCol.r=1.0f;
	LaserCol.g=0.0f;
	LaserCol.b=0.0f;
	LaserCol.a=1.0f;
	return;
}

void cObstacleManager::CraneHandler(Character* Player)
{
	if(iCraneActive==-1)	// Double-check, just to make sure :)
		return;
	else
	{
		glLoadIdentity();
		glTranslated(icCranePos.X,icCranePos.Y,2);
		glColor3d(GlobalCol.r,GlobalCol.g,GlobalCol.b);

		float size=(float)DEFAULTILESIZE/2;
		float u,v;
		if(iCraneStage>=2 && iCraneStage<=4)
		{
			u=(float)CRANEX2;
			v=(float)CRANEY2;
		}
		else
		{
			u=(float)CRANEX1;
			v=(float)CRANEY1;
		}
		
		float eu=u+DEFAULTDIM,ev=v+DEFAULTDIM;	// Extended x/y values in the texture
		
		iCOORD temp;

		glBindTexture(GL_TEXTURE_2D, main.texture[OBJECTBMPID]);

		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&temp.X);
		glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&temp.Y);

		u/=(float)temp.X;	// Get the exact texture coordinates of the current sprite frame
		v/=(float)temp.Y;
		eu/=(float)temp.X;
		ev/=(float)temp.Y;

		glBegin(GL_QUADS);
			glTexCoord2f( u,1-ev);	glVertex2f(-size, size);
			glTexCoord2f(eu,1-ev);	glVertex2f( size, size);
			glTexCoord2f(eu,1-v);	glVertex2f( size,-size);
			glTexCoord2f( u,1-v);	glVertex2f(-size,-size);
		glEnd();

		double fpsGot=fps.Get();

		if(fpsGot<=35)
			iCraneSpeed=8;
		if(fpsGot>35 && fpsGot<=70)
			iCraneSpeed=5;
		else if(fpsGot>70 && fpsGot<=110)
			iCraneSpeed=4;
		else if(fpsGot>110 && fpsGot<=150)
			iCraneSpeed=3;
		else if(fpsGot>150 && fpsGot<=180)
			iCraneSpeed=2;
		else if(fpsGot>180)
			iCraneSpeed=1;

		if(iCraneStage>=1 && iCraneStage<=6)	// Draws the crane line
		{
			glLoadIdentity();
			glTranslated(0,0,2);

			glColor3f((float)GlobalCol.r/2,(float)GlobalCol.g/2,(float)GlobalCol.b/2);

			main.Texture2D(FALSE);
			glLineWidth(6);
			main.Culling(FALSE);

			glBegin(GL_LINES);
				glVertex2d(icCranePos.X, 0);
				glVertex2d(icCranePos.X, icCranePos.Y-16);
			glEnd();

			main.Culling(TRUE);

			main.Texture2D(TRUE);
		}

		switch(iCraneStage)
		{
		case 0:
			if(Player->Data.Pos.X>icCranePos.X+iCraneSpeed)		// Crane goes over
				icCranePos.X+=iCraneSpeed;
			else if(Player->Data.Pos.X<icCranePos.X-iCraneSpeed)
				icCranePos.X-=iCraneSpeed;
			else
			{
				iCraneStage++;
				icCranePos.X=Player->Data.Pos.X;
			}
			break;
		case 1:													// Crane drops down
			if(Player->Data.Pos.Y-24>icCranePos.Y+iCraneSpeed)
				icCranePos.Y+=iCraneSpeed;
			else if(Player->Data.Pos.Y-24<icCranePos.Y-iCraneSpeed)
				icCranePos.Y-=iCraneSpeed;
			else
			{
				iCraneStage++;
				icCranePos.Y=Player->Data.Pos.Y-24;
				iCraneTemp=0;
			}
			break;
		case 2:													// Crane goes back up
			if(iCraneTemp<64)
			{
				iCraneTemp+=iCraneSpeed;
				icCranePos.Y-=iCraneSpeed;
				Player->Data.Pos.Y-=iCraneSpeed;
			}
			else
				iCraneStage++;
			break;
		case 3:													// Crane goes to 2 tiles above dest
			if(icCranePos.Y>icCraneDirection.Y*32-64+iCraneSpeed)
			{
				icCranePos.Y-=iCraneSpeed;
				Player->Data.Pos.Y-=iCraneSpeed;
			}
			else if(icCranePos.Y<icCraneDirection.Y*32-64-iCraneSpeed)
			{
				icCranePos.Y+=iCraneSpeed;
				Player->Data.Pos.Y+=iCraneSpeed;
			}
			else
			{
				iCraneStage++;
				icCranePos.Y=icCraneDirection.Y*32-64;
			}
			break;
		case 4:													// Crane goes over
			Player->Data.Pos.X=icCranePos.X;
			if(icCraneDirection.X*32+16<icCranePos.X-iCraneSpeed)
				icCranePos.X-=iCraneSpeed;
			else if(icCraneDirection.X*32+16>icCranePos.X+iCraneSpeed)
				icCranePos.X+=iCraneSpeed;
			else
			{
				iCraneStage++;
				icCranePos.X=Player->Data.Pos.X=icCraneDirection.X*32+16;
			}
			break;
		case 5:													// Crane drops down
			if(icCraneDirection.Y*32>icCranePos.Y+iCraneSpeed)
			{
				icCranePos.Y+=iCraneSpeed;
				Player->Data.Pos.Y+=iCraneSpeed;
			}
			else if(icCraneDirection.Y*32<icCranePos.Y-iCraneSpeed)
			{
				icCranePos.Y-=iCraneSpeed;
				Player->Data.Pos.Y-=iCraneSpeed;
			}
			else
			{
				iCraneStage++;
				icCranePos.Y=icCraneDirection.Y*32;
				ObstacleList[iCraneActive]->ToggleState();
			}
			break;
		case 6:													// Crane rises to top
			if(icCranePos.Y>16+iCraneSpeed)
				icCranePos.Y-=iCraneSpeed;
			else
			{
				iCraneStage++;
				icCranePos.Y=16;
			}
			break;
		case 7:													// Crane leaves off closest side
			if(0<icCranePos.X && icCranePos.X<800)
			{
				if(icCranePos.X<=400)
					icCranePos.X-=iCraneSpeed;
				else
					icCranePos.X+=iCraneSpeed;
			}
			else
				iCraneStage++;
			break;
		case 8:													// Turn the crane off
			iCraneActive=-1;
			break;
		};
	}
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void cObstacleManager::RunObstacleEffects(Character* Player, Level level)
{
	int finalEffect = OBS_NONE;
	int finalObstacle = -1;
	int curEffect = OBS_NONE;
	
	for(int x = 0; x < iNumObstacles; x++)
	{
		curEffect = ObstacleList[x]->GetPlayerEffect(Player);
		if((finalEffect & 0x0000003f) < (curEffect & 0x0000003f))
		{
			if((curEffect & 0x0000003f)!=OBS_SET_TIMER)
			{
				finalEffect = curEffect;
				finalObstacle = x;
			}
			else
			{
				OkToToggle=false;
				ObstacleList[x]->ulTimer=GetTickCount();
				ObstacleList[x]->ulElapsedTime=0;
				ObstacleList[x]->ulLastTime=GetTickCount();
				ObstacleList[curEffect >> 8]->ToggleState();
			}
		}
	}

	//do effect to player or world
	if(iCraneActive!=-1)
		CraneHandler(Player);
	if(iCraneActive==-1 || iCraneStage>=6)
	{
		if(GetTickCount() > lastFrame)
		{
			lastFrame = GetTickCount()+15; //this can be tweaked, but should probably stay where it is

			switch (finalEffect & 0x0000003f)	//Gets last 6 bits
			{
				case OBS_NONE:
					Player->DisDir(-1);
					return;
				case OBS_MOVE_N:
					if(GetTickCount() > CBcounter)
					{
						int SFXNum=ObstacleList[finalObstacle]->GetSFX();
						if(!SFX[SFXNum].IsPlaying())
						{
							SFX[SFXNum].PlaySong();
							SFX[SFXNum].Volume(100);
						}
						CBcounter=GetTickCount()+CONVEYOR_FRAME_LIMIT;
						for(x = 0; x < (CONVEYOR_MOVE_AMOUNT / Player->Data.Speed) + 1; x++)
							Player->MoveChar(level, false, UP,false);
					}
					Player->DisDir(DOWN);
					return;
				case OBS_MOVE_S:
					if(GetTickCount() > CBcounter)
					{
						int SFXNum=ObstacleList[finalObstacle]->GetSFX();
						if(!SFX[SFXNum].IsPlaying())
						{
							SFX[SFXNum].PlaySong();
							SFX[SFXNum].Volume(100);
						}
						CBcounter=GetTickCount()+CONVEYOR_FRAME_LIMIT;
						for(x = 0; x < (CONVEYOR_MOVE_AMOUNT / Player->Data.Speed) + 1; x++)
							Player->MoveChar(level, false, DOWN,false);
					}
					Player->DisDir(UP);
					return;
				case OBS_MOVE_E:
					if(GetTickCount() > CBcounter)
					{
						int SFXNum=ObstacleList[finalObstacle]->GetSFX();
						if(!SFX[SFXNum].IsPlaying())
						{
							SFX[SFXNum].PlaySong();
							SFX[SFXNum].Volume(100);
						}
						CBcounter=GetTickCount()+CONVEYOR_FRAME_LIMIT;
						for(x = 0; x < (CONVEYOR_MOVE_AMOUNT / Player->Data.Speed) + 1; x++)
							Player->MoveChar(level, false, RIGHT,false);
					}
					Player->DisDir(LEFT);
					return;
				case OBS_MOVE_W:
					if(GetTickCount() > CBcounter)
					{
						int SFXNum=ObstacleList[finalObstacle]->GetSFX();
						if(!SFX[SFXNum].IsPlaying())
						{
							SFX[SFXNum].PlaySong();
							SFX[SFXNum].Volume(100);
						}
						CBcounter=GetTickCount()+CONVEYOR_FRAME_LIMIT;
						for(x = 0; x < (CONVEYOR_MOVE_AMOUNT / Player->Data.Speed) + 1; x++)
							Player->MoveChar(level, false, LEFT,false);
					}
					Player->DisDir(RIGHT);
					return;
				case OBS_TURN_CC:
					Player->DisDir(-1);
					if(GetTickCount() > TTcounter)
					{
						TTcounter = GetTickCount()+iTTSpeed;
						Player->Data.Facing = (Player->Data.Facing + 1) % NUM_DIRECTIONS;
					}
					return;
				case OBS_TURN_CCW:
					Player->DisDir(-1);
					if(GetTickCount() > TTcounter)
					{
						TTcounter = GetTickCount()+iTTSpeed;
						Player->Data.Facing = (Player->Data.Facing + NUM_DIRECTIONS - 1) % NUM_DIRECTIONS;
					}
					return;
				case OBS_TOGGLE:
					{
						int SFXNum=ObstacleList[finalObstacle]->GetSFX();
						if(!SFX[SFXNum].IsPlaying())
						{
							SFX[SFXNum].PlaySong();
						}
						Player->DisDir(-1);
						OkToToggle=false;
						ObstacleList[finalObstacle]->ToggleState();
						OkToToggle=true;
						ObstacleList[finalEffect >> 8]->ToggleState();
	//////PLEASE GET RID OF THIS IF IT DOESN'T WORK OUT/////////
						SynchAll();
	//////END OF PART TO DELETE//////////
					}
					return;
				case OBS_MULTI_TOGGLE:
					Player->DisDir(-1);
					if(finalEffect & 0x00000080)
						ObstacleList[(finalEffect & 0xFF000000) >> 24]->ToggleState();
					ObstacleList[(finalEffect & 0x00FF0000) >> 16 ]->ToggleState();
					ObstacleList[(finalEffect & 0x0000FF00) >> 8 ]->ToggleState();
					ObstacleList[finalObstacle]->ToggleState();
					if(OkToToggle)
						SynchAll();
					return;
				case OBS_CRANE_MOVE:
					icCraneDirection.X=(finalEffect>>8) & 0x00000000FF;
					icCraneDirection.Y=(finalEffect>>16) & 0x00000000FF;
					iCraneActive=finalObstacle;
					ObstacleList[finalObstacle]->ToggleState();
					Player->Data.Frame=1;
					iCraneStage=0;
					return;
				case OBS_KILL:
					iCraneStage=0;
					iCraneActive=-1;
					if(ObstacleList[finalObstacle]->GetSFX()==LASER)
					{
						SFX[ObstacleList[finalObstacle]->GetSFX()].PlaySong();
						SFX[ObstacleList[finalObstacle]->GetSFX()].Volume(50);
					}
					Player->LastObs=finalObstacle;
					Player->flags = PLAYER_DEAD;
					return;
				case OBS_WIN:
					Player->flags = PLAYER_WON;
					return;
				default:
					return;
			}
		}
	}
	return; 
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void cObstacleManager::RemObstacle(int ObstacleNum)
{
	unsigned char byte1,byte2,byte3;
	for(int a=0;a<iNumObstacles;a++)
	{
		switch(ObstacleList[a]->iType)
		{
		case OBS_BUTTON:
			if(ObstacleList[a]->iDirection>=iNumObstacles-1)
				ObstacleList[a]->iDirection--;
			else if(ObstacleList[a]->iDirection>ObstacleNum)
				ObstacleList[a]->iDirection--;
			break;
		case OBS_MULTI:
			byte1=ObstacleList[a]->iDirection & 0x000000FF;
			if(byte1>=iNumObstacles-1)
				byte1--;
			else if(byte1>ObstacleNum)
				byte1--;

			byte2=(ObstacleList[a]->iDirection & 0x0000FF00) >> 8;
			if(byte2>=iNumObstacles-1)
				byte2--;
			else if(byte2>ObstacleNum)
				byte2--;

			byte3=(ObstacleList[a]->iDirection & 0x00FF0000) >> 16;
			if(byte3>=iNumObstacles-1)
				byte3--;
			else if(byte3>ObstacleNum)
				byte3--;

			ObstacleList[a]->iDirection=ObstacleList[a]->iDirection & 0xFF000000;

			ObstacleList[a]->iDirection |= ((byte3<<16) | (byte2<<8) | (byte1));

			break;
		case OBS_TIMER:
			byte1=ObstacleList[a]->iDirection & 0x000000FF;
			if(byte1>=iNumObstacles-1)
				byte1--;
			else if(byte1>ObstacleNum)
				byte1--;

			ObstacleList[a]->iDirection=ObstacleList[a]->iDirection & 0xFFFFFF00;
			ObstacleList[a]->iDirection |= byte1;

			break;
		}
	}
	if(ObstacleNum<iNumObstacles)
	{
		for(a=ObstacleNum;a<iNumObstacles-1;a++)
			ObstacleList[a]=ObstacleList[a+1];
	}
	iNumObstacles--;
}

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

void cObstacleManager::DrawObstacles()
{
	int obsType = 0,x;
	
	for(x = 0; x < iNumObstacles; x++)
	{
		obsType = ObstacleList[x]->GetType();
		if(	obsType == OBS_CONVEYOR ||
			obsType == OBS_CRANE ||
			obsType == OBS_TURN ||
			obsType == OBS_BUTTON ||
			(obsType == OBS_PIT && ObstacleList[x]->bState) ||	// Makes sure the pit is on
			obsType == OBS_GOAL ||
			(obsType == OBS_MULTI && Editor) ||
			(obsType == OBS_TIMER && Editor) ||
			obsType == OBS_HAMMER ||
			obsType == OBS_WALL)
		{
			glColor3f(GlobalCol.r,GlobalCol.g,GlobalCol.b);		// Allows us to do disco mode
			if(ObstacleList[x]->bToggled && !Editor)
			{
				if(GetTickCount()>ObstacleList[x]->ulToggledTimer)
					ObstacleList[x]->bToggled=false;
				glColor3d(1,0,0);
			}
			ObstacleList[x]->Draw();
		}
		else if(obsType == OBS_LASER)
		{
			glColor3f(LaserCol.r,LaserCol.g,LaserCol.b);
			ObstacleList[x]->DrawLaser();
		}
	}
}