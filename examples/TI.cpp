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


//Serial Data를 처리 인스턴싱
void TI::Instance()
{
	//설정된 PortNumber , Baudrate를 통해 SerialPort를 염
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	while (true)
	{
		//PC의 한 프레임에 들어온 Data의 개수를 받음
		BufferSize = my_serial.available();
		//한 프레임에 들어온 Data의 개수만큼 데이터를 읽어서 Header라는 vector안에 넣음
		for (auto i = 0; i < BufferSize; ++i)
		{
			my_serial.read(&byte, 1);
			Header.push_back(byte);
		}
		//기본적으로 하나의 총 패킷프레임만큼 돌려야하므로 초기 TotalPacketLen의 값은 1000 << 추후 변동 가능
		//그 후에 들어오는 Packet의 길이가 가변하므로 3배의 데이터를 여유를 두어 처리함
		if (Header.size() > TotalPacketLen * 3)
		{
			//Point라는 변수가 Header안에 담긴 매직넘버를 찾기때문에 HeaderSize-8만큼만 돌아감
			while (Point <= Header.size() - 8)
			{
				if (FindHeader(Point)) //헤더찾는함수
				{
					OldTime = clock(); // 시간 측정(더블버퍼링 변수처리를 위한 측정)
					Header.erase(Header.begin(), Header.begin() + Point); //헤더를 찾으면 그 전 데이터는 지워버림(vector capacity고려) 

					PrintData(); //데이터 출력
					break;
				}
				else
					Point++; //하나씩 돌면서 못찾으면 다음index로 넘어감(나중에 이진탐색으로 바꿔도 됨)
			}
		}
	}
}


//헤더의 매직넘버를 찾는 함수
bool TI::FindHeader(int Point)
{
	//원본 배열이 sort되면 안되므로 배열자체를 복사해서 구성요소를 매직넘버 배열과 대조함
	std::vector<uint8_t> CopyArray = Header; 
	if (Point < 0 || Point + 8 > CopyArray.size()) // Point가 유효한지 확인
	{
		return false; // Point의 값이 유효하지 않으면 false 반환
	}
	//Point부터 8개의 배열을 낮은순서대로 정렬함
	sort(CopyArray.begin() + Point, CopyArray.begin() + Point + 8, std::less<uint8_t>());
	//매직넘버 배열과 비교해서 1~8이 맞다면 true, 아니라면 false반환
	return std::equal(CopyArray.begin() + Point, CopyArray.begin() + Point + 8, MagicNumberArray.begin());
}


//데이터 파싱 및 콘솔더블버퍼링을 이용한 데이터 출력
void TI::PrintData()
{
	SetHeaderData();
	//offset이 TotalPacketlen보다 작을때 계속 돌아간다
	//(하나의 패킷사이즈를 전부 파싱할때까지 돈다)
	while (offset < TotalPacketLen)
	{
		SetUARTData();
	}
	Header.erase(Header.begin(), Header.begin() + offset);
	ConsoleBufferPrint();
}


void TI::SetHeaderData()
{
	offset = 8; //헤더의 파싱위치 기준을 담당할 변수
	Point = 0;	//Header 재탐색용 변수 초기화
	CharPrint("★★HEADER DATA★★", 0, LineStartPoint);
	ParsingDataPrint("Version", 0, LineStartPoint, "Version");
	TotalPacketLen = ParsingDataPrint("TotalPacketLen", 0, LineStartPoint, "int");
	ParsingDataPrint("Platform", 0, LineStartPoint, "int");
	ParsingDataPrint("FrameNumber", 0, LineStartPoint, "int");
	ParsingDataPrint("TimeCpuCycles", 0, LineStartPoint, "int");
	ParsingDataPrint("NumDetectedObj", 0, LineStartPoint, "int");
	ParsingDataPrint("NumTLVs", 0, LineStartPoint, "int");
	ParsingDataPrint("SubFrameNumber", 0, LineStartPoint, "int");

	
}


