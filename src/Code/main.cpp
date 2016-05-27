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
// Main.cpp
// 
// By Jeff Verkoeyen
// 
// Purpose:
//		Main code for Escape From the Funky Factory
// 
// Version Number:  v0.8
// 
// Versions:
//		v0.1, 5/1/2003:
//			Includes character and a level.  Integrated sprite loading
//			with set-able alpha channels.  Basic movement with sprite
//			animations has been incorporated in to the engine.  Sprite
//			movement consists of three frames: first, middle, last.
//		v0.1a, 5/2/2003:
//			Same as yesterday's.....I just fixed some bugs and added more
//			comments.
//		v0.1b, 5/2/2003:
//			Added a few objects to the code: a Turntable and a conveyor belt
//			pretty simple, just wanted to show 'em off :)
//		v0.5, 6/9/2003:
//			Includes full game support, level editor, music, and all obstacles
//			have been completely integrated.  Support for file loading and
//			saving is included, and an auto-updated allows levels to
//			essentially edit themselves.
//		v0.72, 6/14/200:
//			Fixed many, MANY bugs and added new features, also.
//		v0.73, 7/11/200:
//			Fixing minor bugs, got the AVI intro finally fixed.
//		v0.8, 7/11/200:
//			Added hammers, fixed other bugs.
// 
// ©2003 TDA (The Developer's Alliance)
// 
// For loading levels:
// 0: unfinalized, playable/editable level
// 1: finalized, playable level
// 2: finalized, tutorial level
// 3: finalized, main game level
// 

///NEED THIS FOR FMOD TO WORK///
#pragma comment(lib, "fmodvc.lib")

int SCREENX=800,SCREENY=600,SCREENBITS=16;

///MUSIC STUFF///
enum { CLICKS, BGMUSIC, TMA, NUMSONGS };

#define MAXHIGHS	5

///ANIMATION SEQUENCES FOR THE GAME///
#define SETUP		-1		// For setting up the main window and other things
#define TRADEMARK	0		// Displays the trademark AVI
#define INTRO		1		// The spinning cube flies to the screen
#define TITLESLIDE	2		// The cube turns 2D and slides upwards
#define TEXTSLIDE	3		// Text slides in from the left
#define CHOOSE		4		// This is where the user chooses what to do
#define TRANSITION	5		// Transition picture being shown here
#define INGAME		6		// Inside the game, this is where the engine is run
#define CREDITS		7		// Showing the credits
#define PAUSED		8		// Paused in-game.
#define TUTORIALS	9		// The in-game tutorials

///TUTORIAL SEQUENCES///
#define BRIEFING	0		// Briefing the player
	#define BRIEFSETUP	0	// Briefing:setup subanimation
	#define BRIEFWALKIN	1	// Briefing:Timmy walks in from left
	#define BRIEFTEXT	2	// Briefing:Timmy introduces himself
	#define BRIEFTEXT2	3	// Briefing:More text, explaining the stuff
	#define BRIEFTEXT3	4	// Briefing:Goal
	#define BRIEFLEV1	5	// Briefing:First level showing off Goal obstacle
	#define BRIEFLEV2	6	// Briefing:Second level showing off winding passages
	#define BRIEFTEXT4	7	// Briefing:Conveyor Belts
	#define BRIEFLEV3	8	// Briefing:Third level showing off Conveyor Belts
	#define BRIEFTEXT5	9	// Briefing:Turn-Tables
	#define BRIEFLEV4	10	// Briefing:Fourth level showing off Turn-Tables
	#define BRIEFTEXT6	11	// Briefing:Pits
	#define BRIEFLEV5	12	// Briefing:Fifth level showing off Pits
	#define BRIEFTEXT7	13	// Briefing:Lasers
	#define BRIEFLEV6	14	// Briefing:Sixth level showing off Lasers
	#define BRIEFTEXT8	15	// Briefing:Buttons
	#define BRIEFLEV7	16	// Briefing:Seventh level showing off Buttons
	#define BRIEFTEXT9	17	// Briefing:Buttons
	#define BRIEFLEV8	18	// Briefing:Eighth level showing off Hammers
	#define BRIEFTEXT10	19	// Briefing:Cranes
	#define BRIEFLEV9	20	// Briefing:Ninth level showing off Cranes
	#define BRIEFEND	21	// Briefing:End of briefing
	#define BRIEFMAX	22	// Briefing:Max number of briefing animations

#define PASSWORDFILE	"passwords"

///INCLUDES///
#include "common.h"			// All the common variables
#include "OpenGL.h"			// So we can render OpenGL stuff easily
#include "FPS.h"			// Frames-per-second counter
OpenGL main;				// Create our OpenGL variable
FPS fps;					// Create our FPS variable
#include "Characters.h"		// For drawing, moving, and animating the main character
#include "Levels.h"			// Draws the tile-based level and keeps track of collision detection
#include "Objects.h"		// Draws and handles animations of the objects
#include "obstacle.h"		// Handles the obstacles with the obstacle manager
#include "CFmod.h"			// Plays music and stuff
#include "AVIPlayer.h"		// Plays the AVI files
#include "MATCRYPT.H"		// Encryption class
#include "resource.h"
mtxEncryptor Cryptor;

///FUNCTIONS///
bool LoadLevel(char* FileName, int LoadMode);	// Loads a level and returns the path to it
void LoadFile(char* FileName,char* Filter,char* Ex);	// Loads the file in to the engine
void HandleGame();			// Does all the basic drawing and movement
void RunTutorial();
void RunCredits();
void InitSongs();
char* GetName();
void SaveScores();
BOOL CALLBACK PasswordProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
///GLOBAL DEFINITIONS///
char GlobalDir[512];

Character MainChar;			// Create a character map out of our texture
Level LevelMap;				// Create a new level
cObstacleManager ObsMan;	// Create a new obstacle set
AVIPlayer Player;

int turnCount = 0;
unsigned long lastFrame = 0;

int LoadMode=0;				// Used to remember which load mode we want

////MODS////
bool DISCOMODE=false;

int LastSection;			// Used for the menu so that it knows where to jump back to
int Section=SETUP;			// Set the first section to start out in as the SETUP section
int Animation=0;			// Set the animation counter to 0, so that we can use it in the frame limiting

char trigger=0;				// Trigger system, you can have up to 8 triggers per level.
int SubTutAnim=0;			// Subanimations of each main animations
int RequestedAnim=-1;		// Requested animation

char Choice=0;				// Default choice that the user will make, set it to 0 as default
unsigned long LastFrame=0;	// This is required for frame limiting

HWND hName=NULL;
unsigned long BestScore=0;
char BestName[32];
char ReturnName[32];
bool Tutorial=false;
char CurrLevel[128];
char Password[9]="";
HWND hPasswnd=NULL;
unsigned long LastTick=0;
bool LoadedTransPic=false;
int LevelNumber=0;			// Level number in story mode

int RandSong=0;

char TransPic[64]="";		// This is for showing which transition picture we are going to use

FileData SaveData;

char* Songs;
int NumSongs=0;
char SFXNames[NUMSFX] [16]=
{
	{"laser.mp3"},
	{"conveyor.mp3"},
	{"button.mp3"}
};

CFmod Music[NUMSONGS];		// Songs
CFmod SFX[NUMSFX];			// Sound Effects

