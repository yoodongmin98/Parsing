#include <string>
#include <iostream>
#include <cstdio>
#include <windows.h>

#include "MyImGui.h"


int main(int argc, char** argv)
{
	//ImGui라이브러리를 이용한 프레임헤더X , 패리티비트X , CheckSum X 인 단일 I,Q데이터를 그래프로 시각화
	std::unique_ptr<MyImGui> MyImGuis = std::make_unique<MyImGui>();
	MyImGuis->Instance(argc, argv);
}