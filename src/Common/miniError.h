#pragma once
#include<stdexcept>
#include"miniLog.h"

namespace mini
{
	class Error :public std::exception
	{
	public:
		Error(const char* msg) :error_message(msg) {
			Log(msg, ERROR_TYPE);
		}
		const char* what() const noexcept override {
			return error_message.c_str();
		}
	private:
		std::string error_message;
	};
}

