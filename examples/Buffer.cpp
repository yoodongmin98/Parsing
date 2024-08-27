#include <iostream>
#include <algorithm>
#include <numeric>

#include "Buffer.h"
#include "DataClass.h"
#include "AdcFilter.h"
#include "MyFFT.h"

#include "kiss_fft.h"

Buffer* Buffer::Buffers;

Buffer::Buffer()
{
	Buffers = this;
	MDRRadar_Datas = std::make_shared<MDRRadar_Data>();
	D_SimpleResult = std::make_shared<DopplerSimpleResult>();
	AdcFilters = std::make_shared<AdcFilter>();
	MyFFTs = std::make_shared<MyFFT>();
	MDR_Mag.resize(MDR_ADC_SampleSize / 2);

}

Buffer::~Buffer()
{
	
}


void Buffer::Fill_Adc_raw_buf(uint8_t __byte)
{
	//Adc_raw_buf[0] = Adc_raw_buf[3]; //완성되기전에는 주석 // 왜 하는건지는 모르겠구요...
	Adc_raw_buf.emplace_back(__byte);
}


void Buffer::DataClassify()
{
	//Reset
	if (!MDR_I_Classify.empty() || !MDR_Q_Classify.empty())
	{
		MDR_I_Classify.clear();
		MDR_Q_Classify.clear();
	}

	for (auto i = 1; i < 4096; i+=4)
	{
		MDR_I_Classify.push_back(Adc_raw_buf[i]);
		MDR_Q_Classify.push_back(Adc_raw_buf[i+2]);
	}
	Adc_raw_buf.clear();
	return;
}


void Buffer::Kill_Offset()
{

	//Reset
	if (!MDR_I_Doppler.empty() || !MDR_Q_Doppler.empty())
	{
		MDR_I_Doppler.clear();
		MDR_Q_Doppler.clear();
	}
	
	//Doppler I
	i_mean = std::accumulate(MDR_I_Classify.begin(), MDR_I_Classify.end(), 0.0f) / MDR_I_Classify.size();
	//Kills the offset
	for (auto V : MDR_I_Classify)
	{
		V -= i_mean;
		MDR_I_Doppler.emplace_back(V);
	}
		

	//Doppler Q
	i_mean = std::accumulate(MDR_Q_Classify.begin(), MDR_Q_Classify.end(), 0.0f) / MDR_Q_Classify.size();
	//Kills the offset
	for (auto V : MDR_Q_Classify)
	{
		V -= i_mean;
		MDR_Q_Doppler.emplace_back(V);
	}
}


void Buffer::WindowFunction()
{
	//Reset
	if (!MDR_I_Window.empty() || !MDR_Q_Window.empty())
	{
		MDR_I_Window.clear();
		MDR_Q_Window.clear();
	}
	//효율 따위는 조상님 줘버린 코드
	MDR_I_Window = MDR_I_Doppler;
	MDR_Q_Window = MDR_Q_Doppler;

	//Hamming Window	
	for (auto i = 0; i < static_cast<int>(Temp_Side_Idx); ++i)
	{
		//MDR_I_raw[i] *= i / Temp_Side_i;
		//MDR_Q_raw[i] *= i / Temp_Side_i;
		MDR_I_Window[i] *= i / Temp_Side_Idx;
		MDR_Q_Window[i] *= i / Temp_Side_Idx;
	}
	for (auto p = MDR_ADC_SampleSize - static_cast<int>(Temp_Side_Idx); p < MDR_ADC_SampleSize; ++p)
	{
		//MDR_I_raw[p] *= (MDR_ADC_SampleSize - p - 1) / Temp_Side_i;
		//MDR_Q_raw[p] *= (MDR_ADC_SampleSize - p - 1) / Temp_Side_i;
		MDR_I_Window[p] *= (MDR_ADC_SampleSize - p - 1) / Temp_Side_Idx;
		MDR_Q_Window[p] *= (MDR_ADC_SampleSize - p - 1) / Temp_Side_Idx;
	}
	int DenugValue = 0;
}


