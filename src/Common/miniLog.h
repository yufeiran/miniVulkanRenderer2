#pragma once
#include<iostream>
#include<string>
#include<sstream>
namespace mini
{
	enum LOG_TYPE { INFO_TYPE, ERROR_TYPE, WARNING_TYPE };
	const int LOG_TYPE_SUM = 3;

	void Log(const char* message, LOG_TYPE logType = INFO_TYPE);
	void Log(const std::string message, LOG_TYPE logType = INFO_TYPE);


}