void SetOrtho(void)
{
	glViewport(0,0,SCREENX,SCREENY);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(0.0f,800,600,0.0f,-10.0f,10.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void SetPersp(void)
{
	glViewport(0,0,SCREENX,SCREENY);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)800/(GLfloat)600,0.1f,800.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void CheckPassword(void)
{
	if(Password[0]!='\0')
	{
		char tempDir[512];
		GetCurrentDirectory(511,tempDir);
		ifstream file;
		SetCurrentDirectory(GlobalDir);
		file.open(PASSWORDFILE,ios::in | ios::nocreate);
		if(!file.is_open())
			main.Error("Error",MB_OK,"Error opening passwords, please make sure that '%s' exists",PASSWORDFILE);
		else
		{
			char password[9];
			file.seekg(ios::beg);
			file.get(password,9,'\n');
			int testlevel=0;
			LevelNumber=0;
			while(file.peek()!=-1)
			{
				testlevel++;
				file.get(password,9,'\n');
				password[8]='\0';
				char decrypted[9];
				Cryptor.decrypt(password,decrypted);
				decrypted[8]='\0';
				for(int a=0;a<8;a++)
				{
					if(isupper(decrypted[a]))
						decrypted[a]=tolower(decrypted[a]);
				}
				if(!strcmp(Password,decrypted))
				{
					LevelNumber=testlevel;
					char tempDir[1024];
					sprintf(tempDir,"%s\\Official Levels",GlobalDir);
					SetCurrentDirectory(tempDir);

					char level[32]="Level ";

					if(LevelNumber<10)
						strcat(level,"0");
					sprintf(level,"%s%d.eff",level,LevelNumber);

					strcpy(CurrLevel,level);

					if(!LoadLevel(CurrLevel,3))
						main.keys[VK_RETURN]=main.keys[VK_SPACE]=false;
					else
					{
						main.Culling(TRUE);		// We HAVE to turn the view in to an ortho view here

						SetOrtho();

						Section=TRANSITION;
						if(DISCOMODE)
							Music[BGMUSIC].SetSpectrum(true);
						if(TransPic[0]!='\0')
						{
							if(!main.LoadTexture(TransPic,5))
								main.Error("Missing Transition Picture",0,"Could not find %s, please make sure it exists",TransPic);
							else
								LoadedTransPic=true;
						}
					}
					break;
				}
			}
			if(Section!=TRANSITION)
			{
				main.Error("Sorry",MB_OK,"Incorrect password, sorry.");
				SetCurrentDirectory(tempDir);
			}
			file.close();
		}
	}
}

void LoadPassword(void)
{
	if(Password[0]=='\0')
	{
		char tempDir[512];
		GetCurrentDirectory(511,tempDir);
		ifstream file;
		SetCurrentDirectory(GlobalDir);
		file.open(PASSWORDFILE,ios::in | ios::nocreate);
		if(!file.is_open())
			main.Error("Error",MB_OK,"Error opening passwords, please make sure that '%s' exists",PASSWORDFILE);
		else
		{
			char password[9];
			for(int a=0;a<LevelNumber;a++)
				file.get(password,9,'\n');
			file.get(password,9,'\n');
			password[8]='\0';
			file.close();
			char decrypted[9];
			Cryptor.decrypt(password,decrypted);
			decrypted[8]='\0';
			strcpy(Password,decrypted);
		}
		SetCurrentDirectory(tempDir);
	}
}

void LoadSong(void)
{
	static char lastsong[64]="";
	RandSong=rand()%NumSongs;

	char SongName[64];
	for(int a=0;a<64;a++)
	{
		if(Songs[RandSong*64+a]=='\0')
			break;
		SongName[a]=Songs[RandSong*64+a];
	}
	SongName[a]='\0';

	if(!strcmp(lastsong,SongName))	// Don't load the song again
	{
		Music[BGMUSIC].PlaySong();
		Music[BGMUSIC].Repeat(2);

		if(DISCOMODE)
			Music[BGMUSIC].SetSpectrum(true);
		return;
	}

	strcpy(lastsong,SongName);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glColor3d(1,1,1);

	main.glPrint(0,0,"Loading %s",SongName);
	SwapBuffers(main.hDC);

	char temp[512];
	GetCurrentDirectory(512,temp);

	char tempDir[1024];
	sprintf(tempDir,"%s\\Music",GlobalDir);

	SetCurrentDirectory(tempDir);

	if(!Music[BGMUSIC].LoadSong(SongName))
		main.Error("Error loading sound",MB_OK | MB_ICONINFORMATION,"Error loading sound, please make sure that '%s' exists and that your sound drivers are working.",SongName);

	SetCurrentDirectory(temp);

	Music[BGMUSIC].PlaySong();
	Music[BGMUSIC].Repeat(2);

	if(DISCOMODE)
		Music[BGMUSIC].SetSpectrum(true);
}

bool DrawScene()
{
	if(fps.Get()-45>0)			// This sets our fps limiting to about 50-60 fps, it makes sure
		Sleep((unsigned long)(fps.Get()-45));	// that the fps will NOT go above 70 fps (hopefully)

	if(Section!=SETUP && Section!=INGAME && Section!=TRADEMARK && Section!=TRANSITION)	// Only run the animations if we aren't in the game
	{										// or setting the window up
		if(GetTickCount()>LastFrame)
		{
			LastFrame=GetTickCount()+15;	// Only modify this once every 15 milliseconds or so

			if(fps.Get()<20)				// This sets the velocity of the animation according
				Animation+=6;				// to the fps of the users computer
			else if(fps.Get()>=20 && fps.Get()<=30)
				Animation+=4;
			else if(fps.Get()>=30 && fps.Get()<=80)
				Animation+=2;
		}
	}

	// This code makes it so you can skip the intro if you want to
	if(Section==INTRO || Section==TITLESLIDE || Section==TEXTSLIDE || Section==TRADEMARK)
	{
		if(main.keys[VK_SPACE] || main.keys[VK_RETURN] || main.keys[VK_ESCAPE])
		{
			main.keys[VK_SPACE]=main.keys[VK_RETURN]=main.keys[VK_ESCAPE]=false;
			if(Section==TRADEMARK)
			{
				Player.CloseAVI();
				Music[TMA].FreeSound();
			}
			Section=CHOOSE;
			Choice=0;
			SetPersp();
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// We need to clear the buffer every frame

	switch(Section)
	{
	case SETUP:
		main.Texture2D(TRUE);

		SetOrtho();

		main.CullFace(GL_BACK);		// Set the face to be culled
		main.Culling(TRUE);			// Turn on culling

		Section++;

		Player.SetTicker();
		break;
	case TRADEMARK:
		if(Player.GetFrame()>2 && Player.GetFrame()<15 && !Music[TMA].IsPlaying())
			Music[TMA].PlaySong();
		if(!Player.EndOfAVI())
			Player.DrawAVI();
		else
		{
			Section++;
			Player.CloseAVI();
			Music[TMA].FreeSound();
			SetPersp();
		}
		break;
	case INTRO:
		glLoadIdentity();
		if(Animation<500)	// Makes the cube fly to the screen
		{
			glTranslated(0,0,-600+Animation);
			glRotatef((float)Animation-140,0.5f,1.0f,0.7f);
		}
		else if(Animation<590)	// Makes the cube spin 90 degrees
		{
			glTranslated(0,0,-100);
			glRotatef((float)Animation-140,0.0f,1.0f,0.0f);
		}
		else
		{
			Section++;
			Animation=0;
			break;
		}

		main.SetTexture(4);

		glBegin(GL_QUADS);
			// The wierd 0.258 number is so that it cuts off the black on the bottom of the texture
			glTexCoord2f(1,1);		glVertex3d( 32, 24, 32);	// Front
			glTexCoord2f(0,1);		glVertex3d(-32, 24, 32);
			glTexCoord2f(0,0.258f);	glVertex3d(-32,-24, 32);
			glTexCoord2f(1,0.258f);	glVertex3d( 32,-24, 32);

			glTexCoord2f(1,1);		glVertex3d(-32, 24,-32);	// Back
			glTexCoord2f(0,1);		glVertex3d( 32, 24,-32);
			glTexCoord2f(0,0.258f);	glVertex3d( 32,-24,-32);
			glTexCoord2f(1,0.258f);	glVertex3d(-32,-24,-32);

			glTexCoord2f(1,1);		glVertex3d(-32, 24, 32);	// Left
			glTexCoord2f(0,1);		glVertex3d(-32, 24,-32);
			glTexCoord2f(0,0.258f);	glVertex3d(-32,-24,-32);
			glTexCoord2f(1,0.258f);	glVertex3d(-32,-24, 32);

			glTexCoord2f(1,1);		glVertex3d( 32, 24,-32);	// Right
			glTexCoord2f(0,1);		glVertex3d( 32, 24, 32);
			glTexCoord2f(0,0.258f);	glVertex3d( 32,-24, 32);
			glTexCoord2f(1,0.258f);	glVertex3d( 32,-24,-32);

			if(Animation<500)
			{
				glTexCoord2f(1,1);		glVertex3d( 32, 24,-32);	// Top
				glTexCoord2f(0,1);		glVertex3d(-32, 24,-32);
				glTexCoord2f(0,0.258f);	glVertex3d(-32, 24, 32);
				glTexCoord2f(1,0.258f);	glVertex3d( 32, 24, 32);
				
				glTexCoord2f(0,1);		glVertex3d(-32,-24,-32);	// Bottom
				glTexCoord2f(1,1);		glVertex3d( 32,-24,-32);
				glTexCoord2f(1,0.258f);	glVertex3d( 32,-24, 32);
				glTexCoord2f(0,0.258f);	glVertex3d(-32,-24, 32);
			}

		glEnd();

		break;
	case TITLESLIDE:	// Slides the title image up and shrinks it
		glLoadIdentity();

		glTranslated(0,Animation,-100);

		main.SetTexture(4);

		glBegin(GL_QUADS);
			glTexCoord2f(1,1);		glVertex3d( 32-Animation, 24-Animation, 32);	// Front
			glTexCoord2f(0,1);		glVertex3d(-32+Animation, 24-Animation, 32);
			glTexCoord2f(0,0.258f);	glVertex3d(-32+Animation,-24+Animation, 32);
			glTexCoord2f(1,0.258f);	glVertex3d( 32-Animation,-24+Animation, 32);
		glEnd();

		if(Animation>10)
		{
			Section++;
			Animation=0;
		}
		break;
	case TEXTSLIDE:		// Slides the text in from the right side
		glLoadIdentity();
		glTranslated(0,10,-100);

		main.SetTexture(4);

		glColor3d(1,1,1);

		glBegin(GL_QUADS);
			glTexCoord2f(1,1);		glVertex3d( 22, 14, 32);
			glTexCoord2f(0,1);		glVertex3d(-22, 14, 32);
			glTexCoord2f(0,0.258f);	glVertex3d(-22,-14, 32);
			glTexCoord2f(1,0.258f);	glVertex3d( 22,-14, 32);
		glEnd();

		glColor3f((float)Animation/320,1-(float)Animation/320,1-(float)Animation/160);

		main.glPrint(Animation*4,400,1.5f,2.0f,0,"Play");
		main.glPrint(Animation*4-65,432,1.5f,2.0f,0,"Load Custom");
		main.glPrint(Animation*4-35,464,1.5f,2.0f,0,"Password");
		main.glPrint(Animation*4-35,496,1.5f,2.0f,0,"Tutorial");
		main.glPrint(Animation*4-25,528,1.5f,2.0f,0,"Credits");
		main.glPrint(Animation*4,560,1.5f,2.0f,0,"Quit");

		if(Animation>88)
		{
			Section++;
			Animation=0;
		}
		break;
	case CHOOSE:
		ObsMan.ResetCrane();
		BestScore=0;
		LevelMap.StopTimer();
		glLoadIdentity();
		glTranslated(0,10,-100);

		main.SetTexture(4);

		glColor3d(1,1,1);

		glBegin(GL_QUADS);
			glTexCoord2f(1,1);		glVertex3d( 22, 14, 32);	// Front
			glTexCoord2f(0,1);		glVertex3d(-22, 14, 32);
			glTexCoord2f(0,0.258f);	glVertex3d(-22,-14, 32);
			glTexCoord2f(1,0.258f);	glVertex3d( 22,-14, 32);
		glEnd();

		glColor3f(0.549f,0.941f,0.749f);

		if(Choice==0)
			glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));
		main.glPrint(352,400,1.5f,2.0f,0,"Play");

		glColor3f(0.549f,0.941f,0.749f);

		if(Choice==1)
			glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));
		main.glPrint(287,432,1.5f,2.0f,0,"Load Custom");

		glColor3f(0.549f,0.941f,0.749f);

		if(Choice==2)
			glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));
		main.glPrint(317,464,1.5f,2.0f,0,"Password");

		glColor3f(0.549f,0.941f,0.749f);

		if(Choice==3)
			glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));
		main.glPrint(317,496,1.5f,2.0f,0,"Tutorial");

		glColor3f(0.549f,0.941f,0.749f);

		if(Choice==4)
			glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));
		main.glPrint(327,528,1.5f,2.0f,0,"Credits");

		glColor3f(0.549f,0.941f,0.749f);

		if(Choice==5)
			glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));
		main.glPrint(352,560,1.5f,2.0f,0,"Quit");

		if(main.keys[VK_UP])
		{
			if(!Music[CLICKS].IsPlaying())
				Music[CLICKS].PlaySong();
			main.keys[VK_UP]=false;
			Choice--;
		}
		if(main.keys[VK_DOWN])
		{
			if(!Music[CLICKS].IsPlaying())
				Music[CLICKS].PlaySong();
			main.keys[VK_DOWN]=false;
			Choice++;
		}
		if(Choice<0)
			Choice=5;
		if(Choice>5)
			Choice=0;

		if(main.keys[VK_RETURN] || main.keys[VK_SPACE])
		{
			main.keys[VK_RETURN]=main.keys[VK_SPACE]=false;
			switch(Choice)
			{
			case 0:			// Play
				{
					LevelMap.StopTimer();
					LevelNumber=1;
					char tempDir[1024];
					sprintf(tempDir,"%s\\Official Levels",GlobalDir);
					SetCurrentDirectory(tempDir);

					if(!LoadLevel("Level 01.eff",3))
						main.keys[VK_RETURN]=main.keys[VK_SPACE]=false;
					else
					{
						strcpy(CurrLevel,"Level 01.eff");
						main.Culling(TRUE);		// We HAVE to turn the view in to an ortho view here

						SetOrtho();

						Section=TRANSITION;
						if(DISCOMODE)
							Music[BGMUSIC].SetSpectrum(true);
						if(TransPic[0]!='\0')
						{
							if(!main.LoadTexture(TransPic,5))
								main.Error("Missing Transition Picture",0,"Could not find %s, please make sure it exists",TransPic);
							else
								LoadedTransPic=true;
						}
					}
				}
				break;
			case 1:			// Load Custom
				{
					LevelMap.StopTimer();
					LevelNumber=0;
					char FileName[MAX_PATH]=" ";
					char Filt1[]="EFFF Levels(*.eff)\0*.eff\0";
					char Filt2[]="eff";
					LoadFile(FileName,Filt1,Filt2);

					if(!LoadLevel(FileName,0))
						main.keys[VK_RETURN]=main.keys[VK_SPACE]=false;
					else
					{
						strcpy(CurrLevel,FileName);
						main.Culling(TRUE);		// We HAVE to turn the view in to an ortho view here

						SetOrtho();

						if(TransPic[0]=='\0')	// If there is no transition picture
						{
							LoadSong();
							Section=INGAME;
						}
						else
						{
							if(DISCOMODE)
								Music[BGMUSIC].SetSpectrum(true);
							Section=TRANSITION;
							if(!main.LoadTexture(TransPic,5))
								main.Error("Missing Transition Picture",0,"Could not find %s, please make sure it exists",TransPic);
							else
								LoadedTransPic=true;
						}
					}
				}
				break;
			case 2:
				DialogBox(main.hInstance,MAKEINTRESOURCE(IDD_PASSWORD),main.hWnd,(DLGPROC)PasswordProc);
				break;
			case 3:
				sColor color;
				color.r=1.0f;
				color.g=1.0f;
				color.b=1.0f;
				color.a=1.0f;
				LevelMap.SetGlobalColor(color);
				ObsMan.SetGlobalColor(color);
				MainChar.SetGlobalColor(color);
				color.g=0.0f;
				color.b=0.0f;
				ObsMan.SetLaserColor(color);

				Animation=0;
				SubTutAnim=0;
				iCOORD temp;
				temp.X=50;
				temp.Y=100;
				MainChar.Init(0,temp);
				MainChar.SetFace(RIGHT);
				MainChar.SetFrameSpeed(16);
				MainChar.SetSpeed(3);
				MainChar.flags=0;
				Section=TUTORIALS;
				SetOrtho();
				break;
			case 4:			// Credits
				main.Culling(TRUE);

				SetOrtho();

				Section=CREDITS;
				Animation=0;
				break;
			case 5:			// Quit
				return false;
				break;
			}
		}
		break;
	case TRANSITION:
		if(LastTick==0)
		{
			LastTick=GetTickCount()+60000;
			main.keys[VK_RETURN]=main.keys[VK_SPACE]=main.keys[VK_ESCAPE]=false;
		}
		if(LevelNumber>0)
		{
			strcpy(Password,"");
			LoadPassword();
			glColor3d(1,1,1);
			main.glPrint(0,0,"The password for level %d is: %s",LevelNumber,Password);
		}

		main.glPrint(600,0,"Press space/enter/escape to start the level.");

		if(LoadedTransPic)
		{
			main.SetTexture(5);
			glLoadIdentity();
			glTranslated(400,300,0);
			
			glColor3d(1,1,1);
			
			glBegin(GL_QUADS);		// Display the transition picture
				glTexCoord2f(1,1);	glVertex2d( 256,-256);
				glTexCoord2f(0,1);	glVertex2d(-256,-256);
				glTexCoord2f(0,0);	glVertex2d(-256, 256);
				glTexCoord2f(1,0);	glVertex2d( 256, 256);
			glEnd();
		}
		if(GetTickCount()>LastTick || main.keys[VK_RETURN] || main.keys[VK_SPACE] || main.keys[VK_ESCAPE])
		{
			Password[0]='\0';
			if(!Music[BGMUSIC].IsPlaying())
				LoadSong();
			Section=INGAME;
			LoadedTransPic=false;
			LastTick=0;
		}
		break;
	case INGAME:
		if(LevelMap.TimerBegin==0)
			LevelMap.StartTimer();
		glColor3d(1,1,1);
		if(main.keys['D'] && !main.lkeys['D'])
		{
			DISCOMODE=!DISCOMODE;
			if(!DISCOMODE)
			{
				sColor color;
				color.r=1.0f;
				color.g=1.0f;
				color.b=1.0f;
				color.a=1.0f;
				LevelMap.SetGlobalColor(color);
				ObsMan.SetGlobalColor(color);
				MainChar.SetGlobalColor(color);
				color.g=0.0f;
				color.b=0.0f;

				ObsMan.SetLaserColor(color);

				color.r=0.0f;
				main.ClearColor(color);
				Music[BGMUSIC].SetSpectrum(false);
			}
			else
				Music[BGMUSIC].SetSpectrum(true);
		}

		if(DISCOMODE)
		{
			float* Spectrum;

			Spectrum=Music[BGMUSIC].GetSpectrum();

			sColor color;
			float avg=0;

			for(int a=0;a<10;a++)
				avg+=Spectrum[a];
			avg/=10;
			if(avg>=0.7f)
			{
				color.r=avg;
				color.g=avg+0.2f;
				color.b=avg+0.1f;
			}
			else if(avg<0.7f && avg>=0.4f)
			{
				color.r=avg+0.2f;
				color.g=avg-0.2f;
				color.b=avg-0.1f;
			}
			else
			{
				color.r=avg-0.2f;
				color.g=avg-0.2f;
				color.b=avg+0.3f;
			}

			LevelMap.SetGlobalColor(color);
			ObsMan.SetGlobalColor(color);
			ObsMan.SetLaserColor(color);
			MainChar.SetGlobalColor(color);
			color.g=0.0f;
			color.b=0.0f;

			main.ClearColor(color);
		}

		if(MainChar.flags == PLAYER_WON)
		{
			BestScore=0;
			LevelMap.StopTimer();
			if(LevelNumber>0)
				LevelNumber++;
			sColor color;
			color.r=0.0f;
			color.g=0.0f;
			color.b=0.0f;
			color.a=1.0f;
			main.ClearColor(color);
			Music[BGMUSIC].StopSong();	// This stops the song so that it doesn't play during
										// the transition picture
			SaveScores();
			strcpy(CurrLevel,LevelMap.NextLevel);
			if(!LoadLevel(LevelMap.NextLevel,LoadMode))
			{
				Animation=0;
				Music[BGMUSIC].StopSong();
				Section=CHOOSE;
				SetPersp();
				break;
			}
			Animation=0;
			if(LevelNumber>0)
			{
				Section=TRANSITION;
				if(DISCOMODE)
					Music[BGMUSIC].SetSpectrum(true);
				Animation=0;
				if(TransPic[0]!='\0')
				{
					if(!main.LoadTexture(TransPic,5))
						main.Error("Missing Transition Picture",0,"Could not find %s, please make sure it exists",TransPic);
					else
						LoadedTransPic=true;
				}
			}
			else
			{
				if(TransPic[0]=='\0')
					LoadSong();
				else
				{
					Section=TRANSITION;
					if(!main.LoadTexture(TransPic,5))
						main.Error("Missing File",0,"Could not find %s, please make sure it exists",TransPic);
				}
			}
			MainChar.flags=0;			// Reset all the player's flags for a fresh new game
		}
		else if(MainChar.flags == PLAYER_DEAD)
		{
			BestScore=0;
			LevelMap.StopTimer();
			sColor color;
			color.r=0.0f;
			color.g=0.0f;
			color.b=0.0f;
			color.a=1.0f;
			main.ClearColor(color);
			glColor3d(1,1,1);
			main.SetTexture(6);
			glLoadIdentity();
			glTranslated(400,300,0);
			
			glColor3d(1,1,1);
			
			glBegin(GL_QUADS);
				glTexCoord2f(1,1);		glVertex2d( 400,-300);
				glTexCoord2f(0,1);		glVertex2d(-400,-300);
				glTexCoord2f(0,0.258f);	glVertex2d(-400, 300);
				glTexCoord2f(1,0.258f);	glVertex2d( 400, 300);
			glEnd();
//DEBUG		main.glPrint(0,0,"%s",CurrLevel);
			if(main.keys[VK_RETURN] || main.keys[VK_SPACE])
			{
				main.keys[VK_RETURN]=main.keys[VK_SPACE]=false;

				if(LoadLevel(CurrLevel,LoadMode))
				{
					MainChar.flags=0;
					if(!Music[BGMUSIC].IsPlaying())
						LoadSong();
				}
			}

			if(main.keys[VK_ESCAPE])
			{
				main.keys[VK_ESCAPE]=false;
				LastSection=Section;
				Section=PAUSED;
			}
		}
		else
		{
			if(BestScore==0)
			{
				FILE* FileRead;
				char tempFileName[128];
				sprintf(tempFileName,"%s.highs",CurrLevel);
				FileRead=fopen(tempFileName,"rb");

				if(FileRead)
				{
					fread(&BestScore,sizeof(unsigned long),1,FileRead);
					fread(&BestName,sizeof(char)*32,1,FileRead);
					for(int a=0;a<32;a++)
					{
						if(BestName[a]=='\0')
							break;
						BestName[a]+=50;
					}
				}
				else
					BestScore=1;
			}
			if(BestScore>1)
				main.glPrint(100,582,0,"Best Time by %s at %s",BestName,LevelMap.ConvertTime(BestScore));
			else if(BestScore==1)
				main.glPrint(100,582,0,"No best score detected.");

			if(main.keys[VK_ESCAPE] && !main.lkeys[VK_ESCAPE])
			{
				sColor color;
				color.r=0.0f;
				color.g=0.0f;
				color.b=0.0f;
				color.a=1.0f;
				main.ClearColor(color);
				LastSection=Section;
				Section=PAUSED;
				Choice=0;
				Music[BGMUSIC].SetSpectrum(true);
				LevelMap.Pause();
			}

	//		main.glPrint(0,582,0,"FPS: %i",(int)(fps.Get() + .5));	// Output our frames per second
			glColor3d(1,1,1);
			LevelMap.PrintTime();
			//MainChar.Debug(LevelMap);

			HandleGame();
		}
		break;
	case CREDITS:
		RunCredits();
		break;
	case PAUSED:
		{
			if(Music[BGMUSIC].IsPlaying())
			{
				float* Spectrum;

				Spectrum=Music[BGMUSIC].GetSpectrum();

				glLoadIdentity();
				glTranslated(0,0,0);

				main.Texture2D(false);

				int b=0;

				glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));

				glBegin(GL_QUADS);
					for(float a=0;a<800;a+=3.125)
					{
						glVertex2f(a,600);
						glVertex2f(a+3.125f,600);
						glVertex2f(a+3.125f,600-Spectrum[b]*600-5);
						glVertex2f(a,600-Spectrum[b]*600-5);
						b++;
					}
				glEnd();
			
				main.Texture2D(true);
			}

			glColor3f(0.549f,0.941f,0.749f);

			if(Choice==0)
				glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));
			main.glPrint(352,250,1.5f,2.0f,0,"Resume");

			glColor3f(0.549f,0.941f,0.749f);

			if(Choice==1)
				glColor3f(1.0f,(float)cos((float)Animation/50),(float)sin((float)Animation/50));
			main.glPrint(370,282,1.5f,2.0f,0,"Quit");

			if(main.keys[VK_UP])
			{
				main.keys[VK_UP]=false;
				if(!Music[CLICKS].IsPlaying())
					Music[CLICKS].PlaySong();
				Choice--;
				if(Choice<0)
					Choice=1;
			}
			if(main.keys[VK_DOWN])
			{
				main.keys[VK_DOWN]=false;
				if(!Music[CLICKS].IsPlaying())
					Music[CLICKS].PlaySong();
				Choice++;
				if(Choice>1)
					Choice=0;
			}

			if(main.keys[VK_RETURN] || main.keys[VK_SPACE])
			{
				main.keys[VK_RETURN]=main.keys[VK_SPACE]=false;

				switch(Choice)
				{
				case 0:
					Section=LastSection;
					LevelMap.UnPause();
					break;
				case 1:
					SetPersp();
					Choice=0;
					Section=CHOOSE;
					Music[BGMUSIC].StopSong();
					break;
				}
			}
			if(main.keys[VK_ESCAPE] && !main.lkeys[VK_ESCAPE])
			{
				LevelMap.UnPause();
				Section=LastSection;
			}
		}
		break;
	case TUTORIALS:
		RunTutorial();
		break;
	}
	return true;
}

