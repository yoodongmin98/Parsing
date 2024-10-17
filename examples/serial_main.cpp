#include <iostream>
#include <cstdio>
#include <windows.h>
#include <memory>
#include "Core.h"



int main()
{
	std::unique_ptr<Core> MainCores = std::make_unique<Core>();
	MainCores->Start();
}