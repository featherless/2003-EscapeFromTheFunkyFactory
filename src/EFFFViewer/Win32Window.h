#ifndef WIN32WINDOW
#define WIN32WINDOW

#include <windows.h>
#include "CommonWin32.h"
#include "Logger.h"

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class Win32Window
{
private:
    WNDCLASSEX wClass;
	HWND hWnd;
	HINSTANCE hInstance;

	HICON hIcon;
	HICON hIconSmall;
	HCURSOR hCursor;

	Logger* Log;

	int NumMenus;
public:
	PIXEL WinCoord;
	PIXEL WinSize;

	MOUSE Mouse;

	HMENU* Menus;

	void InitWindow(Logger *Log);
	int CreateWin(char* Title,int SizeX,int SizeY,HINSTANCE hInstance,int cmdShow=SW_SHOW);

	void AddMenu();

	void InitCursor(HCURSOR hCursor) { this->hCursor=hCursor; };
	void InitIcon(HICON hIcon,HICON hIconSmall) { this->hIcon=hIcon; this->hIconSmall=hIconSmall; };

	void UpdateWindowDetails(int Value,WPARAM wParam,LPARAM lParam);

	int KillWin();

	HWND GetHWND() { return hWnd; };
	HINSTANCE GetHINSTANCE() { return hInstance; };

	void Error(char* title, UINT uType, const char* fmt, ...);
};

#endif