void HandleGame()
{
	ObsMan.RunObstacleEffects(&MainChar, LevelMap);

	if(GetTickCount() > lastFrame)		// Checks our frame limiting
	{
		lastFrame = GetTickCount()+30;	// This can be tweaked, but should probably stay where it is

		if(ObsMan.CraneControl()<0 || ObsMan.GetCraneStage()>=5)	// This allows you to move
		{												// if and only if the crane is not picking Timmy up
			if(main.keys[VK_UP] && !main.keys[VK_DOWN])						// Do keypresses here and move the character
				MainChar.MoveChar(LevelMap, true);

			if(main.keys[VK_DOWN] && !main.keys[VK_UP])
				MainChar.MoveChar(LevelMap, false, (MainChar.Data.Facing + 2) % NUM_DIRECTIONS);

			if((!main.keys[VK_UP] && !main.keys[VK_DOWN]) || (main.keys[VK_UP] && main.keys[VK_DOWN]))
				MainChar.Data.Frame=1;

			if(main.keys[VK_RIGHT] && !main.keys[VK_LEFT] && turnCount == 0)
				MainChar.Data.Facing = (MainChar.Data.Facing + 1) % NUM_DIRECTIONS;
			else if(main.keys[VK_LEFT] && !main.keys[VK_RIGHT] && turnCount == 0)
				MainChar.Data.Facing = (MainChar.Data.Facing + NUM_DIRECTIONS - 1) % NUM_DIRECTIONS;

			if(!(main.keys[VK_LEFT] || main.keys[VK_RIGHT])) turnCount = 0;
			else turnCount++;

			if(turnCount >= 20) turnCount = 0;
		}
	}

	glColor3d(1,1,1);							// Set back the default color
	
	//DRAWLEVEL 0
	LevelMap.DrawLevel();

	//DRAWLEVEL 1
	ObsMan.DrawObstacles();

	MainChar.DrawCharToScreen();				// Draw our character
}

