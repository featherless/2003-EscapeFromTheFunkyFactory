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

#ifndef __OPENGL_H__
#define __OPENGL_H__

//
// Name: OpenGL.h
//
// Version: 1.3
//
// Created by: Jeff Verkoeyen and Josh Kasten
//
// This header file is for public use and will NOT be
// sold or traded for money in any way.  This file is
// completely free.
//
// If you make any changes/aditions to this header file,
// please contact Jeff at bloodright@msn.com so that he
// can add it to the real version.
//
// This header file is aimed at beginners and will
// hopefully grow with time.  Any input is welcome!
//
// Versions:
//		v1.2 April 2, 2003
//			Fixed some bugs and added new features.
//		v1.3 July 10, 2003
//			Added auto-word-wrapping to glprint.
//
// For the most updated version, visit the site at:
//
// http://ivgda.skylockgames.com
//
// ©2003 IVGDA

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <math.h.>

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLAux.lib")
#pragma comment(lib, "GLu32.lib")

typedef BOOL (WINAPI *SwapProc)(int);

#define EDGEDEFAULT	800

typedef struct ColorDef
{
	float r;	//Red
	float g;	//Green
	float b;	//Blue
	float a;	//Alpha
} sColor;

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class OpenGL
{
private:
	GLuint	base;
	int Width,Edge;
	int NUMTEXTS;
	bool WordWrap;
public:
	HDC			hDC;
	HGLRC		hRC;
	HWND		hWnd;
	HINSTANCE	hInstance;

	bool	active;
	bool	keys[256],lkeys[256];
	bool	gfullscreen;

	bool textset;

	bool Filter;

	GLuint	tFont;
	int fmode,bmode;

	GLuint*	texture;
	sColor AlphaColor;

	OpenGL()
	{
		WordWrap=false;
		Edge=800;
		Width=11;
		textset=false;
		active=true;
		gfullscreen=false;
		Filter=false;
		hDC=NULL;
		hRC=NULL;
		hWnd=NULL;
		NUMTEXTS=3;
		fmode=GL_FILL;
		bmode=GL_FILL;
		AlphaColor.r=255;
		AlphaColor.g=0;
		AlphaColor.b=255;
		AlphaColor.a=0;
	};

	~OpenGL()
	{
		if(tFont)
			KillFont();
		if(texture)
			delete texture;
	};
	AUX_RGBImageRec* LoadBMP(char* Filename);

	bool CreateGLWindow(char* title, int width=800, int height=600, int bits=16, bool fullscreen=false, unsigned int ICON=0, bool cursor=false);
	bool ChangeRes(int width, int height, int bits, bool fullscreen);	// Changes the screen resolution
	bool InitGL();					// Sets up the GL Window

	bool LoadTexture(char* filename, int space);	// Filename, slot
	void SetTexture(unsigned int slot);	//Enter the slot of the picture you want to set to
	void Cursor(bool cursor);		// TRUE or FALSE
	void ClearColor(sColor color);	// Takes an sColor variable
	void Culling(bool cull);		// TRUE or FALSE
	void CullFace(int face);		// GL_BACK or GL_FRONT
	void Texture2D(bool bTexture);	// TRUE or FALSE
	void SetNumTexts(int nTexts);	// Number of Textures
	void DrawCube(float size);		// Size
	void DrawCubet(float size,float u,float v,float ux, float uy);		// Size and texture width/height and x/y of text
	void Mode(int face, int mode);	// GL_FRONT or GL_BACK, GL_FILL or GL_LINE or GL_POINT
	void UpdateKeys();				// Updates the keypresses
	void Error(char* title, UINT uType, const char* fmt, ...); // Title, type, Text, extra vars
	void SetAlpha(sColor COLOR);	// Sets the alpha color, used in making bmps transparent
	void SetFiltering(bool on);		// Sets the filtering on or off

	bool LoadFont(char* filename);	// Loads a font texture
	void BuildFont();		//Builds the font
	void KillFont();
	void glPrint(GLint x, GLint y, int set, const char *fmt, ...);
	void glPrint(GLint x, GLint y, GLfloat ScaleX, GLfloat ScaleY, int set, const char *fmt, ...);	// Overloaded Print function
	void glPrint(GLint y, int set, const char *fmt, ...);	// Centers the printed text
	void SetTextSpacing(int Spacing);	// Width of each character
	void SetEdge(int Edge);			// Edge where the text wraps around
	void SetWordWrapping(bool On);	// Enables/disables word-wrapping

	void ResizeGLScene(GLsizei width, GLsizei height);
	void KillGLWindow();
};

#endif