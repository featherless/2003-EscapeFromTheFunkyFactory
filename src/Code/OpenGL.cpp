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

void OpenGL::SetFiltering(bool on)
{
	Filter=on;
}

void OpenGL::SetAlpha(sColor COLOR)
{
	AlphaColor=COLOR;
}

void OpenGL::SetTexture(unsigned int slot)
{
	glBindTexture(GL_TEXTURE_2D, texture[slot]);
}

void OpenGL::UpdateKeys()
{
	memcpy(lkeys,keys,sizeof(bool)*256);
}

void OpenGL::BuildFont()									// Build Our Font Display List
{
	base=glGenLists(256);									// Creating 256 Display Lists
	glBindTexture(GL_TEXTURE_2D, tFont);					// Select Our Font Texture

	for(int loop1=0;loop1<256;loop1++)						// Loop Through All 256 Lists
	{
		float cx=float(loop1%16)/16.0f;						// X Position Of Current Character
		float cy=float(loop1/16)/16.0f;						// Y Position Of Current Character

		glNewList(base+loop1,GL_COMPILE);					// Start Building A List
			glBegin(GL_QUADS);								// Use A Quad For Each Character
				glTexCoord2f(cx,1-cy-0.0625f);				// Texture Coord(Bottom Left)
				glVertex2i(0,16);							// Vertex Coord(Bottom Left)
				glTexCoord2f(cx+0.0625f,1-cy-0.0625f);		// Texture Coord(Bottom Right)
				glVertex2i(16,16);							// Vertex Coord(Bottom Right)
				glTexCoord2f(cx+0.0625f,1-cy);				// Texture Coord(Top Right)
				glVertex2i(16,0);							// Vertex Coord(Top Right)
				glTexCoord2f(cx,1-cy);						// Texture Coord(Top Left)
				glVertex2i(0,0);							// Vertex Coord(Top Left)
			glEnd();										// Done Building Our Quad(Character)
			glTranslated(Width,0,0);							// Move To The Right Of The Character
		glEndList();										// Done Building The Display List
	}														// Loop Until All 256 Are Built
}

void OpenGL::KillFont()										// Delete The Font From Memory
{
	glDeleteLists(base,256);								// Delete All 256 Display Lists
}

void OpenGL::SetTextSpacing(int Spacing)
{
	Width=Spacing;
}

void OpenGL::SetEdge(int Edge)
{
	this->Edge=Edge;
}

void OpenGL::SetWordWrapping(bool On)
{
	WordWrap=On;
}

void OpenGL::glPrint(GLint y, int set, const char *fmt, ...)	// Where The Printing Happens
{
	glEnable(GL_BLEND);
	if(y>580)
		y=580;
	int reset=0;
	glFrontFace(GL_CCW);
	if(fmode!=GL_FILL)
	{
		reset=fmode;
		Mode(GL_FRONT,GL_FILL);
	}
	glBindTexture(GL_TEXTURE_2D, tFont);
	char		text[1024];									// Holds Our String
	va_list		ap;											// Pointer To List Of Arguments

	if(fmt == NULL)											// If There's No Text
		return;												// Do Nothing

	va_start(ap, fmt);										// Parses The String For Variables
	    vsprintf(text, fmt, ap);							// And Converts Symbols To Actual Numbers
	va_end(ap);												// Results Are Stored In Text

	if(set>1)												// Did User Choose An Invalid Character Set?
		set=1;												// If So, Select Set 1(Italic)

	glMatrixMode(GL_PROJECTION);							// Select The Projection Matrix

	glPushMatrix();
	
	glLoadIdentity();										// Reset The Projection Matrix
	glOrtho(0.0f,800,600,0.0f,-20.0f,20.0f);

	glMatrixMode(GL_MODELVIEW);								// Select The Modelview Matrix

	glLoadIdentity();										// Reset The Modelview Matrix
	glTranslated(400-((strlen(text)/2)*Width),y,20);	// Position The Text(0,0 - Top Left)
	glListBase(base-32+(128*set));							// Choose The Font Set(0 or 1)

	glCallLists(strlen(text),GL_UNSIGNED_BYTE, text);	// Write The Text To The Screen

	glMatrixMode(GL_PROJECTION);							// Select The Projection Matrix
	glLoadIdentity();										// Reset The Projection Matrix

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);								// Select The Modelview Matrix

	if(reset)
		Mode(GL_FRONT,reset);
	glDisable(GL_BLEND);
}

