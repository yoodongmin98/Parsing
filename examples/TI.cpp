#include "TI.h"
#include "serial/serial.h"
#include "Core.h"
#include "DataClass.h"
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <ctime>
#include <thread>
#include <functional>


#define _CRT_SECURE_NO_WARNINGS
//Double Buffer
static int g_nScreenIndex;
static HANDLE g_hScreen[2];
TI::TI()
{
	ScreenInit();
}

TI::~TI()
{
	Release();
	ScreenRelease();
}

void TI::Data_receiver()
{
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	while (true)
	{
		BufferSize = my_serial.available();
		for (auto i = 0; i < BufferSize; ++i)
		{
			my_serial.read(&byte, 1);
			std::lock_guard<std::mutex> lock(mtx);
			Header.push_back(byte);
		}
		if (BufferSize > 0)
		{
			std::lock_guard<std::mutex> lock(mtx);
			Data_Ready = true;
		}
		cv.notify_one();
		SerialSize = Header.size();
	}
}
void TI::Data_processor()
{
	while (true)
	{
		std::unique_lock<std::mutex> lock(mtx);
		cv.wait(lock, [this]() { return Data_Ready; });
		PrintData();

		//while (true)
		//{
		//	if (FindHeader(Point))
		//	{
		//		OldTime = clock(); // 시간 측정
		//		Header.erase(Header.begin(), Header.begin() + Point);
		//		PrintData();
		//		break;
		//	}
		//	else
		//		Point++;
		//	Data_Ready = false;
		//	break;
		//}
	}
}
void TI::Instance()
{
	//멀티쓰레드
	std::thread T1(std::bind(&TI::Data_receiver, this));
	std::thread T2(std::bind(&TI::Data_processor, this));

	
	T1.join();
	T2.join();
}


bool TI::FindHeader(int& Point)
{
	//투 포인터
	std::vector<uint8_t> CopyArray = Header;
	sort(CopyArray.begin() + Point, CopyArray.begin() + Point + 8, std::less<uint8_t>());
	return std::equal(CopyArray.begin() + Point, CopyArray.begin() + Point + 8, MagicNumberArray.begin());
}


void TI::PrintData()
{
	SetHeaderData();
	SetUARTData();
	ConsoleBufferPrint();
}


void TI::SetHeaderData()
{
	offset = 8;
	// version (4 bytes)
	Version = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;

	// totalPacketLen (4 bytes)
	TotalPacketLen = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;

	// platform (4 bytes)
	Platform = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;

	// frameNumber (4 bytes)
	FrameNumber = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;

	// timeCpuCycles (4 bytes)
	TimeCpuCycles = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;

	// numDetectedObj (4 bytes)
	NumDetectedObj = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;

	// numTLVs (4 bytes)
	NumTLVs = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;

	// subFrameNumber (4 bytes)
	SubFrameNumber = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;
	//Header 재탐색용 초기화
	Point = 0;
}

void TI::SetUARTData()
{
	int Type = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;
	int Length = (Header[offset] | (Header[offset + 1] << 8) | (Header[offset + 2] << 16) | (Header[offset + 3] << 24));
	offset += 4;
	switch (static_cast<TypeName>(Type))
	{
	case TypeName::MMWDEMO_OUTPUT_MSG_DETECTED_POINTS:
	case TypeName::MMWDEMO_OUTPUT_MSG_RANGE_PROFILE:
	case TypeName::MMWDEMO_OUTPUT_MSG_NOISE_PROFILE:
	case TypeName::MMWDEMO_OUTPUT_MSG_AZIMUT_STATIC_HEAT_MAP:
	case TypeName::MMWDEMO_OUTPUT_MSG_RANGE_DOPPLER_HEAT_MAP:
	case TypeName::MMWDEMO_OUTPUT_MSG_STATS:
	case TypeName::MMWDEMO_OUTPUT_MSG_DETECTED_POINTS_SIDE_INFO:
	case TypeName::MMWDEMO_OUTPUT_MSG_AZIMUT_ELEVATION_STATIC_HEAT_MAP:
	case TypeName::MMWDEMO_OUTPUT_MSG_TEMPERATURE_STATS:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_DETECTED_POINTS:
	{
		float xyzUnit = 0.0f;
		memcpy(&xyzUnit, &Header[offset], sizeof(float));
		offset += 4;
		float dopplerUnit = 0.0f;
		memcpy(&dopplerUnit, &Header[offset], sizeof(float));
		
		offset = 8;
		//Test
		//계산끝나면 앞 파싱 끝난배열 지우기
		if(Header.size()>TotalPacketLen)
			Header.erase(Header.begin(), Header.begin() + TotalPacketLen);
		break;
	}
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MAJOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MINOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_AZIMUT_HEAT_MAP_MAJOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_AZIMUT_HEAT_MAP_MINOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_STATS:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_TARGET_LIST:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_TARGET_INDEX:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_MICRO_DOPPLER_RAW_DATA:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_MICRO_DOPPLER_FEATURES:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RADAR_CUBE_MAJOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RADAR_CUBE_MINOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_ENHANCED_PRESENCE_INDICATION:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_ADC_SAMPLES:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_CLASSIFIER_INFO:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RX_CHAN_COMPENSATION_INFO:
	case TypeName::MMWDEMO_OUTPUT_MSG_SPHERICAL_POINTS:
	case TypeName::MMWDEMO_OUTPUT_MSG_TRACKERPROC_3D_TARGET_LIST:
	case TypeName::MMWDEMO_OUTPUT_MSG_TRACKERPROC_TARGET_INDEX:
	case TypeName::MMWDEMO_OUTPUT_MSG_TRACKERPROC_TARGET_HEIGHT:
	case TypeName::MMWDEMO_OUTPUT_MSG_COMPRESSED_POINTS:
	case TypeName::MMWDEMO_OUTPUT_MSG_PRESCENCE_INDICATION:
	case TypeName::MMWDEMO_OUTPUT_MSG_OCCUPANCY_STATE_MACHINE:
	case TypeName::MMWDEMO_OUTPUT_MSG_SURFACE_CLASSIFICATION_PROBABILITY:
	case TypeName::MMWDEMO_OUTPUT_MSG_GESTURE_FEATURES:
	case TypeName::MMWDEMO_OUTPUT_MSG_ANN_OP_PROB:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_CAM_TRIGGERS:
	default:
		break;
	}
}

