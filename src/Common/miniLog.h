#pragma once
#include<iostream>
#include<string>
#include<sstream>
namespace mini
{
	enum LOG_TYPE { INFO, ERROR, WARNING };
	const int LOG_TYPE_SUM = 3;


	void Log(const char* message, LOG_TYPE logType = INFO);
	void Log(const std::string message, LOG_TYPE logType = INFO);


}

