#ifndef LOGGER
#define LOGGER

#include <fstream.h>
#include <stdio.h>
#include <time.h>
#include "CommonWin32.h"

class Logger
{
private:
	char* FileName;
public:
	bool LoggingIsOn;

	int InitLog(char *filename);
	int WriteToLog(const char* text, ...);

	void Error(char* title, UINT uType, const char* fmt, ...);
};

#endif