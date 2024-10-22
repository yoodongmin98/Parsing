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


void TI::Instance()
{
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	while (true)
	{
		BufferSize = my_serial.available();
		for (auto i = 0; i < BufferSize; ++i)
		{
			my_serial.read(&byte, 1);
			Header.push_back(byte);
		}
		SerialSize = Header.size();
		if (Header.size() > TotalPacketLen+500)
		{
			while (Point <= Header.size() - 8)
			{
				if (FindHeader(Point))
				{
					OldTime = clock(); // 시간 측정
					Header.erase(Header.begin(), Header.begin() + Point);
					PrintData();
					break;
				}
				else
					Point++;
			}
		}
	}
}


bool TI::FindHeader(int Point)
{
	//투 포인터
	std::vector<uint8_t> CopyArray = Header;
	if (Point < 0 || Point + 8 > CopyArray.size()) // Point가 유효한지 확인
	{
		return false; // 유효하지 않으면 false 반환
	}
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
	ParsingDataPrint("Version", 0, 0, "int");
	ParsingDataPrint("TotalPacketLen", 0, 1, "int");
	ParsingDataPrint("Platform", 0, 2, "int");
	ParsingDataPrint("FrameNumber", 0, 3, "int");
	ParsingDataPrint("TimeCpuCycles", 0, 4, "int");
	ParsingDataPrint("NumDetectedObj", 0, 5, "int");
	ParsingDataPrint("NumTLVs", 0, 6, "int");
	ParsingDataPrint("SubFrameNumber", 0, 7, "int");
	//Header 재탐색용 초기화
	Point = 1;
}

void TI::SetUARTData()
{
	int Type = ParsingDataPrint("Type", 0, 13, "int");
	ParsingDataPrint("Length", 0, 14, "int");
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
		float UnitValue=ParsingDataPrint("xyzUnit", 0, 15, "float");
		ParsingDataPrint("dopplerUnit", 0, 16, "float");
		ParsingDataPrint("snrUnit", 0, 17, "float");
		ParsingDataPrint("noiseUnit", 0, 18, "float");
		ParsingDataPrint("numDetectedPoint1", 0, 19, "int",2);
		ParsingDataPrint("numDetectedPoint2", 0, 20, "int",2);

		//point X같은거 자꾸 끊겨서보임 더블버퍼문제인듯
		ParsingDataPrint("PointX", 0, 22, "int", 2);
		ParsingDataPrint("PointY", 0, 23, "int", 2);
		ParsingDataPrint("PointZ", 0, 24, "int", 2);
		ParsingDataPrint("Doppler", 0, 25, "int", 2);
		ParsingDataPrint("snr", 0, 26, "int", 1);
		ParsingDataPrint("noise", 0, 27, "int", 1);

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
	offset = 8;
}

void TI::ConsoleBufferPrint()
{
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