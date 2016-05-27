#ifndef __COMMON_H__
#define __COMMON_H__

///////////////////////////////////////////////////////////////////////////////////////
//      COMMON DEFINITIONS AND FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////

const char CURRVERSION[10]="0.94";

enum { DIR_NS = 0, DIR_EW = 1, DIR_N = 2, DIR_E = 3, DIR_S = 4,
        DIR_W = 5, DIR_CC = 6, DIR_CCW = 7, DIR_NONE = 8 };

enum {DOWN,LEFT,UP,RIGHT, NUM_DIRECTIONS};		// Enumerate all the basic directions
								//Dav: changed so that they're in order,
								//making rotation easier.

class cObstacle;

#define swap(a,b) (a^=b^=a^=b) //VERY, VERY handy (swaps two values)
#define GETBIT(b,a)	(((unsigned long)(b) & (1<<(a))) >> a)
#define TOGBIT(b,a)	((unsigned long)(b) ^ (1<<(a)))

#define OBJECTBMPID	2

#define CRANEX1	0
#define CRANEY1	160
#define CRANEX2	32
#define CRANEY2	160

#define TILE_SIZE 32 //tile width and height (square tiles)
#define PLAYER_SIZE 32 //player sprite width and height (square sprite)
#define MAX_OBSTACLES 255

#define CONVEYOR_FRAME_LIMIT 50
#define CONVEYOR_MOVE_AMOUNT 1

#define TURNTABLE_FRAME_LIMIT 5

#define PLAYER_DEAD 1 //dead
#define PLAYER_WON 2 //won current level
#define PLAYER_AUTO 4 //being controlled by program (crane)

#define DEFAULTDIM 32

#define DEFAULTILESIZE	32
#define DEFAULTILEX		25
#define DEFAULTILEY		18

#define NOANIMATIONS	0

// The default COORD structure is a SHORT, and that doesn't work in very many places....

struct iCOORD				// Integral COORD
{
	int X,Y;
};

struct fCOORD				// Floating point COORD
{
	float X,Y;
};

struct CharInfo
{
	iCOORD StartPos;		// These 3 have to do with the texture itself
	unsigned int TEXID;		// Texture position
	iCOORD BmpSize;

	iCOORD Pos;
	iCOORD TilePos;
	int Speed;
	int Facing;
	int Frame;
	int FrameCount;
	int FrameSpeed;
	bool up;
};

struct LevelInfo			// Our level data
{
	unsigned int TEXID;
	iCOORD BmpSize;

	int TilesInfo[DEFAULTILEX*DEFAULTILEY];
	int ColDetect[DEFAULTILEX*DEFAULTILEY];
};

struct ObjectInfo			// Our object data
{
	iCOORD StartPos;
	unsigned int TEXID;
	iCOORD BmpSize;

	iCOORD Pos;
	int Face;
	int Frame;
	int FrameCounter;
	unsigned int FrameSpeed;
	bool Bounce;
	bool up;
	unsigned int numAnims;
};

struct Mouse
{
	iCOORD Pos;
	bool Right,Left,Middle;
	bool hRight,hLeft,hMiddle;
};

struct ObstacleSave
{
	iCOORD Pos;
	int Dir;
	bool State;
	int Type;
	int Face;
};

struct FileData
{
	char Finalized;
	char Version[10];		// v0.1.1a

	char TileName[64];
	char ObjeName[64];
	char PlayName[64];

	iCOORD PlayerStart;

	int numObstacles;

	int LevelData[DEFAULTILEX*DEFAULTILEY];
	int LevelColDat[DEFAULTILEX*DEFAULTILEY];

	char NextLevel[64];

	ObstacleSave Obstacles[MAX_OBSTACLES];

	char Transition[64];
};

enum { LASER, NUMSFX };

#endif