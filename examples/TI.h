#pragma once
#include <iostream>
#include <mutex>
#include "serial/serial.h"
#include "Windows.h"






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
	int TotalPacketLen = 0;		//
	int Platform = 0;			//
	int FrameNumber = 0;		//
	int TimeCpuCycles = 0;		//
	int NumDetectedObj = 0;		//
	int NumTLVs = 0;			//
	int SubFrameNumber = 0;		//
	//////////////////////////////
	

	//Multithread
	std::mutex mtx;
	std::condition_variable cv;
	bool Data_Ready = false;
	void Data_receiver();
	void Data_processor();



	bool FindHeader(int& Point);
	void PrintData();
	void SetHeaderData();
	void ConsoleBufferPrint();
	void SetUARTData();


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

	char* SerialSizeInfo = new char[128];
	char* VersionInfo = new char[128];
	char* TotalPacketLenInfo = new char[128];
	char* PlatformInfo = new char[128];
	char* FrameNumberInfo = new char[128];
	char* TimeCpuCyclesInfo = new char[128];
	char* NumDetectedObjInfo = new char[128];
	char* NumTLVsInfo = new char[128];
	char* SubFrameNumberInfo = new char[128];

};