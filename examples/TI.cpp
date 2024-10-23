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


//Serial Data�� ó�� �ν��Ͻ�
void TI::Instance()
{
	//������ PortNumber , Baudrate�� ���� SerialPort�� ��
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	while (true)
	{
		//PC�� �� �����ӿ� ���� Data�� ������ ����
		BufferSize = my_serial.available();
		//�� �����ӿ� ���� Data�� ������ŭ �����͸� �о Header��� vector�ȿ� ����
		for (auto i = 0; i < BufferSize; ++i)
		{
			my_serial.read(&byte, 1);
			Header.push_back(byte);
		}
		//�⺻������ �ϳ��� �� ��Ŷ�����Ӹ�ŭ �������ϹǷ� �ʱ� TotalPacketLen�� ���� 1000 << ���� ���� ����
		//�� �Ŀ� ������ Packet�� ���̰� �����ϹǷ� 3���� �����͸� ������ �ξ� ó����
		if (Header.size() > TotalPacketLen * 3)
		{
			//Point��� ������ Header�ȿ� ��� �����ѹ��� ã�⶧���� HeaderSize-8��ŭ�� ���ư�
			while (Point <= Header.size() - 8)
			{
				if (FindHeader(Point)) //���ã���Լ�
				{
					OldTime = clock(); // �ð� ����(������۸� ����ó���� ���� ����)
					Header.erase(Header.begin(), Header.begin() + Point); //����� ã���� �� �� �����ʹ� ��������(vector capacity���) 

					PrintData(); //������ ���
					break;
				}
				else
					Point++; //�ϳ��� ���鼭 ��ã���� ����index�� �Ѿ(���߿� ����Ž������ �ٲ㵵 ��)
			}
		}
	}
}


//����� �����ѹ��� ã�� �Լ�
bool TI::FindHeader(int Point)
{
	//���� �迭�� sort�Ǹ� �ȵǹǷ� �迭��ü�� �����ؼ� ������Ҹ� �����ѹ� �迭�� ������
	std::vector<uint8_t> CopyArray = Header; 
	if (Point < 0 || Point + 8 > CopyArray.size()) // Point�� ��ȿ���� Ȯ��
	{
		return false; // Point�� ���� ��ȿ���� ������ false ��ȯ
	}
	//Point���� 8���� �迭�� ����������� ������
	sort(CopyArray.begin() + Point, CopyArray.begin() + Point + 8, std::less<uint8_t>());
	//�����ѹ� �迭�� ���ؼ� 1~8�� �´ٸ� true, �ƴ϶�� false��ȯ
	return std::equal(CopyArray.begin() + Point, CopyArray.begin() + Point + 8, MagicNumberArray.begin());
}


//������ �Ľ� �� �ִܼ�����۸��� �̿��� ������ ���
void TI::PrintData()
{
	SetHeaderData();
	//offset�� TotalPacketlen���� ������ ��� ���ư���
	//(�ϳ��� ��Ŷ����� ���� �Ľ��Ҷ����� ����)
	while (offset < TotalPacketLen)
	{
		SetUARTData();
	}
	Header.erase(Header.begin(), Header.begin() + offset);
	ConsoleBufferPrint();
}


void TI::SetHeaderData()
{
	offset = 8; //����� �Ľ���ġ ������ ����� ����
	Point = 0;	//Header ��Ž���� ���� �ʱ�ȭ
	CharPrint("�ڡ�HEADER DATA�ڡ�", 0, LineStartPoint);
	ParsingDataPrint("Version", 0, LineStartPoint, "Version");
	TotalPacketLen = ParsingDataPrint("TotalPacketLen", 0, LineStartPoint, "int");
	ParsingDataPrint("Platform", 0, LineStartPoint, "int");
	ParsingDataPrint("FrameNumber", 0, LineStartPoint, "int");
	ParsingDataPrint("TimeCpuCycles", 0, LineStartPoint, "int");
	ParsingDataPrint("NumDetectedObj", 0, LineStartPoint, "int");
	ParsingDataPrint("NumTLVs", 0, LineStartPoint, "int");
	ParsingDataPrint("SubFrameNumber", 0, LineStartPoint, "int");

	
}