void Buffer::FastFurierTransform(int Reverse)
{
	//FFT
	if (!Reverse)
	{
		//Reset
		if (!MDR_I_FFT.empty() || !MDR_Q_FFT.empty())
		{
			MDR_I_FFT.clear();
			MDR_Q_FFT.clear();
		}
		//std::vector<kiss_fft_cpx> input(MDR_ADC_SampleSize);
		//std::vector<kiss_fft_cpx> output(MDR_ADC_SampleSize);
		//kiss_fft_cfg cfg = kiss_fft_alloc(MDR_ADC_SampleSize, 0, nullptr, nullptr);

		//for (int i = 0; i < MDR_ADC_SampleSize; ++i) 
		//{
		//	input[i].r = MDR_I_Window[i];  // 실수부에 I 데이터 설정
		//	input[i].i = MDR_Q_Window[i];  // 허수부에 Q 데이터 설정
		//}
		//kiss_fft(cfg, input.data(), output.data());
		//for (int j = 0; j < MDR_ADC_SampleSize; ++j)
		//{
		//	MDR_I_FFT.emplace_back(output[j].r);
		//	MDR_Q_FFT.emplace_back(output[j].i);
		//}
		MDR_I_FFT = MDR_I_Window;
		MDR_Q_FFT = MDR_Q_Window;

		MyFFTs->FFTInit(MDR_I_FFT, MDR_Q_FFT);
		std::vector<std::complex<double>> Result = MyFFTs->Foward_FFT();
		for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
		{
			MDR_I_FFT[i] = Result[i].real();
			MDR_Q_FFT[i] = Result[i].imag();
		}
	}
	else
	{
		if (!MDR_I_RFFT.empty() || !MDR_Q_RFFT.empty())
		{
			MDR_I_RFFT.clear();
			MDR_Q_RFFT.clear();
		}

		MDR_I_RFFT = MDR_I_HLCUT;
		MDR_Q_RFFT = MDR_Q_HLCUT;

		MyFFTs->FFTInit(MDR_I_RFFT, MDR_Q_RFFT);
		std::vector<std::complex<double>> Result = MyFFTs->Reverse_FFT();
		for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
		{
			MDR_I_RFFT[i] = Result[i].real();
			MDR_Q_RFFT[i] = Result[i].imag();
		}
		//std::vector<kiss_fft_cpx> input(MDR_ADC_SampleSize);
		//std::vector<kiss_fft_cpx> output(MDR_ADC_SampleSize);

		//kiss_fft_cfg cfg = kiss_fft_alloc(MDR_ADC_SampleSize, 1, nullptr, nullptr);  // 1이 reverse
		//for (int i = 0; i < MDR_ADC_SampleSize; ++i)
		//{
		//	input[i].r = MDR_I_HLCUT[i];  // 실수부에 I 데이터 설정
		//	input[i].i = MDR_Q_HLCUT[i];  // 이하 동문
		//}
		//kiss_fft(cfg, input.data(), output.data());
		//for (int j = 0; j < MDR_ADC_SampleSize; ++j)
		//{
		//	//일단 덮어쓰기
		//	MDR_I_RFFT.emplace_back(output[j].r);
		//	MDR_Q_RFFT.emplace_back(output[j].i);
		//}
	}
	return;
}


void Buffer::Kill_DC_POINT()
{
	//Reset
	if (!MDR_I_DCPOINT.empty() || !MDR_Q_DCPOINT.empty())
	{
		MDR_I_DCPOINT.clear();
		MDR_Q_DCPOINT.clear();
	}

	//역시 효율따위 조상님 준 코드가 최고야
	MDR_I_DCPOINT = MDR_I_FFT;
	MDR_Q_DCPOINT = MDR_Q_FFT;
	//Kill DC
	//무언가 조건에 의해서 DC성분을 제거하는거같은데
	std::fill(MDR_I_DCPOINT.begin(), MDR_I_DCPOINT.begin() + 8, 0.0f);
	std::fill(MDR_Q_DCPOINT.begin(), MDR_Q_DCPOINT.begin() + 8, 0.0f);

	//Kill Point
	if (MDRRadar_Datas->FreqPointCutEnable)
	{
		//얘는 기본값만 가지고있고, 변하는 기준을 모르니 일단 패스
		int i = MDRRadar_Datas->FreqPointCut;
		std::fill(MDR_I_DCPOINT.begin() + i, MDR_I_DCPOINT.begin() + i + 12, 0.0f);
		std::fill(MDR_Q_DCPOINT.begin() + i, MDR_Q_DCPOINT.begin() + i + 12, 0.0f);
	}
}