void OpenGL::glPrint(GLint x, GLint y, int set, const char *fmt, ...)	// Where The Printing Happens
{
	glEnable(GL_BLEND);
	if(x>780)
		x=780;
	if(y>580)
		y=580;
	int reset=0;
	glFrontFace(GL_CCW);
	if(fmode!=GL_FILL)
	{
		reset=fmode;
		Mode(GL_FRONT,GL_FILL);
	}
	glBindTexture(GL_TEXTURE_2D, tFont);
	char		text[1024];									// Holds Our String
	va_list		ap;											// Pointer To List Of Arguments

	if(fmt == NULL)											// If There's No Text
		return;												// Do Nothing

	va_start(ap, fmt);										// Parses The String For Variables
	    vsprintf(text, fmt, ap);							// And Converts Symbols To Actual Numbers
	va_end(ap);												// Results Are Stored In Text

	if(set>1)												// Did User Choose An Invalid Character Set?
		set=1;												// If So, Select Set 1(Italic)

	glMatrixMode(GL_PROJECTION);							// Select The Projection Matrix

	glPushMatrix();
	
	glLoadIdentity();										// Reset The Projection Matrix
	glOrtho(0.0f,800,600,0.0f,-20.0f,20.0f);

	glMatrixMode(GL_MODELVIEW);								// Select The Modelview Matrix

	glLoadIdentity();										// Reset The Modelview Matrix
	glTranslated(x,y,20);									// Position The Text(0,0 - Top Left)
	glListBase(base-32+(128*set));							// Choose The Font Set(0 or 1)

	int counter=0;
	while(strlen(text)>0)
	{
		int width=strlen(text)*Width;

		if(width+x>=Edge && WordWrap)
		{
			counter++;
			unsigned int max=(Edge-x)/Width-1;

			char *temp=new char[max+1];
			
			for(unsigned int a=max-1;a>0;a--)
			{
				if(text[a]==' ')
				{
					max=a;
					break;
				}
			}
			
			for(a=0;a<max;a++)
				temp[a]=text[a];
			temp[a]='\0';

			for(a=max+1;a<strlen(text);a++)
				text[a-max-1]=text[a];
			text[a-max-1]='\0';
			
			glCallLists(strlen(temp),GL_UNSIGNED_BYTE, temp);	// Write The Text To The Screen
			glLoadIdentity();
			glTranslated(x,y+counter*16,20);

		}
		else
		{
			glCallLists(strlen(text),GL_UNSIGNED_BYTE, text);	// Write The Text To The Screen
			break;
		}
	}

	glMatrixMode(GL_PROJECTION);							// Select The Projection Matrix
	glLoadIdentity();										// Reset The Projection Matrix

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);								// Select The Modelview Matrix

	if(reset)
		Mode(GL_FRONT,reset);
	glDisable(GL_BLEND);
}

