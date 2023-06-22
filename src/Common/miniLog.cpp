#include "miniLog.h"
#include<Windows.h>

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



void Log(const char* message, LOG_TYPE logType)
{

	std::cout << "[";


	SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | LOG_TYPE_COLOR[logType]);

	std::cout << LOG_TYPE_STR[logType];
	SetConsoleTextAttribute(handle, FOREGROUND_INTENSITY | 7);

	std::cout << "] " << message << std::endl;
}

void Log(const std::string message, LOG_TYPE logType)
{
	Log(message.c_str(), logType);
}
}