void Buffer::HighLowCut()
{
	//원하는 주파수의 성분만 남기는것.(어차피 대칭되는 데이터이므로 한쪽만 사용하겠다는거)
	if (!MDR_I_HLCUT.empty() || !MDR_Q_HLCUT.empty())
	{
		MDR_I_HLCUT.clear();
		MDR_Q_HLCUT.clear();
	}
	MDR_I_HLCUT = MDR_I_DCPOINT;
	MDR_Q_HLCUT = MDR_Q_DCPOINT;
	////Low Cut
	//if (MDRRadar_Datas->FreqLowCutEnable)
	//{
	//	std::fill(MDR_I_HLCUT.begin(), MDR_I_HLCUT.begin() + (uint16_t)(MDRRadar_Datas->FreqLowCut / D_SimpleResult->FreqByBin + 1) * 2, 0.0f);
	//	std::fill(MDR_Q_HLCUT.begin(), MDR_Q_HLCUT.begin() + (uint16_t)(MDRRadar_Datas->FreqLowCut / D_SimpleResult->FreqByBin + 1) * 2, 0.0f);
	//}

	//// High Cut
	//if (MDRRadar_Datas->FreqHighCutEnable)
	//{
	//	std::fill(MDR_I_HLCUT.begin() + (uint16_t)(MDRRadar_Datas->FreqHighCut / D_SimpleResult->FreqByBin + 1) * 2,
	//		MDR_I_HLCUT.begin() + MDR_I_HLCUT.size(),
	//		0.0f);
	//	std::fill(MDR_Q_HLCUT.begin() + (uint16_t)(MDRRadar_Datas->FreqHighCut / D_SimpleResult->FreqByBin + 1) * 2,
	//		MDR_Q_HLCUT.begin() + MDR_Q_HLCUT.size(),
	//		0.0f);
	//}
	/*std::fill(MDR_Q_HLCUT.begin() + MDR_Q_HLCUT.size()/2,
		MDR_Q_HLCUT.begin() + MDR_Q_HLCUT.size(),
		0.0f);
	std::fill(MDR_I_HLCUT.begin() + MDR_I_HLCUT.size() / 2,
		MDR_I_HLCUT.begin() + MDR_I_HLCUT.size(),
		0.0f);*/
}


void Buffer::CalculateMagnitude() 
{
	//크기 구하기
	if (!MDR_Mag.empty())
	{
		MDR_Mag.clear();
		MDR_Mag.resize(MDR_ADC_SampleSize / 2);
	}
	for (int i = 0; i < MDR_ADC_SampleSize/2; i++) {
		float real = MDR_I_RFFT[i];   // 실수부
		float imag = MDR_Q_RFFT[i];   // 허수부
		MDR_Mag[i] = sqrtf(real * real + imag * imag);
	}
}


void Buffer::BinFreqSpeedCalculate() //??
{
	if (!MDR_I_BinFreq.empty())
	{
		MDR_I_BinFreq.clear();
	}
	MDR_I_BinFreq = MDR_Mag;

	for (auto i = 0; i < DopplerObjectNum; ++i)
	{
		auto maxIter = max_element(MDR_I_BinFreq.begin(), MDR_I_BinFreq.end());
		//최대 값 담기
		D_SimpleResult->Value[i] = *maxIter;
		//최대 값의 idx
 		D_SimpleResult->Bin[i] = distance(MDR_I_BinFreq.begin(), maxIter);
		//그 값을 0으로 만듬
		MDR_I_BinFreq[D_SimpleResult->Bin[i]] = 0;

		//주파수 구하기
		D_SimpleResult->Freq[i] = D_SimpleResult->FreqByBin * D_SimpleResult->Bin[i];
		//속도 구하기
		D_SimpleResult->Speed[i] = D_SimpleResult->Freq[i] / D_SimpleResult->Freq_1Kmh;
	}
}


