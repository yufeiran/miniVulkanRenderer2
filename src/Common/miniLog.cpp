#include "miniLog.h"
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif
#include <fstream>
#include <chrono>
#include <map>
#include <cstring>

#include "Common/common.h"

namespace mini {

std::ofstream& getLogFile() {
	static std::ofstream instance;
	return instance;
}

const char LOG_TYPE_STR[3][10] = {
	"INFO","ERROR","WARN"
};

const int LOG_TYPE_COLOR[LOG_TYPE_SUM] = {
	0xA,
	4,
	6
};

#if defined(_WIN32) || defined(_WIN64)
HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

std::string toHex(uint64_t handle) {
	std::stringstream stream;
	// Prefix with '0x' and output in hexadecimal
	stream << "0x" << std::hex << handle;
	return stream.str();
}

void initLogFile(const std::string& filePath)
{
	getLogFile().open(filePath, std::ios::out | std::ios::trunc);
	if (!getLogFile().is_open())
	{
		LogE("Failed to open log file: " + filePath);
	}
}

void closeLogFile()
{
	if (getLogFile().is_open())
	{
		getLogFile().flush();
		getLogFile().close();
	}
}

void outputTag(LOG_TYPE logType,bool writeToFile)
{
	if(logType!=NONE_TYPE)
	{	std::cout << "[";

#if defined(_WIN32) || defined(_WIN64)
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | LOG_TYPE_COLOR[logType]);
#endif

		std::cout << LOG_TYPE_STR[logType];
#if defined(_WIN32) || defined(_WIN64)
		SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | 7);
#endif
		std::cout << "] ";

		if (writeToFile && getLogFile().is_open())
		{
			getLogFile() << "[" << LOG_TYPE_STR[logType] << "] ";
		}
	}
}

void Log(const char* message, LOG_TYPE logType)
{
	outputTag(logType);
	std::cout<< message << std::endl;
	if (getLogFile().is_open())
	{
		getLogFile() << message << std::endl;
	}
}

void Log(const std::string message, LOG_TYPE logType)
{
	Log(message.c_str(), logType);
}

void LogI(const char* message)
{
	Log(message, LOG_TYPE::INFO_TYPE);
}

void LogE(const char* message)
{
	Log(message, LOG_TYPE::ERROR_TYPE);
}

void LogW(const char* message)
{
	Log(message, LOG_TYPE::WARNING_TYPE);
}

void LogN(const char* message)
{
	Log(message, LOG_TYPE::NONE_TYPE);
}

void LogI(const std::string&& message)
{
	LogI(message.c_str());
}

void LogE(const std::string&& message)
{
	LogE(message.c_str());
}

void LogW(const std::string&& message)
{
	LogW(message.c_str());
}

void LogN(const std::string&& message)
{
	LogN(message.c_str());
}

void LogSpace()
{
	Log("----------------------------------------------------------------------------",NONE_TYPE);
}

void LogLogo()
{
	std::ifstream logoFile;
	logoFile.open(getAssetPath("logo.txt"));
	while(logoFile)
	{
		char lineBuf[200];
		logoFile.getline(lineBuf,200);
		std::cout<<lineBuf<<std::endl;
		if (getLogFile().is_open())
		{
			getLogFile() << lineBuf << std::endl;
		}
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
	outputTag(INFO_TYPE,false);
	printf("%s \t[%s",title.c_str(),progressBarData);
	for(int i=0;i<progressLen-nowProgressCount - 1;i++)
	{
		printf(" ");
	}
	printf("] %3.0f%% ",percent*100);
	if(percent == 1.0f)
	{
		printf("\n");
		if(getLogFile().is_open())
		{
			getLogFile() << title << " \t[" << progressBarData << "] 100%" << std::endl;
		}
	}

}

std::string waitMessage;
void LogWait(const std::string& message)
{

	waitMessage = message;
	static int nowCount = 0;
	const char waitChar[4] = {'|','/','-','\\'};
	printf("\r");
	outputTag();

	printf("%s %c",message.c_str(),waitChar[nowCount % 4]);
	nowCount++;
}

void LogWaitEnd()
{
	
	printf("\r");
	outputTag();
	printf("%s\n",waitMessage.c_str());

	if (getLogFile().is_open())
	{
		getLogFile() << waitMessage << std::endl;
	}
	waitMessage.clear();
}

std::chrono::time_point last = std::chrono::system_clock::now();

std::map<std::string,std::chrono::time_point<std::chrono::system_clock>> startTimeMap;

void LogTimerStart(const std::string& jobName)
{
	startTimeMap[jobName] = std::chrono::system_clock::now();

}

void LogTimerEnd(const std::string& jobName)
{
	auto result = startTimeMap.find(jobName);

	if(result == startTimeMap.end())
	{
		Log("no match job \""+jobName+"\"",LOG_TYPE::WARNING_TYPE);
		return;
	}

	auto now = std::chrono::system_clock::now();
	std::chrono::duration<double, std::milli> dur = now - result->second;
	double frameTime = double(dur.count());
	Log(jobName + " finished in "+ std::to_string(static_cast<int>(frameTime))+" ms");
	
	startTimeMap.erase(jobName);
}

}

