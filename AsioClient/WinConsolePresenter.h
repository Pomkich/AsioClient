#pragma once
#include "ConsolePresenter.h"
#include <Windows.h>
#include <string>
#include <iostream>
#include <mutex>

class WinConsolePresenter : public ConsolePresenter
{
private:
	HANDLE consoleHandle;
	std::mutex mut;
public:
	WinConsolePresenter();
	void ConsoleWrite(std::string mes) override;
	std::string GetConsoleInput() override;
};