void OpenGL::glPrint(GLint x, GLint y, GLfloat ScaleX, GLfloat ScaleY, int set, const char *fmt, ...)	// Where The Printing Happens
{
	glEnable(GL_BLEND);
	if(x>780)
		x=780;
	if(y>580)
		y=580;
	int reset=0;
	glFrontFace(GL_CCW);
	if(fmode!=GL_FILL)
	{
		reset=fmode;
		Mode(GL_FRONT,GL_FILL);
	}
	glBindTexture(GL_TEXTURE_2D, tFont);
	char		text[1024];								// Holds Our String
	va_list		ap;										// Pointer To List Of Arguments

	if(fmt == NULL)										// If There's No Text
		return;											// Do Nothing

	va_start(ap, fmt);									// Parses The String For Variables
	    vsprintf(text, fmt, ap);						// And Converts Symbols To Actual Numbers
	va_end(ap);											// Results Are Stored In Text

	if(set>1)											// Did User Choose An Invalid Character Set?
		set=1;											// If So, Select Set 1(Italic)

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix

	glPushMatrix();
	
	glLoadIdentity();									// Reset The Projection Matrix
	glOrtho(0.0f,800,600,0.0f,-20.0f,20.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix

	glLoadIdentity();									// Reset The Modelview Matrix
	glTranslated(x,y,20);								// Position The Text(0,0 - Top Left)
	glScalef(ScaleX,ScaleY,1.0f);
	glListBase(base-32+(128*set));						// Choose The Font Set(0 or 1)

	int counter=0;
	while(strlen(text)>0)
	{
		float width=strlen(text)*(Width*ScaleX);

		if(width+x>=Edge && WordWrap)
		{
			counter++;
			float max=(Edge-x)/(Width*ScaleX)-1;

			char *temp=new char[(int)max+1];

			for(unsigned int a=(unsigned int)max-1;a>0;a--)
			{
				if(text[a]==' ')
				{
					max=(float)a;
					break;
				}
			}
			
			for(a=0;a<max;a++)
				temp[a]=text[a];
			temp[a]='\0';

			for(a=(unsigned int)max+1;a<strlen(text);a++)
				text[a-(int)max-1]=text[a];
			text[a-(int)max-1]='\0';
			
			glCallLists(strlen(temp),GL_UNSIGNED_BYTE, temp);	// Write The Text To The Screen
			glLoadIdentity();
			glTranslated(x,y+counter*(16*ScaleY),20);
			glScalef(ScaleX,ScaleY,1.0f);
		}
		else
		{
			glCallLists(strlen(text),GL_UNSIGNED_BYTE, text);	// Write The Text To The Screen
			break;
		}
	}

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix

	if(reset)
		Mode(GL_FRONT,reset);
	glDisable(GL_BLEND);
}

void OpenGL::Mode(int face, int mode)
{
	glPolygonMode(face,mode);
	if(face==GL_FRONT)
		fmode=mode;
	else if(face==GL_BACK)
		bmode=mode;
}

void OpenGL::DrawCubet(float size,float u,float v,float ux, float uy)
{
	glBegin(GL_QUADS);
		glTexCoord2f(u, v);glVertex3f(size, size, size);		// Front
		glTexCoord2f(ux, v);glVertex3f(-size, size, size);
		glTexCoord2f(ux, uy);glVertex3f(-size,-size, size);
		glTexCoord2f(u, uy);glVertex3f(size,-size, size);

		glTexCoord2f(u, v);glVertex3f(-size, size, size);		// Left
		glTexCoord2f(ux, v);glVertex3f(-size, size,-size);
		glTexCoord2f(ux, uy);glVertex3f(-size,-size,-size);
		glTexCoord2f(u, uy);glVertex3f(-size,-size, size);

		glTexCoord2f(u, v);glVertex3f(size, size,-size);		// Right
		glTexCoord2f(ux, v);glVertex3f(size, size, size);
		glTexCoord2f(ux, uy);glVertex3f(size,-size, size);
		glTexCoord2f(u, uy);glVertex3f(size,-size,-size);

		glTexCoord2f(u, v);glVertex3f(-size, size,-size);		// Back
		glTexCoord2f(ux, v);glVertex3f(size, size,-size);
		glTexCoord2f(ux, uy);glVertex3f(size,-size,-size);
		glTexCoord2f(u, uy);glVertex3f(-size,-size,-size);

		glTexCoord2f(u, v);glVertex3f(size, size,-size);		// Top
		glTexCoord2f(ux, v);glVertex3f(-size, size,-size);
		glTexCoord2f(ux, uy);glVertex3f(-size, size, size);
		glTexCoord2f(u, uy);glVertex3f(size, size, size);

		glTexCoord2f(u, v);glVertex3f(-size,-size,-size);		// Bottom
		glTexCoord2f(ux, v);glVertex3f(size,-size,-size);
		glTexCoord2f(ux, uy);glVertex3f(size,-size, size);
		glTexCoord2f(u, uy);glVertex3f(-size,-size, size);
	glEnd();
}

void OpenGL::DrawCube(float size)
{
	unsigned char on=glIsEnabled(GL_TEXTURE_2D);

	glDisable(GL_TEXTURE_2D);	

	glBegin(GL_QUADS);
		glVertex3f(size, size, size);		// Front
		glVertex3f(-size, size, size);
		glVertex3f(-size,-size, size);
		glVertex3f(size,-size, size);

		glVertex3f(-size, size, size);		// Left
		glVertex3f(-size, size,-size);
		glVertex3f(-size,-size,-size);
		glVertex3f(-size,-size, size);

		glVertex3f(size, size,-size);		// Right
		glVertex3f(size, size, size);
		glVertex3f(size,-size, size);
		glVertex3f(size,-size,-size);

		glVertex3f(-size, size,-size);		// Back
		glVertex3f(size, size,-size);
		glVertex3f(size,-size,-size);
		glVertex3f(-size,-size,-size);

		glVertex3f(size, size,-size);		// Top
		glVertex3f(-size, size,-size);
		glVertex3f(-size, size, size);
		glVertex3f(size, size, size);

		glVertex3f(-size,-size,-size);		// Bottom
		glVertex3f(size,-size,-size);
		glVertex3f(size,-size, size);
		glVertex3f(-size,-size, size);
	glEnd();

	if(on)
		glEnable(GL_TEXTURE_2D);
}

void OpenGL::SetNumTexts(int nTexts)
{
	if(!textset)
	{
		textset=true;
		texture=new GLuint[nTexts];
	}
	else
		texture=(GLuint*)realloc(texture,sizeof(GLuint)*nTexts);
	NUMTEXTS=nTexts;
}

AUX_RGBImageRec* OpenGL::LoadBMP(char *Filename)					// Loads A Bitmap Image
{
	FILE *File=NULL;							// File Handle

	if(!Filename)
		return NULL;

	File=fopen(Filename,"r");

	if(File)								// Does The File Exist?
	{
		fclose(File);							// Close The Handle
		return auxDIBImageLoad(Filename);				// Load The Bitmap And Return A Pointer
	}

	return NULL;								// If Load Failed Return NULL
}

bool OpenGL::LoadFont(char* filename)
{
	bool Status=FALSE;
	
	AUX_RGBImageRec *TextureImage[1];

	memset(TextureImage,0,sizeof(void *)*1);

	if(TextureImage[0]=LoadBMP(filename))
	{
		Status=TRUE;
		glGenTextures(1, &tFont);

		glBindTexture(GL_TEXTURE_2D, tFont);
		
		int texture_size = TextureImage[0]->sizeX*TextureImage[0]->sizeY;
		GLubyte* RGBA_pixels = new GLubyte[TextureImage[0]->sizeX*TextureImage[0]->sizeY*4];
		for(int i=0;i<texture_size;i++)
		{
			RGBA_pixels[i*4] = TextureImage[0]->data[i*3];
			RGBA_pixels[i*4+1] = TextureImage[0]->data[i*3+1];
			RGBA_pixels[i*4+2] = TextureImage[0]->data[i*3+2];
			
			if(TextureImage[0]->data[i*3]==AlphaColor.r && TextureImage[0]->data[i*3+1]==AlphaColor.g && TextureImage[0]->data[i*3+2]==AlphaColor.b)
				RGBA_pixels[i*4+3]=0;
			else
				RGBA_pixels[i*4+3]=255;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, RGBA_pixels);
		
		if(!Filter)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		}

		if(TextureImage[0]->data)
			free(TextureImage[0]->data);
		
		free(TextureImage[0]);						// Free The Image Structure
	}
	else
		Error("ERROR",MB_OK|MB_ICONEXCLAMATION,"Can't find %s",filename);

	return Status;								// Return The Status
}

