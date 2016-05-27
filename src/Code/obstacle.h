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

#ifndef __OBSTACLE_H__
#define __OBSTACLE_H__

#include "common.h"
#include "Objects.h"
#include "Characters.h"

///////////////////////////////////////////////////////////////////////////////////////
//obstacle types
///////////////////////////////////////////////////////////////////////////////////////
enum { OBS_CONVEYOR, OBS_LASER, OBS_TURN, OBS_HAMMER, OBS_BUTTON,
        OBS_PIT, OBS_CRANE, OBS_WALL, OBS_GOAL, OBS_MULTI, OBS_TIMER, OBS_NUMOBS };

///////////////////////////////////////////////////////////////////////////////////////
//possible effects of obstacle on player
// OBS_NONE = no effect (laser if off, hammer if not right time, any disabled obstacle)
// OBS_BLOCK = blocks player, i.e. wall
// OBS_KILL = player dies (hammer at right time, laser while on, pit)
// OBS_MOVE_X = moves player 1 tile in direction X
// OBS_TURN_X = rotates player 90 degrees in direction X
// OBS_WIN = goal square, level complete
// OBS_CRANE = crane, upper byte contains target tile # = (destY*scrWidth)+destX
// OBS_TOGGLE = button, upper byte contains target object #
// OBS_SET_TIMER = sets a timer that toggles each obstacle after a certain amount of time
///////////////////////////////////////////////////////////////////////////////////////
enum { OBS_NONE, OBS_BLOCK, OBS_KILL, OBS_TURN_CC, OBS_TURN_CCW, OBS_MOVE_N, OBS_MOVE_E,
        OBS_MOVE_S, OBS_MOVE_W, OBS_WIN, OBS_CRANE_MOVE, OBS_TOGGLE, OBS_MULTI_TOGGLE,
		OBS_SET_TIMER };

///////////////////////////////////////////////////////////////////////////////////////
//possible iDirection values
///////////////////////////////////////////////////////////////////////////////////////
enum { DIR_NS = 0, DIR_EW = 1, DIR_N = 2, DIR_E = 3, DIR_S = 4,
        DIR_W = 5, DIR_CC = 6, DIR_CCW = 7, DIR_NONE = 8 };

///////////////////////////////////////////////////////////////////////////////////////
//possible bState values
///////////////////////////////////////////////////////////////////////////////////////
enum { STATE_ON = -1, STATE_OFF = 0, STATE_ACTIVATED = -1,
        STATE_UNACTIVATED = 0, STATE_ENABLED = -1, STATE_DISABLED = 0,
        STATE_FORWARD = -1, STATE_BACKWARD = 0 };
        
        

///////////////////////////////////////////////////////////////////////////////////////
/*************************************************************************************/     
///////////////////////////////////////////////////////////////////////////////////////

class cObstacle
{
	friend class LevelEditor;
	friend class cObstacleManager;
    protected: //yeah, I know classes default to private, but this is nice for reading's sake
        
		char			Offset;
		int				OffsetValue;
        PIXEL			pPosition; //position of object (level position, not screen)
        long			iDirection; //direction of object (orientation, target for cranes and buttons)
        bool			bState; //toggle state of object (on/off, activated, backward/forward, et cetera)
        int				iType; //type of obstacle
		unsigned int	TEX_ID; //stores texture ID
		unsigned long	ulTimer;
		unsigned long	ulElapsedTime;	// Elapsed number of milliseconds
		unsigned long	ulLastTime;	// Last logged number of milliseconds

		bool			bToggled;
		unsigned long	ulToggledTimer;
        
        bool			PlayerAffected(Character* Player);   //internal test to see if player is
															 //currently affected by obstacle,
															//can be overridden in derived classes
        Object			Disp;
		char			SFX;

    public:
    
        //initialization
        cObstacle() {};

		void Init(unsigned int TEXID, iCOORD Pos, unsigned int Face); //initialize display object

        void SetProps(PIXEL Pos, int dir, bool state, int type) { pPosition.X = Pos.X; pPosition.Y = Pos.Y; iDirection = dir; bState = state; iType = type;}
        
        //in-game functionality
        long GetPlayerEffect(Character* Player); //Returns code for effect (if any) the given obstacle has on the player
        void ToggleState(); //toggles state of object (can be generically called from a button)
		int GetType() { return iType; }; //returns type of obstacle
		void Draw();
		void DrawLaser();
		void DrawLaserToPos(int x, int y, int z);
		char GetSFX() { return SFX; };

		PIXEL GetObjPos(){return pPosition;};
};

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////

class cObstacleManager
{
	private:
		cObstacle*      ObstacleList[MAX_OBSTACLES];
		int             iNumObstacles;
		int             iCraneActive; //contains # of activated crane, -1 if none
		iCOORD			icCraneDirection;	//Where the crane will go
		iCOORD			icCranePos;			//Where the crane IS
		int				iCraneTemp;
		int				iCraneStage;
		int				iCraneSpeed;
		int				iTTSpeed;
		double			TTcounter;
		double			CBcounter;

		unsigned long	lastFrame;

		sColor			GlobalCol;
		sColor			LaserCol;
	public:

		bool Editor;

		cObstacleManager();
		~cObstacleManager() { Clear(); }
		
		int AddObstacle() { ObstacleList[iNumObstacles] = new cObstacle; iNumObstacles++; return iNumObstacles - 1; } //returns number of obstacle
		void RemObstacle(int ObstacleNum);
		
		void Clear() //resets ObstacleManager back to initial state (to load a new level)
		{
			for(int x=0; x < MAX_OBSTACLES; x++)
			{
				if(ObstacleList[x])
					delete ObstacleList[x];
				ObstacleList[x]=NULL;
			}
			iNumObstacles = 0;
			iCraneActive = -1;
			return;
		}
		
		cObstacle* GetObstacle(int ObsNum) { if(ObsNum >=0 && ObsNum < iNumObstacles) return ObstacleList[ObsNum]; else return NULL; } //returns pointer to obstacle requested
		
		void DrawObstacles();

		void ResetCrane() { iCraneActive=-1; iCraneStage=0; };

		void RunObstacleEffects(Character* Player, Level level); //determines overall effect to player this cycle

		void CraneHandler(Character* Player); //handles crane

		int GetNumObs(){return iNumObstacles;};

		int CraneControl() { return iCraneActive;};

		void SetTTSpeed(int Speed) { iTTSpeed=Speed; };

		int GetCraneStage() { return iCraneStage;};
		
		void SynchAll() { if(GetNumObs()>0) { for(int a=0;a<GetNumObs();a++) { cObstacle* temp; temp=GetObstacle(a); temp->Disp.Reset(); } } };
		
		void SetGlobalColor(sColor Global) { GlobalCol=Global; };
		void SetLaserColor(sColor Global) { LaserCol=Global; };
};

#endif