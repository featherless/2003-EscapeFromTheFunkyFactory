#ifndef RETURNS
#define RETURNS

#include <windows.h>

#define ERROR			0
#define OK				1

#define ERRORDEFAULT	MB_OK | MB_ICONEXCLAMATION

#endif

#ifndef STRUCTS
#define STRUCTS

#ifndef PIXEL
struct PIXEL
{
	int X,Y;
};
#endif

struct MOUSE
{
	int X,Y;
	bool Left,Right,Middle;
	bool hLeft,hRight,hMiddle;
};

#endif

#ifndef MACROS
#define MACROS

#define ISPOW2(a) ((a) & ((a)-1))

#endif