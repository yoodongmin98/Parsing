#pragma once
#include <iostream>
#include "serial/serial.h"
#include "Windows.h"
class TI
{
public:
	TI();
	~TI();

	void Instance();
	
protected:
private:
	uint8_t byte = 0;
	size_t BufferSize = 0;
	int Point = 0;



	//Header(MagicNumber)
	std::vector<uint8_t> Header;
	const std::vector<int> MagicNumberArray = { 1,2,3,4,5,6,7,8 };

	//Header Data
	int Version = 0;
	int TotalPacketLen = 0;
	int Platform = 0;
	int FrameNumber = 0;
	int TimeCpuCycles = 0;
	int NumDetectedObj = 0;
	int NumTLVs = 0;
	int SubFrameNumber = 0;

	

	serial::Serial my_serial;
	bool FindHeader(int& Point);
	void PrintHeaderData();
	void SetPrintData();


	//Double Buffer
	void ScreenInit();
	void ScreenFlipping();
	void ScreenClear();
	void ScreenRelease();
	void ScreenPrint(int x, int y, char* string);
	void Render();
	void Release();

	int g_numofFPS = 0;
	clock_t CurTime, OldTime;

	char* VersionInfo = new char[128];
	char* TotalPacketLenInfo = new char[128];
	char* PlatformInfo = new char[128];
	char* FrameNumberInfo = new char[128];
	char* TimeCpuCyclesInfo = new char[128];
	char* NumDetectedObjInfo = new char[128];
	char* NumTLVsInfo = new char[128];
	char* SubFrameNumberInfo = new char[128];

};