void TI::ConsoleBufferPrint()
{
	sprintf(SerialSizeInfo, "담겨있는 Serial Data Size: %d", SerialSize);
	sprintf(VersionInfo, "Version : %d", Version);
	sprintf(TotalPacketLenInfo, "TotalPacket : %d", TotalPacketLen);
	sprintf(PlatformInfo, "Platform : %d", Platform);
	sprintf(FrameNumberInfo, "FrameNumber: %d", FrameNumber);
	sprintf(TimeCpuCyclesInfo, "TimeCpuCycles: %d", TimeCpuCycles);
	sprintf(NumDetectedObjInfo, "NumDetectedObj: %d", NumDetectedObj);
	sprintf(NumTLVsInfo, "NumTLVs: %d", NumTLVs);
	sprintf(SubFrameNumberInfo, "SubFrameNumber: %d", SubFrameNumber);
	CurTime = clock();
	Render();

	ScreenClear();
}


void TI::ScreenInit()
{
	CONSOLE_CURSOR_INFO cci;

	g_hScreen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	g_hScreen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);

	cci.dwSize = 1;
	cci.bVisible = FALSE;
	SetConsoleCursorInfo(g_hScreen[0], &cci);
	SetConsoleCursorInfo(g_hScreen[1], &cci);
}


void TI::ScreenFlipping()
{
	SetConsoleActiveScreenBuffer(g_hScreen[g_nScreenIndex]);
	g_nScreenIndex = !g_nScreenIndex;
}


void TI::ScreenClear()
{
	COORD Coor = { 0, 0 };
	DWORD dw;
	FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', 80 * 25, Coor, &dw);
}


void TI::ScreenRelease()
{
	CloseHandle(g_hScreen[0]);
	CloseHandle(g_hScreen[1]);
}


void TI::ScreenPrint(int x, int y, char* string)
{
	DWORD dw;
	COORD CursorPosition = { x, y };
	SetConsoleCursorPosition(g_hScreen[g_nScreenIndex], CursorPosition);
	WriteFile(g_hScreen[g_nScreenIndex], string, strlen(string), &dw, NULL);
}


void TI::Render()
{
	if (CurTime - OldTime >= 500)
	{
		OldTime = CurTime;
		g_numofFPS = 0;
	}
	g_numofFPS++;
	ScreenPrint(1, 10, SerialSizeInfo);
	ScreenPrint(0, 0, VersionInfo);
	ScreenPrint(0, 1, TotalPacketLenInfo);
	ScreenPrint(0, 2, PlatformInfo);
	ScreenPrint(0, 3, FrameNumberInfo);
	ScreenPrint(0, 4, TimeCpuCyclesInfo);
	ScreenPrint(0, 5, NumDetectedObjInfo);
	ScreenPrint(0, 6, NumTLVsInfo);
	ScreenPrint(0, 7, SubFrameNumberInfo);
	ScreenFlipping();
}


void TI::Release()
{
	delete[] VersionInfo;
	delete[] TotalPacketLenInfo;
	delete[] PlatformInfo;
	delete[] FrameNumberInfo;
	delete[] TimeCpuCyclesInfo;
	delete[] NumDetectedObjInfo;
	delete[] NumTLVsInfo;
	delete[] SubFrameNumberInfo;
}