void LoadFile(char* FileName, char* Filter, char* Ex)
{
	strcpy(FileName,"");
	FileName[0]='\0';
	ShowCursor(true);
	OPENFILENAME ofn;
	
	ZeroMemory(&ofn, sizeof(ofn));	// Empty out the file structure
	
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = main.hWnd;
	ofn.lpstrFilter = Filter;
	
	char temp[512];
	
	GetCurrentDirectory(512, temp);	// We need to get the currdir so that we can set the default
									// directory
	ofn.lpstrInitialDir=temp;

	ofn.lpstrFile = FileName;
	ofn.hwndOwner = main.hWnd;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = Ex;

	GetOpenFileName(&ofn);
	ShowCursor(false);
}

bool LoadLevel(char* FileName, int loadmode)	// Loads a file in to the engine itself
{
	if(FileName[0]!='\0')
	{
		FILE *savefile;
		savefile = fopen(FileName,"rb");
		if(savefile)
		{
			fread(&SaveData.Finalized,sizeof(char),1,savefile);

			if(SaveData.Finalized==2 && loadmode!=2)
			{
				main.Error("Tutorial Level",0,"You are not allowed to open this level, as it is a tutorial level.");
				fclose(savefile);
				return false;
			}

			if(SaveData.Finalized!=3 && loadmode==3)
			{
				main.Error("Expected Game Level",0,"%s was expected to be a Game Level, please make sure that the level has not been modified.",FileName);
				fclose(savefile);
				return false;
			}

			if(SaveData.Finalized==3 && loadmode!=3)
			{
				main.Error("Game Level",0,"You are not allowed to play Game Levels outside of the actual StoryMode");
				fclose(savefile);
				return false;
			}

			if(SaveData.Finalized!=2 && loadmode==2)
			{
				main.Error("Corrupted level.",0,"This file is meant to be opened as a tutorial level, but it is corrupted.");
				fclose(savefile);
				return false;
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

			for(a=0;a<SaveData.numObstacles;a++)	// Reads in all the obstacles
			{
				fread(&SaveData.Obstacles[a].Pos,sizeof(iCOORD),1,savefile);

				fread(&SaveData.Obstacles[a].Dir,sizeof(int),1,savefile);
				fread(&SaveData.Obstacles[a].State,sizeof(bool),1,savefile);
				fread(&SaveData.Obstacles[a].Type,sizeof(int),1,savefile);
				fread(&SaveData.Obstacles[a].Face,sizeof(int),1,savefile);
			}

			ObsMan.Clear();

			sColor Alpha;
			Alpha.r=255;
			Alpha.g=0;
			Alpha.b=255;
			
			main.SetAlpha(Alpha);
			
			main.LoadTexture(SaveData.PlayName,0);
			main.LoadTexture(SaveData.TileName,1);
			main.LoadTexture(SaveData.ObjeName,OBJECTBMPID);

			strcpy(LevelMap.NextLevel,SaveData.NextLevel);

			LevelMap.Init(1);

			for(a=0;a<DEFAULTILEX*DEFAULTILEY;a++)
			{
				LevelMap.Data.TilesInfo[a]=SaveData.LevelData[a];
				LevelMap.Data.ColDetect[a]=SaveData.LevelColDat[a];
			}

			MainChar.Init(0,SaveData.PlayerStart);	// Set the defaults for Timmy's animations
			MainChar.SetFace(DOWN);
			MainChar.SetFrameSpeed(35);
			MainChar.SetSpeed(3);
			MainChar.flags=0;

			cObstacle* obs;

			for(a=0;a<SaveData.numObstacles;a++)
			{
				obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				obs->SetProps(SaveData.Obstacles[a].Pos, SaveData.Obstacles[a].Dir, SaveData.Obstacles[a].State, SaveData.Obstacles[a].Type);
				obs->Init(2, SaveData.Obstacles[a].Pos, SaveData.Obstacles[a].Face);
// Debug		main.Error("blah",0,"%dx%d",SaveData.Obstacles[a].Pos.X,SaveData.Obstacles[a].Pos.Y);
			}

			if(array[0]=='v')	// Cut out the v from the older versions
			{
				for(a=0;a<9;a++)
					array[a]=array[a+1];
			}

			double temp=atof(array);	// We need to get the floating point value of the version

			if(temp<0.9)	// If the level is outdated.....
			{
				if(temp<0.5)
					strcpy(TransPic,"");
				else
				{
					fread(&SaveData.Transition,sizeof(char)*64,1,savefile);
					strcpy(TransPic,SaveData.Transition);
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
					fwrite(&TransPic,sizeof(char)*64,1,savefile);
					fclose(savefile);
				}
			}
			else
			{
				fread(&SaveData.Transition,sizeof(char)*64,1,savefile);
				strcpy(TransPic,SaveData.Transition);
				fclose(savefile);
			}
			LoadMode=loadmode;
			return true;
		}
		else
		{
			main.Error("Error finding file",0,"Could not find %s, please make sure that it exists.",FileName);
			return false;
		}
	}
	return false;
}

BOOL CALLBACK PasswordProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hWnd,IDC_PASSWORDBOX),EM_SETLIMITTEXT,8,NULL);
		SetFocus(GetDlgItem(hWnd,IDC_PASSWORDBOX));
		ShowCursor(true);
		break;
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		EndDialog(hWnd,0);
		hPasswnd=NULL;
		ShowCursor(false);
		EnableWindow(main.hWnd,true);
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OK:
			GetDlgItemText(hWnd,IDC_PASSWORDBOX,Password,9);
			CheckPassword();

			EndDialog(hWnd,0);
			hPasswnd=NULL;
			return true;
			break;
		}
		break;
	}
	return FALSE;
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch(uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if(!HIWORD(wParam))
				main.active=TRUE;
			else
				main.active=FALSE;
			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch(wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			main.keys[wParam]=TRUE;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			main.keys[wParam]=FALSE;					// If So, Mark It As FALSE
			return 0;								// Jump Back
		}

		case WM_SIZE:								// Resize The OpenGL Window
		{
			main.ResizeGLScene(LOWORD(lParam),HIWORD(lParam)); // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					LPSTR		lpCmdLine,
					int			nCmdShow)
{
	GetCurrentDirectory(512,GlobalDir);
	Player.SetRepeat(false);

	MSG		msg;

	bool done=false;

	bool FULL=true;

	if(!strcmp(lpCmdLine,"-win"))	// Starts the window off in windowed mode
		FULL=false;
	
	if(!main.CreateGLWindow("Escape From the Funky Factory",SCREENX,SCREENY,SCREENBITS,FULL,IDI_ICON1))
		return 0;				// Create our window using the OpenGL.h header's create window command

	main.SetNumTexts(10);		// 10 textures should be fine for now

	main.Texture2D(TRUE);		// Enable textures

	if(!Player.OpenAVI("DEVALLY.avi"))
	{
		main.KillGLWindow();
		return false;
	}

	sColor Alpha;				// Make a temporary Alpha color channel

	Alpha.r=0;					// Set the color to black for the font
	Alpha.g=0;
	Alpha.b=0;

	main.SetAlpha(Alpha);		// Set the Alpha color

	main.SetFiltering(true);

	if(!main.LoadFont("font.bmp"))	// Load the font with black being transparent
	{
		main.KillGLWindow();
		return 0;
	}

	main.BuildFont();			// Create our font

	main.SetFiltering(false);	// Turn off the filtering (crisper pictures)

	Alpha.r=0;				// Set the background color
	Alpha.g=0;
	Alpha.b=0;
	main.ClearColor(Alpha);		// Set the background color to white

	ObsMan.Editor=false;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	main.glPrint(300,0,"Loading Title.bmp");
	SwapBuffers(main.hDC);

	main.LoadTexture("Title.bmp",4);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	main.glPrint(300,300,0,"Loading GameOver.bmp");
	SwapBuffers(main.hDC);

	main.LoadTexture("GameOver.bmp",6);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	main.glPrint(300,0,"Loading Clicks.mp3");
	SwapBuffers(main.hDC);

	if(!Music[CLICKS].Init("Click.mp3"))
		main.Error("Error loading sound",MB_OK | MB_ICONINFORMATION,"Error loading sound, please make sure that click.mp3 exists and that your sound drivers are working.");

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	main.glPrint(300,0,"Loading TMA.mp3");
	SwapBuffers(main.hDC);

	if(!Music[TMA].LoadSong("TMA.mp3"))
		main.Error("Error loading sound",MB_OK | MB_ICONINFORMATION,"Error loading sound, please make sure that TMA.mp3 exists and that your sound drivers are working.");

	char temp[1024];
	sprintf(temp,"%s\\Sound Effects",GlobalDir);
	SetCurrentDirectory(temp);
	for(int a=0;a<NUMSFX;a++)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		main.glPrint(300,0,"Loading %s",SFXNames[a]);
		SwapBuffers(main.hDC);

		if(!SFX[a].LoadSong(SFXNames[a]))
			main.Error("Error loading sound",MB_OK | MB_ICONINFORMATION,"Error loading sound, please make sure that %s exists and that your sound drivers are working.",SFXNames[a]);
	}

	InitSongs();

	fps.Set(1,100);				// Init the FPS counter

	srand(GetTickCount());

	while(!done)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if(msg.message==WM_QUIT)
				done=TRUE;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(main.active)
			{
				fps.Start();	// Start the FPS counter
				if(!DrawScene())
					done=TRUE;
				else
				{
					SwapBuffers(main.hDC);	// Flip the buffers
					main.UpdateKeys();	// Update the keys
				}
				fps.End();		// End our FPS counter
			}
		}
	}

	main.KillFont();

	main.KillGLWindow();

	Music[a].FreeSoundSystem();
	return(msg.wParam);
}

