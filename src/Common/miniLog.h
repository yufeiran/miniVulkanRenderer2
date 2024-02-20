#pragma once
#include<iostream>
#include<string>
#include<sstream>
namespace mini
{
	enum LOG_TYPE { INFO_TYPE, ERROR_TYPE, WARNING_TYPE,NONE_TYPE };
	const int LOG_TYPE_SUM = 3;

	void outputTag(LOG_TYPE logType = INFO_TYPE);
	void Log(const char* message, LOG_TYPE logType = INFO_TYPE);
	void Log(const std::string message, LOG_TYPE logType = INFO_TYPE);
	void LogI(const char* message);
	void LogE(const char* message);
	void LogW(const char* message);
	void LogN(const char* message);

	void LogI(const std::string&& message);
	void LogE(const std::string&& message);
	void LogW(const std::string&& message);
	void LogN(const std::string&& message);


	
	void LogSpace();
	void LogLogo();
	void LogProgressBar(const std::string& title,double percent);
	void LogWait(const std::string& message);
	void LogWaitEnd();

	void LogTimerStart(const std::string& jobName);
	void LogTimerEnd(const std::string& jobName);

}