void Buffer::Phase()
{
	if (!MDR_I_Phase.empty() || !MDR_Q_Phase.empty())
	{
		MDR_I_Phase.clear();
		MDR_Q_Phase.clear();
	}
	MDR_I_Phase = MDR_I_RFFT;
	MDR_Q_Phase = MDR_Q_RFFT;
	for (auto i = 0; i < DopplerObjectNum; ++i)
	{
		D_SimpleResult->PhaseRadian_I[i] = atan2f(MDR_I_Phase[D_SimpleResult->Bin[i] * 2 + 1], MDR_I_Phase[D_SimpleResult->Bin[i] * 2]); //복소수 형태로 표현
		D_SimpleResult->PhaseRadian_Q[i] = atan2f(MDR_Q_Phase[D_SimpleResult->Bin[i] * 2 + 1], MDR_Q_Phase[D_SimpleResult->Bin[i] * 2]);

		//Phase예각 구하기
		if (D_SimpleResult->PhaseRadian_I[i] >= 0)
		{
			D_SimpleResult->PhaseRadianDiff[i] = D_SimpleResult->PhaseRadian_I[i] - D_SimpleResult->PhaseRadian_Q[i];

			if (D_SimpleResult->PhaseRadianDiff[i] > PI)
				D_SimpleResult->PhaseRadianDiff[i] -= (PI * 2);
		}
		else
		{
			D_SimpleResult->PhaseRadianDiff[i] = -(D_SimpleResult->PhaseRadian_Q[i] - D_SimpleResult->PhaseRadian_I[i]);

			if (D_SimpleResult->PhaseRadianDiff[i] < -PI)
				D_SimpleResult->PhaseRadianDiff[i] += (PI * 2);
		}
		// PhaseDegree 구하기
		D_SimpleResult->PhaseDegree_I[i] = (int32_t)(D_SimpleResult->PhaseRadian_I[i] * 180 / PI);
		D_SimpleResult->PhaseDegree_Q[i] = (int32_t)(D_SimpleResult->PhaseRadian_Q[i] * 180 / PI);

		// PhaseDegreeDiff 구하기
		if (D_SimpleResult->PhaseDegree_I[i] >= 0)
		{
			D_SimpleResult->PhaseDegreeDiff[i] = D_SimpleResult->PhaseDegree_I[i] - D_SimpleResult->PhaseDegree_Q[i];

			if (D_SimpleResult->PhaseDegreeDiff[i] > 180)
				D_SimpleResult->PhaseDegreeDiff[i] -= 360;
		}
		else
		{
			D_SimpleResult->PhaseDegreeDiff[i] = -(D_SimpleResult->PhaseDegree_Q[i] - D_SimpleResult->PhaseDegree_I[i]);

			if (D_SimpleResult->PhaseDegreeDiff[i] < -180)
				D_SimpleResult->PhaseDegreeDiff[i] += 360;
		}
		// Direction 구하기
		if (D_SimpleResult->PhaseDegreeDiff[i] > 70 && D_SimpleResult->PhaseDegreeDiff[i] < 110)
			D_SimpleResult->Direction[i] = EnumDirection::Faraway;
		else if (D_SimpleResult->PhaseDegreeDiff[i] < -70 && D_SimpleResult->PhaseDegreeDiff[i] > -110)
			D_SimpleResult->Direction[i] = EnumDirection::Oncoming;
		else
			D_SimpleResult->Direction[i] = EnumDirection::Unknown;
	}


	// Accuracy 계산
	MDRRadar_Datas->Accuracy = 0;
	for (uint8_t i = 0; i < DopplerObjectNum; i++)
		MDRRadar_Datas->Accuracy += static_cast<int>(D_SimpleResult->Direction[i]);

	// 평균속도 구하기
	MDRRadar_Datas->Speed = 0;
	for (uint8_t idx = 0; idx < 4; idx++)
		MDRRadar_Datas->Speed += D_SimpleResult->Speed[idx];

	if (MDRRadar_Datas->Accuracy > 0)
		MDRRadar_Datas->Speed /= 4.f;
	else if (MDRRadar_Datas->Accuracy < 0)
		MDRRadar_Datas->Speed /= -4.f;
	else
		MDRRadar_Datas->Speed = 0;


	// Speed에 방향적용
	for (uint16_t i = 0; i < DopplerObjectNum; i++)
	{
		D_SimpleResult->Direction[i] == EnumDirection::Oncoming ? D_SimpleResult->Speed[i] *= -1 : 1;
		D_SimpleResult->Direction[i] == EnumDirection::Unknown ? D_SimpleResult->Speed[i] *= 0 : 1;
	}
}