void RunTutorial()
{
	if(main.keys[VK_ESCAPE])
	{
		main.keys[VK_ESCAPE]=false;
		Section=CHOOSE;
		SetPersp();
		return;
	}
	if(main.keys[VK_SPACE])
	{
		main.keys[VK_SPACE]=false;
		RequestedAnim=SubTutAnim+1;
		if(SubTutAnim+1>=BRIEFMAX)
		{
			Section=CHOOSE;
			SetPersp();
		}
	}
	if(main.keys['Z'])
	{
		main.keys['Z']=false;
		RequestedAnim=SubTutAnim-1;
		if(SubTutAnim-1<=BRIEFSETUP)
		{
			Section=CHOOSE;
			SetPersp();
		}
	}
	if(RequestedAnim!=-1)
	{
		trigger=0;
		if(RequestedAnim>=BRIEFMAX)
		{
			Section=CHOOSE;
			SetPersp();
		}
		SubTutAnim=RequestedAnim;
		switch(RequestedAnim)
		{
		case BRIEFWALKIN:
			iCOORD temp;
			temp.X=50;
			temp.Y=100;
			MainChar.Init(0,temp);	// Set the defaults for Timmy's animations
			MainChar.SetFace(RIGHT);
			MainChar.SetFrameSpeed(16);
			MainChar.SetSpeed(3);
			MainChar.flags=0;
			LevelMap.ClearData();
			break;
		case BRIEFTEXT:
			MainChar.Data.Pos.Y=300;
			MainChar.Data.Pos.X=400;
			MainChar.Data.Facing=DOWN;
			MainChar.Data.Frame=1;
			LevelMap.ClearData();
			break;
		case BRIEFTEXT3:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
				ObsMan.Clear();
				cObstacle* obs=ObsMan.GetObstacle(ObsMan.AddObstacle());

				iCOORD tempPos;
				tempPos.X=16;
				tempPos.Y=32;
				
				obs->SetProps(tempPos, UP, true, OBS_GOAL);
				obs->Init(2, tempPos, UP);
			}
			break;
		case BRIEFLEV1:
			if(!LoadLevel("Tut1.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFLEV2:
			if(!LoadLevel("Tut2.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFTEXT4:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
				ObsMan.Clear();
				cObstacle* obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				
				iCOORD tempPos;
				tempPos.X=16;
				tempPos.Y=64;
				
				obs->SetProps(tempPos, UP, true, OBS_CONVEYOR);
				obs->Init(2, tempPos, UP);
			}
			break;
		case BRIEFLEV3:
			if(!LoadLevel("Tut3.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFTEXT5:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
				ObsMan.Clear();
				cObstacle* obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				
				iCOORD tempPos;
				tempPos.X=16;
				tempPos.Y=64;
				
				obs->SetProps(tempPos, OBS_TURN_CCW, true, OBS_TURN);
				obs->Init(2, tempPos, UP);
			}
			break;
		case BRIEFLEV4:
			if(!LoadLevel("Tut4.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFTEXT6:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
				ObsMan.Clear();
				cObstacle* obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				
				iCOORD tempPos;
				tempPos.X=16;
				tempPos.Y=48;
				
				obs->SetProps(tempPos, OBS_TURN_CCW, true, OBS_PIT);
				obs->Init(2, tempPos, UP);
			}
			break;
		case BRIEFLEV5:
			if(!LoadLevel("Tut5.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFTEXT7:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
				ObsMan.Clear();
				cObstacle* obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				
				iCOORD tempPos;
				tempPos.X=16;
				tempPos.Y=112;
				
				obs->SetProps(tempPos, DIR_E, true, OBS_LASER);
				obs->Init(2, tempPos, RIGHT);
			}
			break;
		case BRIEFLEV6:
			if(!LoadLevel("Tut9.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFTEXT8:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
				ObsMan.Clear();
				cObstacle* obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				
				iCOORD tempPos;
				tempPos.X=16;
				tempPos.Y=64;
				
				obs->SetProps(tempPos, 0, false, OBS_BUTTON);
				obs->Init(2, tempPos, UP);
			}
			break;
		case BRIEFLEV7:
			if(!LoadLevel("Tut6.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFTEXT9:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
				ObsMan.Clear();
				cObstacle* obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				
				iCOORD tempPos;
				tempPos.X=16;
				tempPos.Y=80;
				
				obs->SetProps(tempPos, UP, true, OBS_HAMMER);
				obs->Init(2, tempPos, UP);
			}
			break;
		case BRIEFLEV8:
			if(!LoadLevel("Tut7.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFTEXT10:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
				ObsMan.Clear();
				cObstacle* obs=ObsMan.GetObstacle(ObsMan.AddObstacle());
				
				iCOORD tempPos;
				tempPos.X=16;
				tempPos.Y=80;
				
				obs->SetProps(tempPos, 0, false, OBS_CRANE);
				obs->Init(2, tempPos, UP);
			}
			break;
		case BRIEFLEV9:
			if(!LoadLevel("Tut8.eff",2))
			{
				Animation=0;
				SetPersp();
				Section=CHOOSE;
			}
			break;
		case BRIEFEND:
			{
				MainChar.Data.Pos.Y=300;
				MainChar.Data.Pos.X=400;
				MainChar.Data.Facing=DOWN;
				MainChar.Data.Frame=1;
			}
			break;
		}
		RequestedAnim=-1;
	}
	switch(SubTutAnim)
	{
	case BRIEFSETUP:
		trigger=0;
		sColor Alpha;
		Alpha.r=255;
		Alpha.g=0;
		Alpha.b=255;
		
		main.SetAlpha(Alpha);
		char tempDir[600];
		strcpy(tempDir,GlobalDir);
		strcat(tempDir,"\\Tutorial");
		if(!SetCurrentDirectory(tempDir))
		{
			main.Error("Missing folder",0,"Could not find Tutorials folder, please make sure that it exists");
			Animation=0;
			SetPersp();
			Section=CHOOSE;
			break;
		}
		if(!main.LoadTexture("sprites.bmp",0) || !main.LoadTexture("tiles.bmp",1) || !main.LoadTexture("Objects.bmp",OBJECTBMPID))
		{
			main.Error("Could not load bmp",0,"Tutorial can not continue without these images.");
			Animation=0;
			SetPersp();
			Section=CHOOSE;
			break;
		}
		iCOORD temp;
		temp.X=50;
		temp.Y=100;
		MainChar.Init(0,temp);	// Set the defaults for Timmy's animations
		MainChar.SetFace(RIGHT);
		MainChar.SetFrameSpeed(16);
		MainChar.SetSpeed(3);
		MainChar.flags=0;
		SubTutAnim++;
		glColor3d(1,1,1);
		LevelMap.ClearData();
		break;
	case BRIEFWALKIN:
		MainChar.DrawCharToScreen();
		if(MainChar.Data.Pos.Y<300)
			MainChar.MoveChar(LevelMap, true);
		else
			RequestedAnim=SubTutAnim+1;
		if(MainChar.Data.Pos.X>400 && MainChar.Data.Facing==RIGHT)
			MainChar.Data.Facing = (MainChar.Data.Facing + 1) % NUM_DIRECTIONS;
		break;
	case BRIEFTEXT:
		MainChar.DrawCharToScreen();
		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"Welcome to Escape From the Funky Factory!  My name is Timmy, and I'll be helping you get used to EFFF today, so let's get started!");
		main.glPrint(0,64,0,"Extra controls used in this tutorial:");
		main.glPrint(0,80,0,"Space skips the current area.");
		main.glPrint(0,96,0,"Z moves back an area.");
		main.glPrint(0,576,1,"Press Enter to continue.");
		main.SetWordWrapping(false);
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			SubTutAnim++;
		}
		break;
	case BRIEFTEXT2:
		MainChar.DrawCharToScreen();
		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"First off, let's get used to the controls of the game.  In a little bit, you will play a level in which all you must do is turn, and walk to the exit.  Sounds simple, right?  Well, it is.  Try some experimenting with me right now by using the arrow keys on your keyboard.");
		main.glPrint(0,80,0,"Note: Try to get used to the controls now, or you might get frustrated later on.");
		main.glPrint(0,112,0,"Controls:");
		main.glPrint(0,128,0,"Up moves me in the direction I'm facing.");
		main.glPrint(0,144,0,"Left and Right turn me respectively.");
		main.glPrint(0,160,0,"Down moves me in the opposite direction, backwards.");
		main.SetWordWrapping(false);

		if(GetTickCount() > lastFrame)		// Checks our frame limiting
		{
			lastFrame = GetTickCount()+15;	// This can be tweaked, but should probably stay where it is

			if(main.keys[VK_UP] && !main.keys[VK_DOWN])						// Do keypresses here and move the character
				MainChar.MoveChar(LevelMap, true);

			if(main.keys[VK_DOWN] && !main.keys[VK_UP])
				MainChar.MoveChar(LevelMap, false, (MainChar.Data.Facing + 2) % NUM_DIRECTIONS);

			if((!main.keys[VK_UP] && !main.keys[VK_DOWN]) || (main.keys[VK_UP] && main.keys[VK_DOWN]))
				MainChar.Data.Frame=1;

			if(main.keys[VK_RIGHT] && !main.keys[VK_LEFT] && turnCount == 0)
				MainChar.Data.Facing = (MainChar.Data.Facing + 1) % NUM_DIRECTIONS;
			else if(main.keys[VK_LEFT] && !main.keys[VK_RIGHT] && turnCount == 0)
				MainChar.Data.Facing = (MainChar.Data.Facing + NUM_DIRECTIONS - 1) % NUM_DIRECTIONS;

			if(!(main.keys[VK_LEFT] || main.keys[VK_RIGHT])) turnCount = 0;
			else turnCount++;

			if(turnCount >= 20) turnCount = 0;
		}
		
		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFTEXT3:
		MainChar.DrawCharToScreen();
		ObsMan.DrawObstacles();
		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"Next, let's look at our very first obstacle, the Goal obstacle.");
		main.glPrint(0,48,0,"In EFFF, there are many obstacles that you will get to see as you are playing, this is the MOST important obstacle of them all, because it allows you to beat the level.  As soon as you step on the Goal obstacle, you win the level!");
		main.SetWordWrapping(false);

		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFLEV1:
		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"This level is extremely easy to beat.  All you have to do is turn left once, and walk straight to the Goal obstacle.");
		main.glPrint(0,48,0,"Hint: Remember, when you press UP, I will walk in the direction I am FACING, not necessarily up.");
		main.SetWordWrapping(false);

		HandleGame();
		
		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFLEV2:
		main.SetWordWrapping(true);
		main.SetEdge(500);
		main.glPrint(100,0,0,"Congratulations!  That wasn't that hard now, was it?  Now, this level tests your ability to navigate through a hallway by turning and moving in different directions.");
		main.glPrint(100,96,0,"Hint: Remember that left and right make me turn.");
		main.SetEdge(EDGEDEFAULT);
		main.SetWordWrapping(false);

		HandleGame();
		
		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFTEXT4:
		MainChar.DrawCharToScreen();
		ObsMan.DrawObstacles();

		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"Now that you should have the hang of walking around in EFFF, let's learn about a few more obstacles. The next obstacle we will look at is the Conveyor Belt.");

		main.glPrint(0,80,0,"Whenever you step on a Conveyor Belt, you will be moved along it, without being able to walk back the other way.");
		main.SetWordWrapping(false);

		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFLEV3:
		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"This level is a prime example of some common uses of Conveyor Belts.  There are a few different ways that you can go, but they all lead to the same place.");
		main.glPrint(0,64,0,"Hint: If you get stuck on a wall while on the Conveyor Belt, just move away from the wall and continue on the Belt, because there's no use in resisting it.");
		main.SetWordWrapping(false);

		HandleGame();
		
		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFTEXT5:
		MainChar.DrawCharToScreen();
		ObsMan.DrawObstacles();

		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"The next obstacle that I will teach you about is the Turn-Table.  Whenever you step on a turntable, you will be turned in the direction that it is turning.");

		main.glPrint(0,80,0,"This can be a little disorienting sometimes, but once you get your timing down, you'll be hopping from Turn-Table to Turn-Table easier than anything!");
		main.SetWordWrapping(false);

		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFLEV4:
		main.SetWordWrapping(true);
		main.glPrint(0,480,0,"This level gets you used to how the Turn-Tables work.  Throughout this level, you must get your timing down so that you can get past them.");
		main.glPrint(0,528,0,"Hint: Time my movement so that as soon as I face the way that you want me to go, hit forward, and I will walk off the Turn-Table.");
		main.SetWordWrapping(false);

		HandleGame();
		
		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFTEXT6:
		MainChar.DrawCharToScreen();
		ObsMan.DrawObstacles();

		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"Congratulations!  You've made it through the basics of EFFF!  Now, however, we're going to start playing some more challenging levels.");

		main.glPrint(0,64,0,"This obstacle is the Pit.  Whenever you happen to fall in a pit, you die.  However, they are very easy to avoid, if you pay attention.");
		main.SetWordWrapping(false);

		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFLEV5:
		main.SetWordWrapping(true);
		main.glPrint(0,464,0,"This level may look a bit intimidating compared to the other levels you played so far, but remember, it's all in the timing!  If you can get your timing down, you can get past all of the Pits and Turn-Tables easily.");

		main.glPrint(0,544,0,"Hint: Be patient when you're on the Turn-Tables, if you aren't, you'll run the risk of falling in to a pit.");

		HandleGame();

		if(MainChar.flags == PLAYER_DEAD)
		{
			if(MainChar.LastObs==7 && !GETBIT(trigger,0))
				trigger=(char)TOGBIT(trigger,0);

			if((MainChar.LastObs==15 || MainChar.LastObs==16  ||
			  (MainChar.LastObs>=18 && MainChar.LastObs<=25) ||
			  MainChar.LastObs==28 || MainChar.LastObs==33   ||
			  MainChar.LastObs==34) && !GETBIT(trigger,1))
				trigger=(char)TOGBIT(trigger,1);

			if(MainChar.LastObs==42 && !GETBIT(trigger,2))
				trigger=(char)TOGBIT(trigger,2);

			MainChar.Data.Pos.X=SaveData.PlayerStart.X;
			MainChar.Data.Pos.Y=SaveData.PlayerStart.Y;
			MainChar.flags=0;
		}

		main.SetEdge(380);
		if(GETBIT(trigger,0))
			main.glPrint(0,0,0,"Remember to get off the Conveyor-Belts before you hit the end.");

		if(GETBIT(trigger,1))
			main.glPrint(0,64,0,"Try and take your time on the Turn-Tables.");

		if(GETBIT(trigger,2))
			main.glPrint(0,112,0,"Always look ahead before jumping on to a Conveyor Belt.");

		main.SetEdge(EDGEDEFAULT);
		main.SetWordWrapping(false);

		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFTEXT7:
		MainChar.DrawCharToScreen();
		ObsMan.DrawObstacles();

		glColor3d(1,1,1);

		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"Lasers are probably one of the easiest obstacles to get around, if you are patient enough.  Usually, lasers are set on a timer, meaning that they will toggle on and off at a set rate.  By having the lasers do this, you just have to figure out the pattern and go as soon as the lasers disappear.");

		main.glPrint(0,128,0,"If you will notice, the lasers themselves are actually quite a small obstacle.  However, a well-placed laser and one miss-step and you'll make me walk right in to one!");
		main.SetWordWrapping(false);

		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFLEV6:
		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"This level is overall quite easy.  The first part of the level has you getting your timing right to walk past the lasers.  The next part takes a little more timing.  And finally, the rest of the level involves either buttons or avoiding the lasers.");

		main.glPrint(0,64,0,"Hint: Just like most other obstacles in EFFF, you'll have to be patient when dealing with the lasers.");
		main.SetWordWrapping(false);

		HandleGame();

		if(MainChar.flags == PLAYER_DEAD)
		{
			MainChar.Data.Pos.X=SaveData.PlayerStart.X;
			MainChar.Data.Pos.Y=SaveData.PlayerStart.Y;
			MainChar.flags=0;
		}

		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFTEXT8:
		MainChar.DrawCharToScreen();
		ObsMan.DrawObstacles();

		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"Now, you probably died a couple times while you were playing that last level, but once you play a few more levels, you should be able to easily breeze past them without any trouble at all.");

		main.glPrint(0,80,0,"The next obstacle I'll teach you about is the Button.  Buttons allow you to turn things on and off and may even allow you to access new areas!  Usually, whenever you see a Button, you should press it.  Also, when you step on a button, all of the obstacles that it affected will turn RED, allowing you to see what was affected much easier.");
		main.SetWordWrapping(false);

		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFLEV7:
		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"In this level you will need to think and use your brains a bit to get past the obstacles.  One of the main things that you should remember is that if you see a button, hit it!");

		main.glPrint(0,64,0,"Hint: If you can't get past an area, try hitting a button and maybe it will allow you to pass.");
		main.SetWordWrapping(false);

		HandleGame();

		if(MainChar.flags == PLAYER_DEAD)
		{
			MainChar.Data.Pos.X=SaveData.PlayerStart.X;
			MainChar.Data.Pos.Y=SaveData.PlayerStart.Y;
			MainChar.flags=0;
		}

		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFTEXT9:
		MainChar.DrawCharToScreen();
		ObsMan.DrawObstacles();

		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"Now I am going to introduce you to yet another obstacle in EFFF: The Hammer.  Hammers in EFFF are mainly used as as another timing obstacle, in that you have to avoid the Hammers and be able to dodge them without being hit.");

		main.glPrint(0,96,0,"Whenever you see a Hammer, you'll want to do your best to not run in to it, because if you do, you die.  To get past a Hammer, be patient and get used to the timing of it.  Once you do, just run past it.");
		main.SetWordWrapping(false);

		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFLEV8:
		main.SetWordWrapping(true);
		main.SetEdge(630);
		main.glPrint(0,0,0, "This level takes the most timing out of any level so far.  You will have to dodge from Hammer to Hammer, making sure that you don't get hit by any of them.");
		main.glPrint(0,64,0,"Hint: Be patient and watch for patterns.");

		HandleGame();

		if(MainChar.flags == PLAYER_DEAD)
		{
			MainChar.Data.Pos.X=SaveData.PlayerStart.X;
			MainChar.Data.Pos.Y=SaveData.PlayerStart.Y;
			MainChar.flags=0;
		}

		main.SetEdge(EDGEDEFAULT);
		main.SetWordWrapping(false);

		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFTEXT10:
		MainChar.DrawCharToScreen();
		ObsMan.DrawObstacles();

		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"The final obstacle I will teach you about is the Crane.  Throughout EFFF, you will use the Cranes MANY times. This is because when you step on a Crane spot, the Crane picks you up and moves you over to a completely new area!");

		main.glPrint(0,96,0,"Usually, whenever you see a Crane, you'll want to step on it so that you can see where it goes, but sometimes they might put you in an area where you don't want to go.");
		main.SetWordWrapping(false);

		main.glPrint(0,576,1,"Press Enter to continue.");
		if(main.keys[VK_RETURN])
		{
			main.keys[VK_RETURN]=false;
			RequestedAnim=SubTutAnim+1;
		}
		break;
	case BRIEFLEV9:
		main.SetWordWrapping(true);
		main.SetEdge(620);
		main.glPrint(0,0,0, "This is probably the hardest level you'll have done yet.  There are many points in the level where you'll have to be careful not to get hit by hammers or fall in Pits.");

		main.glPrint(0,64,0,"Hint: Try to hit every Button you can, even if it looks like you can't get back.");

		HandleGame();

		if(MainChar.flags == PLAYER_DEAD)
		{
			MainChar.Data.Pos.X=SaveData.PlayerStart.X;
			MainChar.Data.Pos.Y=SaveData.PlayerStart.Y;
			MainChar.flags=0;
		}

		main.SetEdge(EDGEDEFAULT);
		main.SetWordWrapping(false);

		if(MainChar.flags == PLAYER_WON)
			RequestedAnim=SubTutAnim+1;
		break;
	case BRIEFEND:
		main.SetWordWrapping(true);
		main.glPrint(0,0,0,"Congratulations!  You've made it through the Basic Training of EFFF!  You should now be ready to try out some of the other levels in the game.  Remember, the key point is patience, without it, you'll only get frustrated.");
		main.SetWordWrapping(false);
		break;
	}
}

