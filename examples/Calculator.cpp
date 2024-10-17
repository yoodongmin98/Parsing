#include "Buffer.h"
#include "Calculator.h"
#include "serial/serial.h"
#include "Core.h"
#include <cstdint>

Calculator::Calculator()
{

}
Calculator::~Calculator()
{

}





void Calculator::CalCulate()
{
	serial::Serial my_serial(Core::Cores->GetPortNumber(), Core::Cores->GetBaudrate(), serial::Timeout::simpleTimeout(1000));
	for (auto i = 0; i < 4096; ++i)
	{
		my_serial.read(&byte,1);
		Buffer::Buffers->Fill_Adc_raw_buf(byte); // Fill Adc_raw_buf
	}
	Buffer::Buffers->DataClassify(); // I,Q Classify
	Buffer::Buffers->Kill_Offset(); // Doppler I,Q && Kill offset
	Buffer::Buffers->WindowFunction(); // Apply window function
	Buffer::Buffers->FastFurierTransform(); // Apply FFT <enter 1 as the argument = reverse fft>
	Buffer::Buffers->Kill_DC_POINT(); //Kill Dc And Point
	Buffer::Buffers->HighLowCut(); //High,Low Data Cut
	Buffer::Buffers->FastFurierTransform(1); // Apply FFT <enter 1 as the argument = reverse fft>
	Buffer::Buffers->CalculateMagnitude(); //Apply Magnitude(Vector Size Calculate)
	Buffer::Buffers->BinFreqSpeedCalculate(); //Bin , Freq , Speed
	Buffer::Buffers->Phase(); //Divide Phase
	Buffer::Buffers->Filtering(); //Filtering Graph
	Buffer::Buffers->IQPower_LPF(); //Get Realtime_IQ_Power_LPF_Buf
	Buffer::Buffers->ScailingAndAlgorithm(); //Scailing And Apply Integral Algorithm 
}