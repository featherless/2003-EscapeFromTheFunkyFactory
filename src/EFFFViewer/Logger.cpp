#include "Logger.h"

int Logger::InitLog(char *filename)
{
	FileName=filename;

	LoggingIsOn=true;

	return OK;
}

int Logger::WriteToLog(const char* text, ...)
{
	if(!LoggingIsOn)
		return OK;

	char buff[1024];
	
	va_list ap;

	if(text==NULL)
		return ERROR;

	va_start(ap, text);
	vsprintf(buff, text, ap);
	va_end(ap);

	time_t plaintime;
	tm *ptm;

	time(&plaintime);
	ptm = localtime(&plaintime);

	ofstream file;

	if(!FileName)
		file.open("LOG.txt",ios::out | ios::app);
	else
		file.open(FileName,ios::out | ios::app);

	if(!file)
		return ERROR;
	else
	{
		file << ptm->tm_mon+1 << '/' << ptm->tm_mday << '/' << ptm->tm_year+1900 <<
			    ' ' << ptm->tm_hour << ':' << ptm->tm_min << ':' << ptm->tm_sec;
		file << "  ";

		file << buff;
		file << endl;
	}

	file.close();

	return OK;
}