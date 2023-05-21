#pragma once
#include<stdexcept>
#include"miniLog.h"

class Error :public std::exception
{
public:
	Error(const char* msg) :error_message(msg) {
		Log(msg, ERROR);
	}
	const char* what() const noexcept override {
		return error_message.c_str();
	}
private:
	std::string error_message;
};