void RunCredits()
{
	if(main.keys[VK_ESCAPE])
	{
		SetPersp();

		Section=CHOOSE;
		Choice=0;
		return;
	}
	glLoadIdentity();
	if(main.keys[VK_UP] || main.keys[VK_SPACE])		// This code allows you to speed the credits
		Animation+=5;
	if(main.keys[VK_DOWN] && Animation>0)
		Animation-=10;

	float tempAnim=(float)Animation/2;		// Figure this out once, and it'll save clock cycles

	// All of the code below tests to see where everything is, and if it's on the screen, then
	// it will draw the text/image, but otherwise, it won't even look at it.  This speeds up
	// the draw speed significantly
	if(tempAnim<900)
	{
		glTranslated(400,740-tempAnim,0);

		main.SetTexture(4);

		glColor3d(1,1,1); /* WHITE */

		glBegin(GL_QUADS);
			glTexCoord2f(1,1);		glVertex3d( 220,-140, 0);	// Front
			glTexCoord2f(0,1);		glVertex3d(-220,-140, 0);
			glTexCoord2f(0,0.258f);	glVertex3d(-220, 140, 0);
			glTexCoord2f(1,0.258f);	glVertex3d( 220, 140, 0);
		glEnd();
	}
	//===BASICS======================================================

	int counter=900;
	if(tempAnim>counter-600)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int(counter-tempAnim),1,"A Production of The Developers Alliance");
	}
	counter+=32;
	
