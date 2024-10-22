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
	int LineStartPoint = 0;
	offset = 8;
	ParsingDataPrint("Version", 0, LineStartPoint, "int");
	PacketSize=ParsingDataPrint("TotalPacketLen", 0, LineStartPoint, "int");
	ParsingDataPrint("Platform", 0, LineStartPoint, "int");
	ParsingDataPrint("FrameNumber", 0, LineStartPoint, "int");
	ParsingDataPrint("TimeCpuCycles", 0, LineStartPoint, "int");
	ParsingDataPrint("NumDetectedObj", 0, LineStartPoint, "int");
	ParsingDataPrint("NumTLVs", 0, LineStartPoint, "int");
	ParsingDataPrint("SubFrameNumber", 0, LineStartPoint, "int");
	//Header 재탐색용 초기화
	Point = 1;
}

void TI::SetUARTData()
{
	int LineStartPoint = 13; //이거 기준으로 print함수안에 offset처럼 자동으로 줄넘김해주는 함수 만들기
	int Type = ParsingDataPrint("Type", 0, LineStartPoint, "int");
	int Length = ParsingDataPrint("Length", 0, LineStartPoint, "int");
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
		char DataName[128] = "20byte(xyzUnit,dopperUnit,snrUnit,noiseUnit,numDetectedPoints) + 10byte(x,y,z,doppler,snr,noise) X NumDetected Obj";
		ScreenPrint(0, LineStartPoint, DataName);

		//LineStart 너 왜 여기서 f에서 안올라가냐 이 괘씸한놈아
		float UnitValue=ParsingDataPrint("xyzUnit", 0, LineStartPoint, "float"); //나중에 pos도 다 변수로 바꿔야함 + 데이터 여러개면 합산의 평균을 구해야할듯
		ParsingDataPrint("dopplerUnit", 0, LineStartPoint, "float");
		ParsingDataPrint("snrUnit", 0, LineStartPoint, "float");
		ParsingDataPrint("noiseUnit", 0, LineStartPoint, "float");
		ParsingDataPrint("numDetectedPoint1", 0, LineStartPoint, "int",2);
		ParsingDataPrint("numDetectedPoint2", 0, LineStartPoint, "int",2);

		//point X같은거 자꾸 끊겨서보임 더블버퍼문제인듯
		ParsingDataPrint("PointX", 0, LineStartPoint, "int", 2);
		ParsingDataPrint("PointY", 0, LineStartPoint, "int", 2);
		ParsingDataPrint("PointZ", 0, LineStartPoint, "int", 2);
		ParsingDataPrint("Doppler", 0, LineStartPoint, "int", 2);
		ParsingDataPrint("snr", 0, LineStartPoint, "int", 1);
		ParsingDataPrint("noise", 0, LineStartPoint, "int", 1);

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

float TI::ParsingDataPrint(std::string _PrintData, int _posX, int& _posY, std::string _floatint, int _bytesize, float _multiply)
{
	transform(_floatint.begin(), _floatint.end(), _floatint.begin(), ::toupper);

	char PrintInfo[64];

	if (_floatint == "FLOAT")
	{
		float Data = 0.0f;
		memcpy(&Data, &Header[offset], sizeof(float));
		sprintf(PrintInfo, (_PrintData + " : %f ").c_str(), Data);
		offset += 4;
		ScreenPrint(_posX, _posY, PrintInfo);
		return Data;
	}
	if (_floatint == "INT")
	{
		int BitData = 0;
		for (auto i = 0; i < _bytesize; ++i)
		{
			BitData |= (Header[offset + i] << i * 8);
		}
		if (_multiply)
			sprintf(PrintInfo, (_PrintData + " : %d ").c_str(), BitData * _multiply);
		else
			sprintf(PrintInfo, (_PrintData + " : %d ").c_str(), BitData);
		offset += _bytesize;
		_posY += 1;
		ScreenPrint(_posX, _posY, PrintInfo);
		return static_cast<int>(BitData);
	}
}