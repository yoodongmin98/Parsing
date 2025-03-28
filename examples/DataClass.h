#pragma once
#include <cstdint>
#include <vector>



#define MDR_ADC_SampleSize 1024
#define MDR_ADC_ChannelSize 3 
#define DATACOUNT MDR_ADC_ChannelSize
#define DopplerObjectNum 20
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679
#define AdcFilterSize_MAX 9
#define AdcFilterSize_MIN 1
#define MaxFFTSize 4096
#define MagicNumberSize 8
#define PrintCharSize 64



class Buffer;
class MyImGui;
enum class EnumDirection
{
	Unknown = (0),
	Faraway = (1),
	Oncoming = (-1)
};

class MDRRadar_Data final
{

public:
	friend Buffer;
	friend MyImGui;
private:
	uint32_t
		VtuneAllways,					// 'D' is OFF, 'E' is ON
		VtuneApply;						// 'D' is OFF, 'E' is ON

	uint32_t
		DetectLevel, 				//5000~999999(ms)
		HoldTime,				//500~999999(ms)
		MotionDataCheckEnable,			// ON:0x01, OFF:0x00

		AdcFillterSize_uint16,		// 0~4095
		AdcFillterSize_float32,		// 0~4095
		AdcFillterEnable_uint16,	// 'D' is OFF, 'E' is ON
		AdcFillterEnable_float32 = 1,	// 'D' is OFF, 'E' is ON
		VcoDac_Use_VPTAP;			// 'D' is OFF, 'E' is ON
	int32_t	VcoDac_VPTAP_Offset;		// 0~4095
	uint32_t
		VcoDac_VPTAP,				// 0~4095
		VcoDac_MMIC_DIV;			// 0~4095

	float
		IF_I_Integral,
		IF_Q_Integral,
		IF_I_Integral_Lowpass,
		IF_Q_Integral_Lowpass;
	float	Speed;
	float	Accuracy;
	float	AmplificationValue = 2.5f;
	float	LPF_Coefficient = 0.93f;
	uint32_t	DebugCheckEnable;			// ON:0x01, OFF:0x00
	float	FreqLowCut = 30.0f;
	uint32_t	FreqLowCutEnable = 1;		// ON:0x01, OFF:0x00
	float	FreqHighCut = 2300.0f;
	uint32_t	FreqHighCutEnable = 1;		// ON:0x01, OFF:0x00
	uint32_t	GPIO_Mode;					// Default:0x00, Mode1:0x01
	uint32_t	FreqPointCut = 24;			
	uint32_t	FreqPointCutEnable = 1;		// ON:0x01, OFF:0x00
	uint32_t	Algorithm;					// (Integral 0, Variance 1);
	uint32_t	GPIO_DirectionHoldingNum;	// Default:8
	uint32_t	GPIO_Reverse;	// Default:0
	uint32_t	IntermittentNoiseThreshold;	// Default:0
};


class DopplerSimpleResult final
{
public:
	//초기화는 먼 미래에 알게되면 시켜줄게...
	DopplerSimpleResult()
	{
		Speed.resize(DopplerObjectNum);
		Value.resize(DopplerObjectNum);
		Freq.resize(DopplerObjectNum);
		Bin.resize(DopplerObjectNum);
	}
	friend Buffer;

	float
		//알고보니 5Ghz였고요
		FreqByBin = 4.8828125f, //  5GHz (샘플링 주파수) / 1024 (샘플링 데이터 개수) 
		// 주파수를 특정속도와 대응시키는 값 (mini 기준 avg값인 24.15 기준으로 계산된 값)
		Freq_1Kmh = 44.72222222222216f;
	uint32_t
		BiausBin = 1,
		CalcBinNums = 500;
		std::vector<float> Bin;
		std::vector<float> Speed;
		std::vector<float> Value;
		std::vector<float> Freq;
	float
		PhaseRadian_I[DopplerObjectNum],
		PhaseRadian_Q[DopplerObjectNum],
		PhaseRadianDiff[DopplerObjectNum];
	int32_t
		PhaseDegree_I[DopplerObjectNum],
		PhaseDegree_Q[DopplerObjectNum],
		PhaseDegreeDiff[DopplerObjectNum];
	EnumDirection
		Direction[DopplerObjectNum];
};