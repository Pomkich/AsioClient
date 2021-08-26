#pragma once
#include <string>
#include <iostream>

class ConsolePresenter
{
protected:
	enum { max_len = 1024 };
	char input_buf[max_len];
public:
	virtual void ConsoleWrite(std::string mes) = 0;
	virtual std::string GetConsoleInput() = 0;
};