//===DEVELOPMENT======================================================
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,0); /* YELLOW */
		main.glPrint(int(counter-tempAnim),0,"All names are ordered alphabetically by last name.");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,0); /* GREEN */
		main.glPrint(int(counter-tempAnim),1,"Developer Team");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),1,"Lead Programmer");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Jeff Verkoeyen");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"Programmers");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Dav Yust");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"Game Engine");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Jeff Verkoeyen");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"Obstacle Engine");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Dav Yust");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"Level Editor");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Jeff Verkoeyen");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"Fmod visualizations");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Jeff Verkoeyen");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"EFFF Logo");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Dav Yust");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"Sprite engines (levels, characters, obstacles)");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Jeff Verkoeyen");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"Level Designer");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Mark Christoforetti");
	}
	counter+=32;
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,1); /* CYAN */
		main.glPrint(int(counter-tempAnim),0,"Graphic Artist");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Michael Romer");
	}
	counter+=48;
	
//===PRODUCTION INFO=================================================
	
	if(tempAnim>counter-600)
	{
		glColor3d(0,0,1); /* BLUE */
		main.glPrint(int(counter-tempAnim),1,"EFFF was conceived and designed by Dav Yust.");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Production started on May 2, 2003.");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Production finished on July 26, 2003.");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"The entire project was completed in less than 3 months.");
	}
	counter+=32;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"EFFF was written using MS Visual C++ 6.");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"EFFF renders its graphics with OpenGL.");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"EFFF plays music thanks to the fmod library.");
	}
	counter+=48;
	
//===THE DEVELOPERS ALLIANCE===================================
	
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,0); /* GREEN */
		main.glPrint(int(counter-tempAnim),0,"THE DEVELOPERS ALLIANCE");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,0); /* GREEN */
		main.glPrint(int(counter-tempAnim),0,"___________________________");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"The Developers Alliance is a group of teenage programmers");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"who combine their efforts to achieve larger and more efficient results.");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"If you are a teen and have programming or artistic skill, you can join TDA.");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"If you don't know how to program, but would like to learn,");
	}
	counter+=16;
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"the Alliance's forums are always open for questions.");
	}
	counter+=48;
	
//===WEBSITE======================================================
	
	
	if(tempAnim>counter-600)
	{
		glColor3d(0,1,0); /* GREEN */
		main.glPrint(int(counter-tempAnim),0,"Website");
	}
	counter+=16;
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"http://www.TheDevelopersAlliance.com/");
	}
	counter+=48;
	
//===COPYRIGHT======================================================
	
	if(tempAnim>counter-600)
	{
		glColor3d(1,1,1); /* WHITE */
		main.glPrint(int(counter-tempAnim),0,"Copyright (c) 2003 The Developer's Alliance");
	}

	int offset=800;
	
