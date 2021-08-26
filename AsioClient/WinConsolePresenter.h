#pragma once
#include "ConsolePresenter.h"
#include <Windows.h>
#include <string>
#include <iostream>

class WinConsolePresenter : public ConsolePresenter
{
private:
	HANDLE consoleHandle;

public:
	WinConsolePresenter();
	void ConsoleWrite(std::string mes) override;
	std::string GetConsoleInput() override;
};

