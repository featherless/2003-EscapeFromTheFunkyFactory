#include "Win32Window.h"

void Win32Window::InitWindow(Logger *Log)
{
	this->Log=Log;

	NumMenus=0;

	hIcon=LoadIcon(NULL, IDI_APPLICATION);
	hIconSmall=LoadIcon(NULL, IDI_APPLICATION);
	hCursor=LoadCursor(NULL, IDC_ARROW);
}

void Win32Window::UpdateWindowDetails(int Value,WPARAM wParam,LPARAM lParam)
{
	switch(Value)
	{
	case WM_SIZE:
		WinSize.X=LOWORD(lParam);
		WinSize.Y=HIWORD(lParam);
		break;
	case WM_MOVE:
		WinCoord.X=LOWORD(lParam);
		WinCoord.Y=HIWORD(lParam);
		break;
	case WM_MOUSEMOVE:
		Mouse.X=LOWORD(lParam);
		Mouse.Y=HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN:
		Mouse.Left=true;
		break;
	case WM_LBUTTONUP:
		Mouse.Left=false;
		Mouse.hLeft=false;
		break;
	case WM_RBUTTONDOWN:
		Mouse.Right=true;
		break;
	case WM_RBUTTONUP:
		Mouse.Right=false;
		Mouse.hRight=false;
		break;
	case WM_MBUTTONDOWN:
		Mouse.Middle=true;
		break;
	case WM_MBUTTONUP:
		Mouse.Middle=false;
		break;
	}
}

int Win32Window::CreateWin(char* Title,int SizeX,int SizeY,HINSTANCE hInstance,int cmdShow)
{
	this->hInstance=hInstance;
	Log->WriteToLog("Registering Window.");

	wClass.cbSize			= sizeof(WNDCLASSEX);
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
	wClass.lpszClassName	= "Win32";

	if(!RegisterClassEx(&wClass))
	{
		Error("Error",ERRORDEFAULT,"Error registering class.");
		return ERROR;
	}

	Log->WriteToLog("Window Registered.");

	Log->WriteToLog("Creating Window.");

	hWnd = CreateWindowEx(	WS_EX_CLIENTEDGE,
							"Win32",
							Title,
							WS_OVERLAPPEDWINDOW,
							0, 0, SizeX, SizeY,
							NULL, NULL, this->hInstance, NULL);

	if(hWnd==NULL)
	{
		Error("Error",ERRORDEFAULT,"Error creating window.");
		return ERROR;
	}

	WinCoord.X=0;
	WinCoord.Y=0;

	WinSize.X=SizeX;
	WinSize.Y=SizeY;

	Log->WriteToLog("Window Created.");

	ShowWindow(hWnd, cmdShow);
	UpdateWindow(hWnd);

	return OK;
}

int Win32Window::KillWin()
{
	Log->WriteToLog("Killing Window");

	UnregisterClass("Win32",hInstance);

	if(hWnd && !DestroyWindow(hWnd))
	{
		Error("Error",ERRORDEFAULT,"Error destroying window, could not release hWnd");
		hWnd=NULL;
		return ERROR;
	}
	hWnd=NULL;
	Log->WriteToLog("Window Killed");
	return OK;
}

void Win32Window::AddMenu()
{
	if(!NumMenus)
		Menus=new HMENU[++NumMenus];
	else
		Menus=(HMENU*)realloc(Menus,sizeof(HMENU)*(++NumMenus));
}

void Win32Window::Error(char* title, UINT uType, const char* fmt, ...)
{
	ShowCursor(TRUE);
	char buff[1024];
	
	va_list		ap;

	if(fmt == NULL)
		return;

	va_start(ap, fmt);
	    vsprintf(buff, fmt, ap);
	va_end(ap);

	Log->WriteToLog("Error: (%s) %s",title,buff);

	MessageBox(hWnd,buff,title,uType);
	ShowCursor(FALSE);
}