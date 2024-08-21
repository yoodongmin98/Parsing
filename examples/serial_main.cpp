#include <string>
#include <iostream>
#include <cstdio>
#include <windows.h>

#include "MyImGui.h"


int main(int argc, char** argv)
{
	std::unique_ptr<MyImGui> MyImGuis = std::make_unique<MyImGui>();
	MyImGuis->Instance(argc, argv);
}