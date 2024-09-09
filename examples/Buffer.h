#pragma once
#include <vector>
#include <iostream>

class MyFFT;
class AdcFilter;
class Calculator;
class MDRRadar_Data;
class DopplerSimpleResult;
class Buffer
{
	friend Calculator;
public:
	static Buffer* Buffers;
	Buffer();
	~Buffer();


	inline const size_t GetAdc_Size()
	{
		return Adc_raw_buf.size();
	}
	inline std::shared_ptr<DopplerSimpleResult> GetDopplerSimpleResult()
	{
		return D_SimpleResult;
	}
	inline std::shared_ptr<MDRRadar_Data> GetMDRRader()
	{
		return MDRRadar_Datas;
	}


	//////////////////////Get Vector//////////////////////

	//Classify
	inline const std::vector<float>& GetMDR_I_Classify()
	{
		return MDR_I_Classify;
	}
	inline const std::vector<float>& GetMDR_Q_Classify()
	{
		return MDR_Q_Classify;
	}
	//Doppler
	inline const std::vector<float>& GetMDR_I_Doppler()
	{
		return MDR_I_Doppler;
	}
	inline const std::vector<float>& GetMDR_Q_Doppler()
	{
		return MDR_Q_Doppler;
	}
	//Window
	inline const std::vector<float>& GetMDR_I_Window()
	{
		return MDR_I_Window;
	}
	inline const std::vector<float>& GetMDR_Q_Window()
	{
		return MDR_Q_Window;
	}
	//FFT
	inline const std::vector<float>& GetMDR_I_FFT()
	{
		return MDR_I_FFT;
	}
	inline const std::vector<float>& GetMDR_Q_FFT()
	{
		return MDR_Q_FFT;
	}
	//Kill DC, POINT
	inline const std::vector<float>& GetMDR_I_DCPOINT()
	{
		return MDR_I_DCPOINT;
	}
	inline const std::vector<float>& GetMDR_Q_DCPOINT()
	{
		return MDR_Q_DCPOINT;
	}
	//High Low Cut
	inline const std::vector<float>& GetMDR_I_HLCUT()
	{
		return MDR_I_HLCUT;
	}
	inline const std::vector<float>& GetMDR_Q_HLCUT()
	{
		return MDR_Q_HLCUT;
	}
	//RFFT
	inline const std::vector<float>& GetMDR_I_RFFT()
	{
		return MDR_I_RFFT;
	}
	inline const std::vector<float>& GetMDR_Q_RFFT()
	{
		return MDR_Q_RFFT;
	}
	//magnitude
	inline const std::vector<float>& GetMDR_I_Mag()
	{
		return MDR_Mag;
	}
	//BinFreqSpeed
	inline const std::vector<float>& GetMDR_BFS()
	{
		return MDR_I_BinFreq;
	}
	//Phase
	inline const std::vector<float>& GetMDR_I_Phase()
	{
		return MDR_I_Phase;
	}
	inline const std::vector<float>& GetMDR_Q_Phase()
	{
		return MDR_Q_Phase;
	}
	//Filtering
	inline const std::vector<float>& GetMDR_I_Filter()
	{
		return MDR_I_Filter;
	}
	inline const std::vector<float>& GetMDR_Q_Filter()
	{
		return MDR_Q_Filter;
	}
	//LPF
	inline const std::vector<float>& GetMDR_I_LPF()
	{
		return MDR_I_LPF;
	}
	inline const std::vector<float>& GetMDR_Q_LPF()
	{
		return MDR_Q_LPF;
	}
	//Scailing
	inline const std::vector<float>& GetMDR_I_Scale()
	{
		return MDR_I_Scale;
	}
	inline const std::vector<float>& GetMDR_Q_Scale()
	{
		return MDR_Q_Scale;
	}
protected:
	//Calculate Funtion
	void Fill_Adc_raw_buf(uint8_t __byte);
	void DataClassify();
	void Kill_Offset();
	void WindowFunction();
	void FastFurierTransform(int Reverse = 0);
	void Kill_DC_POINT();
	void HighLowCut();
	void CalculateMagnitude();
	void BinFreqSpeedCalculate();
	void Phase();
	void Filtering();
	void IQPower_LPF();
	void ScailingAndAlgorithm();
private:

	float i_mean = 0.0f;
	float Temp_Side_Idx = 100.0f;
	float LPF_Temp_I = 0;
	float LPF_Temp_Q = 0;

	std::vector<uint8_t> Adc_raw_buf;

	std::vector<float> MDR_I_Classify;
	std::vector<float> MDR_Q_Classify;


	std::vector<float> MDR_I_Doppler;
	std::vector<float> MDR_Q_Doppler;

	std::vector<float> MDR_I_Window;
	std::vector<float> MDR_Q_Window;

	std::vector<float> MDR_I_FFT;
	std::vector<float> MDR_Q_FFT;

	std::vector<float> MDR_I_DCPOINT;
	std::vector<float> MDR_Q_DCPOINT;

	std::vector<float> MDR_I_HLCUT;
	std::vector<float> MDR_Q_HLCUT;

	std::vector<float> MDR_I_RFFT;
	std::vector<float> MDR_Q_RFFT;

	std::vector<float> MDR_Mag;

	std::vector<float> MDR_I_BinFreq;
	
	std::vector<float> MDR_I_Phase; //Phase도 I와Q를 합친 하나의 배열로 표현
	std::vector<float> MDR_Q_Phase;

	std::vector<float> MDR_I_Filter;
	std::vector<float> MDR_Q_Filter;

	std::vector<float> MDR_I_LPF;
	std::vector<float> MDR_Q_LPF;

	std::vector<float> MDR_I_Scale;
	std::vector<float> MDR_Q_Scale;

	std::shared_ptr<MDRRadar_Data> MDRRadar_Datas = nullptr;
	std::shared_ptr<DopplerSimpleResult> D_SimpleResult = nullptr;
	std::shared_ptr<AdcFilter> AdcFilters = nullptr;
	std::shared_ptr<MyFFT> MyFFTs = nullptr;
};