//===QUOTES====================================================

	if(tempAnim>offset+1386)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1386)-tempAnim),0,"Quotes");
	}
	
	if(tempAnim>offset+1400)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1400)-tempAnim),0,"Mark: Timmy's walking so fast I can't see him!");
	}
	
	if(tempAnim>offset+1416)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1416)-tempAnim),0,"Dav: FIX IT!");
	}
	
	if(tempAnim>offset+1432)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1432)-tempAnim),0,"Jeff: It works fine for me?!?!?");
	}
	
	if(tempAnim>offset+1448)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1448)-tempAnim),0,"* * *");
	}
	
	if(tempAnim>offset+1464)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1464)-tempAnim),0,"Jeff: Michael, I need those hammer pics.");
	}
	
	if(tempAnim>offset+1480)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1480)-tempAnim),0,"Michael: <extended silence>");
	}
	
	if(tempAnim>offset+1496)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1496)-tempAnim),0,"Mark: Michael, Jeff needs the hammer pics.");
	}
	
	if(tempAnim>offset+1512)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1512)-tempAnim),0,"Michael: <still no reply>");
	}
	
	if(tempAnim>offset+1528)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1528)-tempAnim),0,"Dav: YES! I GOT THE HAMMER PICS!");
	}
	
	if(tempAnim>offset+1544)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1544)-tempAnim),0,"* * *");
	}
	
	if(tempAnim>offset+1560)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1560)-tempAnim),0,"Jeff: Be warned - it's programmer art.");
	}
	
	if(tempAnim>offset+1576)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1576)-tempAnim),0,"* * *");
	}
	
	if(tempAnim>offset+1592)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1592)-tempAnim),0,"Dav: Heck, my art is pretty decent!");
	}
	
	if(tempAnim>offset+1608)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1608)-tempAnim),0,"* * *");
	}
	
	if(tempAnim>offset+1624)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1624)-tempAnim),0,"Dav: You know, I really fit into all the credits sections...");
	}
	
	if(tempAnim>offset+1640)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1640)-tempAnim),0,"* * *");
	}
	
	if(tempAnim>offset+1656)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1656)-tempAnim),0,"Jeff: Oh wait... did I forget to say that?");
	}
	
	if(tempAnim>offset+1672)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1672)-tempAnim),0,"* * *");
	}
	
	if(tempAnim>offset+1688)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1688)-tempAnim),0,"Mark: I was bored, so I designed a bunch of levels on paper.");
	}
	
	if(tempAnim>offset+1704)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1704)-tempAnim),0,"* * *");
	}
	
	if(tempAnim>offset+1720)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1720)-tempAnim),0,"Jeff: 9972?!? We're only 27 lines of code away from 10,000! :(");
	}
	
	if(tempAnim>offset+1736)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1736)-tempAnim),0,"Dav: ummm, Jeff...");
	}
	
	if(tempAnim>offset+1752)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1752)-tempAnim),0,"Jeff: Wait... 28. That's right.");
	}
	
	if(tempAnim>offset+1768)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1768)-tempAnim),0,"Dav: <sigh>");
	}
	
	if(tempAnim>offset+1784)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1784)-tempAnim),0,"* * *");
	}
	
	if(tempAnim>offset+1800)
	{
		glColor3d(1,0,0); /* RED */
		main.glPrint(int((offset+600+1800)-tempAnim),0,"Jeff: Adding a SynchAll call at the end of each toggle switch.");
	}
//===RETURN TO MENU======================================================	
	
	if(tempAnim>offset+600+1832)
	{
		SetPersp();

		Section=CHOOSE;
		Choice=0;
	}
}

void InitSongs()
{
	char currdirect[1024];

	sprintf(currdirect,"%s\\Music\\*.mp3",GlobalDir);

	char lfile[64];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	Songs=new char[64];

	hFind=FindFirstFile(currdirect,&FindFileData);
	strcpy(lfile,FindFileData.cFileName);

	for(unsigned int a=0;a<strlen(lfile);a++)
		Songs[a]=lfile[a];
	Songs[a]='\0';

	NumSongs=1;

	while(1)
	{
		FindNextFile(hFind,&FindFileData);
		
		if(!strcmp(FindFileData.cFileName,lfile))
			break;

		NumSongs++;

		Songs=(char*)realloc(Songs,sizeof(char)*64*NumSongs);

		strcpy(lfile,FindFileData.cFileName);

		for(a=0;a<strlen(lfile);a++)
			Songs[(NumSongs-1)*64+a]=lfile[a];
		Songs[(NumSongs-1)*64+a]='\0';
	}
}

BOOL CALLBACK NameProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
	case WM_INITDIALOG:
		SendMessage(GetDlgItem(hName,IDC_NAME),EM_SETLIMITTEXT,30,NULL);
		ShowCursor(true);
		break;
	case WM_MOVE:
		InvalidateRect(hWnd,NULL,true);
		UpdateWindow(hWnd);
		return true;
		break;
    case WM_CLOSE:
	case WM_DESTROY:
		GetWindowText(GetDlgItem(hWnd,IDC_NAME),ReturnName,31);
		if(ReturnName[0]!='\0')
		{
			EndDialog(hWnd,0);
			hName=NULL;
			ShowCursor(false);
			EnableWindow(main.hWnd,true);
		}
		else
			main.Error("Error",MB_OK,"Please enter your name.");
		return true;
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_OK:
			GetWindowText(GetDlgItem(hWnd,IDC_NAME),ReturnName,31);
			if(ReturnName[0]!='\0')
			{
				EndDialog(hWnd,0);
				hName=NULL;
				EnableWindow(main.hWnd,true);
			}
			else
				main.Error("Error",MB_OK,"Please enter your name.");
			return true;
			break;
		}
		break;
	}
	return FALSE;
}

char* GetName()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SwapBuffers(main.hDC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SwapBuffers(main.hDC);

	DialogBox(main.hInstance,MAKEINTRESOURCE(IDD_NAME),main.hWnd,(DLGPROC)NameProc);

	return ReturnName;
}

void SaveScores()
{
	char FileName[256];
	strcpy(FileName,CurrLevel);
	strcat(FileName,".highs");

	FILE* FileRead;
	FileRead=fopen(FileName,"rb");

	bool JustAdded=false;
	if(!FileRead)
	{
		main.Error("Notice",MB_OK,"Creating High Score file");
		FILE* FileWrite;

		FileWrite=fopen(FileName,"wb");

		if(!FileWrite)
			main.Error("Error",MB_OK,"Something went wrong with opening the High Score file...");
		else
		{
			GetName();
			unsigned long test=LevelMap.GetTime();

			for(int a=0;a<32;a++)
			{
				if(ReturnName[a]=='\0')
					break;
				ReturnName[a]-=50;
			}

			for(a=0;a<5;a++)
			{
				fwrite(&test,sizeof(unsigned long),1,FileWrite);
				fwrite(&ReturnName,sizeof(char),32,FileWrite);
			}
			fclose(FileWrite);
			JustAdded=true;
			FileRead=fopen(FileName,"rb");
		}
	}

	unsigned long scores[MAXHIGHS];
	char names[32*MAXHIGHS];

	for(int a=0;a<MAXHIGHS;a++)
	{
		fread(&scores[a],sizeof(unsigned long),1,FileRead);
		char temp[32];
		fread(&temp,sizeof(char),32,FileRead);
		for(int b=a*32;b<a*32+32;b++)
		{
			if(temp[b-a*32]=='\0')
				break;
			names[b]=temp[b-a*32]+50;
			temp[b-a*32]+=50;
		}
		names[b]='\0';
	}
	fclose(FileRead);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	SwapBuffers(main.hDC);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for(a=0;a<MAXHIGHS;a++)
	{
		char Name[32];
		for(int b=0;b<32;b++)
		{
			if(names[b+a*32]=='\0')
				break;
			Name[b]=names[b+a*32];
		}
		Name[b]='\0';

		main.glPrint(a*16,0,"%s  -  %s",Name,LevelMap.ConvertTime(scores[a]));
	}

	main.glPrint(a*16,0,"Your Time: %s",LevelMap.ConvertTime(LevelMap.GetTime()));

	SwapBuffers(main.hDC);

	MSG msg;

	while(!main.keys[VK_ESCAPE] && !main.keys[VK_RETURN] && !main.keys[VK_SPACE])
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	main.keys[VK_ESCAPE]=main.keys[VK_RETURN]=main.keys[VK_SPACE]=false;

	if(!JustAdded)
	{
		int BetterThan=-1;
		for(a=0;a<MAXHIGHS;a++)
		{
			if(LevelMap.GetTime()<scores[a])
			{
				BetterThan=a;
				break;
			}
		}

		if(BetterThan!=-1)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			SwapBuffers(main.hDC);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			GetName();

			for(a=4;a>BetterThan;a--)
			{
				scores[a]=scores[a-1];
				for(int b=a*32;b<a*32+32;b++)
					names[b]=names[b-32];
			}
			scores[a]=LevelMap.GetTime();
			for(a=32*BetterThan;a<32*BetterThan+32;a++)
				names[a]=ReturnName[a-32*BetterThan];
			
			for(a=0;a<MAXHIGHS;a++)
			{
				char Name[32];
				for(int b=0;b<32;b++)
				{
					if(names[b+a*32]=='\0')
						break;
					Name[b]=names[b+a*32];
				}
				Name[b]='\0';

				main.glPrint(a*16,0,"%s  -  %s",Name,LevelMap.ConvertTime(scores[a]));
			}
			main.glPrint(300,0,"Congratulations!  You got a high score!");

			FILE* FileWrite;

			FileWrite=fopen(FileName,"wb");

			if(!FileWrite)
				main.Error("Error",MB_OK,"Something went wrong with opening the High Score file...");
			else
			{
				for(int a=0;a<MAXHIGHS;a++)
				{
					for(int b=a*32;b<a*32+32;b++)
					{
						if(names[b]=='\0')
							break;
						names[b]-=50;
					}
				}

				for(a=0;a<MAXHIGHS;a++)
				{
					fwrite(&scores[a],sizeof(unsigned long),1,FileWrite);
					char Name[32];
					for(int b=0;b<32;b++)
					{
						if(names[b+a*32]=='\0')
							break;
						Name[b]=names[b+a*32];
					}
					Name[b]='\0';
					fwrite(&Name,sizeof(char),32,FileWrite);
				}
				fclose(FileWrite);
			}

			SwapBuffers(main.hDC);

			MSG msg;

			while(!main.keys[VK_ESCAPE] && !main.keys[VK_RETURN] && !main.keys[VK_SPACE])
			{
				if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
			main.keys[VK_ESCAPE]=main.keys[VK_RETURN]=main.keys[VK_SPACE]=false;
		}
	}
}