//Ĩ�� �Է��� ��ɾ�(CLI)�� Ÿ�Կ����� �����͸� �Ľ�����
void TI::SetUARTData()
{
	//�������� Ÿ�� ���� �Ľ�

	//Debug��
	int Type = (Header[offset] | Header[offset + 1] << 8 | Header[offset + 2] << 16 | Header[offset + 3] << 24);


	//�������� Ÿ�Կ� ���� ��µǴ� ������ ó��+
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
		CharPrint("----------------------------------------------------������ ����----------------------------------------------------", 20, LineStartPoint);
		CharPrint("20byte(xyzUnit,dopperUnit,snrUnit,noiseUnit,numDetectedPoints) + 10byte(x,y,z,doppler,snr,noise) X NumDetected Obj", 20, LineStartPoint);
		CharPrint("-------------------------------------------------------------------------------------------------------------------", 20, LineStartPoint);
		CharPrint("���Էµ� �������� Ÿ���� 'MMWDEMO_OUTPUT_EXT_MSG_DETECTED_POINTS' �Դϴ١� ", 40, LineStartPoint);
		LineStartPoint++;
		ParsingDataPrint("TypeNumber", MiddleX, LineStartPoint, "int");
		int Length = ParsingDataPrint("Length", MiddleX, LineStartPoint, "int");
	
		ParsingDataPrint("xyzUnit", MiddleX, LineStartPoint, "float");
		ParsingDataPrint("dopplerUnit", MiddleX, LineStartPoint, "float");
		ParsingDataPrint("snrUnit", MiddleX, LineStartPoint, "float");
		ParsingDataPrint("noiseUnit", MiddleX, LineStartPoint, "float");
		ParsingDataPrint("numDetectedPoint1", MiddleX, LineStartPoint, "int",2);
		ParsingDataPrint("numDetectedPoint2", MiddleX, LineStartPoint, "int",2);

		//point X������ �ڲ� ���ܼ����� ������۹����ε�
		ParsingDataPrint("PointX", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("PointY", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("PointZ", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("Doppler", MiddleX, LineStartPoint, "int", 2);
		ParsingDataPrint("snr", MiddleX, LineStartPoint, "int", 1);
		ParsingDataPrint("noise", MiddleX, LineStartPoint, "int", 1);
		LineStartPoint++;
		CharPrint("���� ���� �������� ��� ����Ʈ ���� ������ ����Ʈ Ŭ���� ���� ���� ���ؾ��մϴ�.", 38, LineStartPoint);
		CharPrint("EX) ���� ������ǥ : x * xyzUnit ", 60, LineStartPoint);
		LineStartPoint++;
		//�ϴ� ������ �Ѱܾ��ϴϱ� length��ŭ offset������--------------------------------------------------------------------------------------------------------------
		offset += Length - 30;
		break;
	}
	case TypeName::MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MAJOR:
	{
		int MiddleX = 70;
		CharPrint("----------------------------------------------------������ ����----------------------------------------------------", 20, LineStartPoint);
		CharPrint("----------------------------------------(Number Of Range Bins) x (4 Bytes)----------------------------------------", 20, LineStartPoint);
		CharPrint("-------------------------------------------------------------------------------------------------------------------", 20, LineStartPoint);
		CharPrint("���Էµ� �������� Ÿ���� 'MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MAJOR' �Դϴ١� ", 36, LineStartPoint);
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
		CharPrint("----------------------------------------------------������ ����----------------------------------------------------", 20, LineStartPoint);
		CharPrint("------------24 Bytes(InterFrame 4byte + transmitOutput 4byte + powerMeasured 8byte + tempReading 8byte)------------", 20, LineStartPoint);
		CharPrint("-------------------------------------------------------------------------------------------------------------------", 20, LineStartPoint);
		CharPrint("���Էµ� �������� Ÿ���� 'MMWDEMO_OUTPUT_EXT_MSG_STATS' �Դϴ١� ", 44, LineStartPoint);
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
	//offset�ʱ�ȭ
	offset = 8;
	
	Render();
	ScreenClear();
}


//���� ���� ȭ�� init���ִ� �Լ�
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
	LineStartPoint = 0; //�ܼ��� ��µǴ� ���� ����� ����
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
	//���� string�� ��ҹ��� ���о��� �빮�ڷ� ��ȯ����
	transform(_floatint.begin(), _floatint.end(), _floatint.begin(), ::toupper);

	char PrintInfo[64];

	if (_floatint == "FLOAT")
	{
		//float�� �ε��Ҽ��� �����̱⶧���� ��Ʈ���� ��� memcpy
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