bool OpenGL::LoadTexture(char* filename, int space)
{
	bool Status=FALSE;
	
	AUX_RGBImageRec *TextureImage[1];

	memset(TextureImage,0,sizeof(void *)*1);

	if(TextureImage[0]=LoadBMP(filename))
	{
		Status=TRUE;
		glGenTextures(1, &texture[space]);

		glBindTexture(GL_TEXTURE_2D, texture[space]);

		int texture_size = TextureImage[0]->sizeX*TextureImage[0]->sizeY;
		GLubyte* RGBA_pixels = new GLubyte[TextureImage[0]->sizeX*TextureImage[0]->sizeY*4];
		for(int i=0;i<texture_size;i++)
		{
			RGBA_pixels[i*4] = TextureImage[0]->data[i*3];
			RGBA_pixels[i*4+1] = TextureImage[0]->data[i*3+1];
			RGBA_pixels[i*4+2] = TextureImage[0]->data[i*3+2];
			
			if(TextureImage[0]->data[i*3]==AlphaColor.r && TextureImage[0]->data[i*3+1]==AlphaColor.g && TextureImage[0]->data[i*3+2]==AlphaColor.b)
				RGBA_pixels[i*4+3]=0;
			else
				RGBA_pixels[i*4+3]=255;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TextureImage[0]->sizeX, TextureImage[0]->sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, RGBA_pixels);
		
		if(!Filter)
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		}
		else
		{
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		}
		
		if(TextureImage[0]->data)
			free(TextureImage[0]->data);
		
		free(TextureImage[0]);						// Free The Image Structure
	}
	else
		Error("ERROR",MB_OK|MB_ICONEXCLAMATION,"Can't find %s",filename);

	return Status;								// Return The Status
}

