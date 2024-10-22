#pragma once
#include <iostream>
#include <mutex>
#include <string>
#include <algorithm>
#include "serial/serial.h"
#include "Windows.h"
#include "DataClass.h"






enum class TypeName : int16_t
{
	None,
	MMWDEMO_OUTPUT_MSG_DETECTED_POINTS,
	MMWDEMO_OUTPUT_MSG_RANGE_PROFILE,
	MMWDEMO_OUTPUT_MSG_NOISE_PROFILE,
	MMWDEMO_OUTPUT_MSG_AZIMUT_STATIC_HEAT_MAP,
	MMWDEMO_OUTPUT_MSG_RANGE_DOPPLER_HEAT_MAP,
	MMWDEMO_OUTPUT_MSG_STATS,
	MMWDEMO_OUTPUT_MSG_DETECTED_POINTS_SIDE_INFO,
	MMWDEMO_OUTPUT_MSG_AZIMUT_ELEVATION_STATIC_HEAT_MAP,
	MMWDEMO_OUTPUT_MSG_TEMPERATURE_STATS,
	MMWDEMO_OUTPUT_EXT_MSG_DETECTED_POINTS = 301,
	MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MAJOR,
	MMWDEMO_OUTPUT_EXT_MSG_RANGE_PROFILE_MINOR,
	MMWDEMO_OUTPUT_EXT_MSG_RANGE_AZIMUT_HEAT_MAP_MAJOR,
	MMWDEMO_OUTPUT_EXT_MSG_RANGE_AZIMUT_HEAT_MAP_MINOR,
	MMWDEMO_OUTPUT_EXT_MSG_STATS,
	MMWDEMO_OUTPUT_EXT_MSG_TARGET_LIST = 308,
	MMWDEMO_OUTPUT_EXT_MSG_TARGET_INDEX,
	MMWDEMO_OUTPUT_EXT_MSG_MICRO_DOPPLER_RAW_DATA,
	MMWDEMO_OUTPUT_EXT_MSG_MICRO_DOPPLER_FEATURES,
	MMWDEMO_OUTPUT_EXT_MSG_RADAR_CUBE_MAJOR,
	MMWDEMO_OUTPUT_EXT_MSG_RADAR_CUBE_MINOR,
	MMWDEMO_OUTPUT_EXT_MSG_ENHANCED_PRESENCE_INDICATION = 315,
	MMWDEMO_OUTPUT_EXT_MSG_ADC_SAMPLES,
	MMWDEMO_OUTPUT_EXT_MSG_CLASSIFIER_INFO,
	MMWDEMO_OUTPUT_EXT_MSG_RX_CHAN_COMPENSATION_INFO,
	MMWDEMO_OUTPUT_MSG_SPHERICAL_POINTS = 1000,
	MMWDEMO_OUTPUT_MSG_TRACKERPROC_3D_TARGET_LIST =1010,
	MMWDEMO_OUTPUT_MSG_TRACKERPROC_TARGET_INDEX,
	MMWDEMO_OUTPUT_MSG_TRACKERPROC_TARGET_HEIGHT,
	MMWDEMO_OUTPUT_MSG_COMPRESSED_POINTS =1020,
	MMWDEMO_OUTPUT_MSG_PRESCENCE_INDICATION,
	MMWDEMO_OUTPUT_MSG_OCCUPANCY_STATE_MACHINE = 1030,
	MMWDEMO_OUTPUT_MSG_SURFACE_CLASSIFICATION_PROBABILITY,
	MMWDEMO_OUTPUT_MSG_GESTURE_FEATURES = 1050,
	MMWDEMO_OUTPUT_MSG_ANN_OP_PROB,
	MMWDEMO_OUTPUT_EXT_MSG_CAM_TRIGGERS = 3000,
};



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
	int SerialSize = 0;


	///////////////////////Header(MagicNumber)////////////////////////
	std::vector<uint8_t> Header;									//
	const std::vector<int> MagicNumberArray = { 1,2,3,4,5,6,7,8 };	//
	//////////////////////////////////////////////////////////////////

	/////////Header Data//////////
	size_t offset = 8;			//
	int Version = 0;			//
	int TotalPacketLen = 1000;	//
	int Platform = 0;			//
	int FrameNumber = 0;		//
	int TimeCpuCycles = 0;		//
	int NumDetectedObj = 0;		//
	int NumTLVs = 0;			//
	int SubFrameNumber = 0;		//
	//////////////////////////////


	//Multithread (기회되면 해야지)
	std::mutex mtx;
	std::condition_variable cv;
	bool Data_Ready = false;
	void Data_receiver();
	void Data_processor();



	bool FindHeader(int Point);
	void PrintData();
	void SetHeaderData();
	void ConsoleBufferPrint();
	void SetUARTData();
	float ParsingDataPrint(std::string _PrintData, int _posX, int _posY, std::string _floatint, int _bytesize = 4, float _multiply = 0)
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
			if(_multiply)
				sprintf(PrintInfo, (_PrintData + " : %d ").c_str(), BitData* _multiply);
			else
				sprintf(PrintInfo, (_PrintData + " : %d ").c_str(), BitData);
			offset += _bytesize;
			ScreenPrint(_posX, _posY, PrintInfo);
			return static_cast<int>(BitData);
		}
	}


	////////////////////Double Buffer Setting/////////////////////
	void ScreenInit();											//
	void ScreenFlipping();										//
	void ScreenClear();											//
	void ScreenRelease();										//
	void ScreenPrint(int x, int y, char* string);				//
	void Render();												//
	void Release();												//
	int g_numofFPS = 0;											//
	clock_t CurTime, OldTime;									//
	//////////////////////////////////////////////////////////////

	char* SerialSizeInfo = new char[PrintCharSize];
	char* VersionInfo = new char[PrintCharSize];
	char* TotalPacketLenInfo = new char[PrintCharSize];
	char* PlatformInfo = new char[PrintCharSize];
	char* FrameNumberInfo = new char[PrintCharSize];
	char* TimeCpuCyclesInfo = new char[PrintCharSize];
	char* NumDetectedObjInfo = new char[PrintCharSize];
	char* NumTLVsInfo = new char[PrintCharSize];
	char* SubFrameNumberInfo = new char[PrintCharSize];
};