void Buffer::Filtering()
{
	if (!MDR_I_Filter.empty() || !MDR_Q_Filter.empty())
	{
		MDR_I_Filter.clear();
		MDR_Q_Filter.clear();
	}
	MDR_I_Filter = MDR_I_RFFT;
	MDR_Q_Filter = MDR_Q_RFFT;


	if (MDRRadar_Datas->AdcFillterEnable_float32)
	{
		AdcFilters->AdcFilter_To_Float(MDR_I_Filter);
		AdcFilters->AdcFilter_To_Float(MDR_I_Filter);
		AdcFilters->AdcFilter_To_Float(MDR_Q_Filter);
		AdcFilters->AdcFilter_To_Float(MDR_Q_Filter);
	}
}


void Buffer::IQPower_LPF()
{
	
	if (!MDR_I_LPF.empty() || !MDR_Q_LPF.empty())
	{
		MDR_I_LPF.clear();
		MDR_Q_LPF.clear();
		LPF_Temp_I = 0;
		LPF_Temp_Q = 0;
	}
	MDR_I_LPF = MDR_I_Filter;
	MDR_Q_LPF = MDR_Q_Filter;
	for (int i = 0; i < MDR_ADC_SampleSize; i++)
	{
		MDR_I_LPF[i] = MDRRadar_Datas->LPF_Coefficient * LPF_Temp_I + (1 - MDRRadar_Datas->LPF_Coefficient) * MDR_I_LPF[i];
		LPF_Temp_I = MDR_I_LPF[i];
		MDR_Q_LPF[i] = MDRRadar_Datas->LPF_Coefficient * LPF_Temp_Q + (1 - MDRRadar_Datas->LPF_Coefficient) * MDR_Q_LPF[i];
		LPF_Temp_Q = MDR_Q_LPF[i];
	}
}


void Buffer::ScailingAndAlgorithm()
{
	if (!MDR_I_Scale.empty() || !MDR_Q_Scale.empty())
	{
		MDR_I_Scale.clear();
		MDR_Q_Scale.clear();
		
	}
	MDR_I_Scale = MDR_I_LPF;
	MDR_Q_Scale = MDR_Q_LPF;

	//Scailing
	for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
	{
		MDR_I_Scale[i] *= MDRRadar_Datas->AmplificationValue;
		MDR_Q_Scale[i] *= MDRRadar_Datas->AmplificationValue;
	}

	if (!MDRRadar_Datas->Algorithm)
	{
		i_mean = std::accumulate(MDR_I_Scale.begin(), MDR_I_Scale.end(), 0.0f) / MDR_I_Scale.size();
		for (auto& V : MDR_I_Scale)
			V = abs(V - i_mean);

		i_mean = std::accumulate(MDR_Q_Scale.begin(), MDR_Q_Scale.end(), 0.0f) / MDR_Q_Scale.size();
		for (auto& V : MDR_Q_Scale)
			V = abs(V - i_mean);
	}
}