void OpenGL::Texture2D(bool bTexture)
{
	if(bTexture)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

void OpenGL::CullFace(int face)
{
	glCullFace(face);
}

void OpenGL::Culling(bool cull)
{
	if(cull)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void OpenGL::ClearColor(sColor color)
{
	glClearColor(color.r,color.g,color.b,color.a);
}

void OpenGL::Cursor(bool cursor)
{
	ShowCursor(cursor);
}

void OpenGL::KillGLWindow()										// Properly Kill The Window
{
	if(gfullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if(hRC)											// Do We Have A Rendering Context?
	{
		if(!wglMakeCurrent(NULL,NULL))
			MessageBox(hWnd,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);

		if(!wglDeleteContext(hRC))
			MessageBox(hWnd,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hRC=NULL;										// Set RC To NULL
	}

	if(hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(hWnd,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if(hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(hWnd,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if(!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(hWnd,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

void OpenGL::ResizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
{/*
	if(!height)
		height=1;

	glViewport(0,0,width,height);

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	glOrtho(0.0f,800,600,0.0f,-10.0f,10.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix*/

	// Only allow the size to be modified INGAME
}

bool OpenGL::ChangeRes(int width, int height, int bits, bool fullscreen)
{
	RECT		WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)height;

	DWORD		dwExStyle;
	DWORD		dwStyle;


	if(fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			char temp[1024];
			sprintf(temp,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?\n%dx%d",width,height);
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if(MessageBox(NULL,temp,"ERROR",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen=FALSE
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if(fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	return true;
}

bool OpenGL::CreateGLWindow(char* title, int width, int height, int bits, bool fullscreen, unsigned int ICON, bool cursor)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASSEX	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	gfullscreen=fullscreen;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	
	wc.cbSize		= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		=(WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(ICON));			// Load The Default Icon
	wc.hIconSm			= LoadIcon(hInstance, MAKEINTRESOURCE(105));			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name
/*
	wClass.style			= CS_HREDRAW | CS_VREDRAW;
	wClass.lpfnWndProc		= WndProc;
	wClass.cbClsExtra		= 0;
	wClass.cbWndExtra		= 0;
	wClass.hInstance		= this->hInstance;
	wClass.hIcon			= hIcon;
	wClass.hIconSm			= hIconSmall;
	wClass.hCursor			= hCursor;
	wClass.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wClass.lpszMenuName		= NULL;
	wClass.lpszClassName	= "Win32";*/
	if(!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if(fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			char temp[1024];
			sprintf(temp,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?\n%dx%d",width,height);
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if(MessageBox(NULL,temp,"ERROR",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen=FALSE
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	ShowCursor(cursor);

	if(fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if(!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer(Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if(!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.\r\nFor this game to work, you must have OpenGL installed on your computer.\r\nIf you aren't sure if you have it, please visit this site and download OpenGL from there:\r\nhttp://webdeploy.glsetup.com/webdeploy.121/glsetup.exe","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.\r\nFor this game to work, you must have OpenGL installed on your computer.\r\nIf you aren't sure if you have it, please visit this site and download OpenGL from there:\r\nhttp://webdeploy.glsetup.com/webdeploy.121/glsetup.exe","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat\r\nFor this game to work, you must have OpenGL installed on your computer.\r\nIf you aren't sure if you have it, please visit this site and download OpenGL from there:\r\nhttp://webdeploy.glsetup.com/webdeploy.121/glsetup.exe.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.\r\nFor this game to work, you must have OpenGL installed on your computer.\r\nIf you aren't sure if you have it, please visit this site and download OpenGL from there:\r\nhttp://webdeploy.glsetup.com/webdeploy.121/glsetup.exe","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.\r\nFor this game to work, you must have OpenGL installed on your computer.\r\nIf you aren't sure if you have it, please visit this site and download OpenGL from there:\r\nhttp://webdeploy.glsetup.com/webdeploy.121/glsetup.exe","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ResizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if(!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(hWnd,"Initialization Failed.\r\nFor this game to work, you must have OpenGL installed on your computer.\r\nIf you aren't sure if you have it, please visit this site and download OpenGL from there:\r\nhttp://webdeploy.glsetup.com/webdeploy.121/glsetup.exe","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

bool OpenGL::InitGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0.0);
	
	PROC procSwapInterval = wglGetProcAddress("wglSwapIntervalEXT"); 
	
	// Does the extension exist? 
	if (procSwapInterval) 
	{ 
		// Create a function pointer to the extension 
		SwapProc pS = (SwapProc)procSwapInterval; 
		
		// Disable vertical sync 
		pS(0); 
	}
	return TRUE;
}

void OpenGL::Error(char* title, UINT uType, const char* fmt, ...)
{
	ShowCursor(TRUE);
	char buff[1024];
	
	va_list		ap;

	if(fmt == NULL)
		return;

	va_start(ap, fmt);
	    vsprintf(buff, fmt, ap);
	va_end(ap);

	memset(keys,0,sizeof(bool)*256);

	char temp[512];
	GetCurrentDirectory(511,temp);

	sprintf(buff,"%s\nWorking Directory: %s",buff,temp);

	MessageBox(hWnd,buff,title,uType);
	ShowCursor(FALSE);
}