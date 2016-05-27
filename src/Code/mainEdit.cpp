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

#include "common.h"
#include "OpenGL.h"
OpenGL main;
#include "Characters.h"
#include "Levels.h"
#include "Objects.h"
#include "FPS.h"
#include "obstacle.h"
#include "LevelEditor.h"
class CFmod					// Sorry, I know this is ugly, but I needed to create this class so
{							// that the level editor wouldn't HAVE to have fmod to run it.
public:
	void PlaySong(void);
	void Volume(int Volume);
	signed char IsPlaying(void);
};
void CFmod::PlaySong(void)
{
}
void CFmod::Volume(int Volume)
{
}
signed char CFmod::IsPlaying(void)
{
	return 0;
}
CFmod SFX[NUMSFX];

int SCREENX=800,SCREENY=600;

Mouse mouse;

LevelEditor Edit;

bool done=false;

char GlobalDir[512];

FPS fps;

void SetOrtho()
{
	glViewport(0,0,SCREENX,SCREENY);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(0.0f,800,600,0.0f,-10.0f,20.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void SetPersp()
{
	glViewport(0,0,SCREENX,SCREENY);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f,(GLfloat)800/(GLfloat)600,0.1f,800.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool DrawScene()
{
	if(fps.Get()-45>0)
		Sleep(unsigned long(fps.Get()-45));
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if(!Edit.RunLevelEditor())
		return false;
	return true;
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

		case WM_MOUSEMOVE:
		{
			mouse.Pos.X=LOWORD(lParam);
			mouse.Pos.Y=HIWORD(lParam);
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			mouse.Left=true;
			return 0;
		}

		case WM_LBUTTONUP:
		{
			mouse.Left=false;
			mouse.hLeft=false;
			return 0;
		}

		case WM_RBUTTONDOWN:
		{
			mouse.Right=true;
			return 0;
		}

		case WM_RBUTTONUP:
		{
			mouse.Right=false;
			mouse.hRight=false;
			return 0;
		}

		case WM_MBUTTONDOWN:
		{
			mouse.Middle=true;
			return 0;
		}

		case WM_MBUTTONUP:
		{
			mouse.Middle=false;
			mouse.hMiddle=false;
			return 0;
		}
	}

	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

int WINAPI WinMain(	HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					LPSTR		lpCmdLine,
					int			nCmdShow)
{
	MSG		msg;

	bool FULL=true;

	if(!strcmp(lpCmdLine,"-win"))
		FULL=false;

	if(!main.CreateGLWindow("Funky Factory",SCREENX,SCREENY,16,FULL,0))
		return 0;				// Create our window using the OpenGL.h header's create window command
	
	GetCurrentDirectory(511,GlobalDir);

	SetOrtho();

	main.Texture2D(TRUE);		// Enable textures

	sColor Alpha;				// Make a temporary Alpha color channel

	main.SetFiltering(false);	// Turn off the filtering (crisper pictures)

	Alpha.r=0;					// Set the color to black for the font
	Alpha.g=0;
	Alpha.b=0;

	main.SetAlpha(Alpha);		// Set the Alpha color

	main.SetFiltering(true);	// Turn on the filtering (smoother pictures)
	if(!main.LoadFont("font.bmp"))	// Load the font with black being transparent
		return 0;
	main.BuildFont();			// Create our font
	main.SetFiltering(false);	// Turn off the filtering (crisper pictures)

	main.CullFace(GL_BACK);		// Set the face to be culled
	main.Culling(TRUE);			// Turn on culling

	Alpha.r=0;				// Set the background color
	Alpha.g=0;
	Alpha.b=0;
	main.ClearColor(Alpha);		// Set the background color to white

	if(!Edit.InitEditor(&mouse))
		return 0;

	Edit.SetMode(true);

	fps.Set(1,100);				// Init the FPS counter

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
	return(msg.wParam);
}