//칩에 입력한 명령어(CLI)의 타입에따라 데이터를 파싱해줌
void TI::SetUARTData()
{
	//데이터의 타입 먼저 파싱

	//Debug용
	int Type = (Header[offset] | Header[offset + 1] << 8 | Header[offset + 2] << 16 | Header[offset + 3] << 24);


	//데이터의 타입에 따라 출력되는 데이터 처리+
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
		int MiddleX = 70;
		CharPrint("----------------------------------------------------데이터 길이----------------------------------------------------", 20, LineStartPoint);
		CharPrint("20byte(xyzUnit,dopperUnit,snrUnit,noiseUnit,numDetectedPoints) + 10byte(x,y,z,doppler,snr,noise) X NumDetected Obj", 20, LineStartPoint);
		CharPrint("-------------------------------------------------------------------------------------------------------------------", 20, LineStartPoint);
		CharPrint("★입력된 데이터의 타입은 'MMWDEMO_OUTPUT_EXT_MSG_DETECTED_POINTS' 입니다★ ", 40, LineStartPoint);
		LineStartPoint++;
		ParsingDataPrint("TypeNumber", MiddleX, LineStartPoint, "int");
		int Length = ParsingDataPrint("Length", MiddleX, LineStartPoint, "int");
	
		ParsingDataPrint("xyzUnit", MiddleX, LineStartPoint, "float");
		ParsingDataPrint("dopplerUnit", MiddleX, LineStartPoint, "float");
		ParsingDataPrint("snrUnit", MiddleX, LineStartPoint, "float");
		ParsingDataPrint("noiseUnit", MiddleX, LineStartPoint, "float");
		ParsingDataPrint("numDetectedPoint1", MiddleX, LineStartPoint, "int",2);
		ParsingDataPrint("numDetectedPoint2", MiddleX, LineStartPoint, "int",2);

		//point X같은거 자꾸 끊겨서보임 더블버퍼문제인듯
		ParsingDataPrint("PointX", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("PointY", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("PointZ", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("Doppler", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("snr", MiddleX, LineStartPoint, "int", 1);
		ParsingDataPrint("noise", MiddleX, LineStartPoint, "int", 1);
		LineStartPoint++;
		CharPrint("실제 값을 얻으려면 모든 포인트 값에 적절한 포인트 클라우드 단위 값을 곱해야합니다.", 38, LineStartPoint);
		CharPrint("EX) 점의 실제좌표 : x * xyzUnit ", 60, LineStartPoint);
		LineStartPoint++;
		//일단 데이터 넘겨야하니까 length만큼 offset더해줌--------------------------------------------------------------------------------------------------------------
		offset += Length - 30;
		break;
	}
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MAJOR:
	{
		int MiddleX = 70;
		CharPrint("----------------------------------------------------데이터 길이----------------------------------------------------", 20, LineStartPoint);
		CharPrint("----------------------------------------(Number Of Range Bins) x (4 Bytes)----------------------------------------", 20, LineStartPoint);
		CharPrint("-------------------------------------------------------------------------------------------------------------------", 20, LineStartPoint);
		CharPrint("★입력된 데이터의 타입은 'MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MAJOR' 입니다★ ", 36, LineStartPoint);
		LineStartPoint++;
		ParsingDataPrint("TypeNumber", MiddleX, LineStartPoint, "int");
		int Length = ParsingDataPrint("Length", MiddleX, LineStartPoint, "int");
		LineStartPoint++;
		offset += Length;
		break;
	}
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MINOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_AZIMUT_HEAT_MAP_MAJOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_AZIMUT_HEAT_MAP_MINOR:
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_STATS:
	{
		int MiddleX = 70;
		CharPrint("----------------------------------------------------데이터 길이----------------------------------------------------", 20, LineStartPoint);
		CharPrint("------------24 Bytes(InterFrame 4byte + transmitOutput 4byte + powerMeasured 8byte + tempReading 8byte)------------", 20, LineStartPoint);
		CharPrint("-------------------------------------------------------------------------------------------------------------------", 20, LineStartPoint);
		CharPrint("★입력된 데이터의 타입은 'MMWDEMO_OUTPUT_EXT_MSG_STATS' 입니다★ ", 44, LineStartPoint);
		LineStartPoint++;
		ParsingDataPrint("TypeNumber", MiddleX, LineStartPoint, "int");
		int Length = ParsingDataPrint("Length", MiddleX, LineStartPoint, "int");

		ParsingDataPrint("InterFrameProcessingTime[usec] : ", MiddleX, LineStartPoint, "int");
		ParsingDataPrint("TransmitOutputTime[usec] : ", MiddleX, LineStartPoint, "int");
		ParsingDataPrint("PowerMeasured[1] : ", MiddleX, LineStartPoint, "int",2);
		ParsingDataPrint("PowerMeasured[2] : ", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("PowerMeasured[3] : ", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("PowerMeasured[4] : ", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("TempReading[1] : ", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("TempReading[2] : ", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("TempReading[3] : ", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("TempReading[4] : ", MiddleX, LineStartPoint, "int", 2);
		LineStartPoint++;
		offset += Length;
		break;
	}
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
	{
		offset = TotalPacketLen;
		break;
	}
	}
	
}

void TI::ConsoleBufferPrint()
{
	//offset초기화
	offset = 8;
	
	Render();
	ScreenClear();
}


//더블 버퍼 화면 init해주는 함수
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
	FillConsoleOutputCharacter(g_hScreen[g_nScreenIndex], ' ', 1300 * 1200, Coor, &dw);
	LineStartPoint = 0; //콘솔의 출력되는 줄을 담당할 변수
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
	CurTime = clock();
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

}

float TI::ParsingDataPrint(std::string _PrintData, int _posX, int& _posY, std::string _floatint, int _bytesize, float _multiply)
{
	//들어온 string을 대소문자 구분없이 대문자로 변환해줌
	transform(_floatint.begin(), _floatint.end(), _floatint.begin(), ::toupper);

	char PrintInfo[64];

	if (_floatint == "FLOAT")
	{
		//float는 부동소수점 형식이기때문에 비트연산 대신 memcpy
		float Data = 0.0f;
		memcpy(&Data, &Header[offset], sizeof(float));
		sprintf(PrintInfo, (_PrintData + " : %f ").c_str(), Data);
		offset += 4;
		ScreenPrint(_posX, _posY, PrintInfo);
		_posY += 1;
		return Data;
	}
	else if (_floatint == "INT")
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
		ScreenPrint(_posX, _posY, PrintInfo);
		_posY += 1;
		return static_cast<int>(BitData);
	}
	else if (_floatint == "VERSION")
	{
		sprintf(PrintInfo, (_PrintData + " : %d.%d.%d ").c_str(), Header[offset+3], Header[offset + 2], Header[offset + 1]);
		ScreenPrint(_posX, _posY, PrintInfo);
		offset += 4;
		_posY += 1;
		return 0;
	}
}

void TI::CharPrint(std::string _PrintChar, int _posX, int& _posY)
{
	char PrintInfo[128];
	sprintf(PrintInfo, (_PrintChar).c_str());
	ScreenPrint(_posX, _posY, PrintInfo);
	_posY += 1;
}