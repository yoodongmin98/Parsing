#include <iostream>
#include <cstdio>
#include <windows.h>
#include <memory>
#include "Core.h"



int main()
{
	HWND console = GetConsoleWindow();
	RECT r;
	GetWindowRect(console, &r);
	MoveWindow(console, 50, 50, 1300, 1200, TRUE);
	std::unique_ptr<Core> MainCores = std::make_unique<Core>();
	MainCores->Start();
}