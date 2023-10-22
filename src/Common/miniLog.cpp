#include "miniLog.h"
#include<Windows.h>
#include<fstream>

namespace mini {

const char LOG_TYPE_STR[3][10] = {
	"INFO","ERROR","WARN"
};

const int LOG_TYPE_COLOR[LOG_TYPE_SUM] = {
	0xA,
	4,
	6
};

HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

void outputTag(LOG_TYPE logType)
{
	if(logType!=NONE_TYPE)
	{	std::cout << "[";

		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | LOG_TYPE_COLOR[logType]);

		std::cout << LOG_TYPE_STR[logType];
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | 7);

		std::cout << "] ";
	}
}

void Log(const char* message, LOG_TYPE logType)
{
	outputTag(logType);
	std::cout<< message << std::endl;
}

void Log(const std::string message, LOG_TYPE logType)
{
	Log(message.c_str(), logType);
}

void LogSpace()
{
	Log("----------------------------------------------------------------------------",NONE_TYPE);
}

void LogLogo()
{
	std::ifstream logoFile;
	logoFile.open("../../assets/logo.txt");
	while(logoFile)
	{
		char lineBuf[200];
		logoFile.getline(lineBuf,200);
		std::cout<<lineBuf<<std::endl;
	}
}

void LogProgressBar(const std::string& title, double percent)
{
	const int progressLen = 50;
	int nowProgressCount = progressLen * percent;
	
	char progressBarData[progressLen+1]={};

	memset(progressBarData,'=',nowProgressCount);
	if(nowProgressCount<progressLen)
	{
		progressBarData[nowProgressCount]='>';
	}
	else
	{
		int a;
	}
	printf("\r");
	outputTag();
	printf("%s [%s",title.c_str(),progressBarData);
	for(int i=0;i<progressLen-nowProgressCount;i++)
	{
		printf(" ");
	}
	printf("] %3.0f%% ",percent*100);
	if(percent == 1.0f)
	{
		printf("\n");
	}

}

}

