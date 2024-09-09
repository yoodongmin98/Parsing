#include <cstdint>
#include "Calculator.h"
#include "Buffer.h"


Calculator::Calculator()
{

}
Calculator::~Calculator()
{

}





void Calculator::CalCulate(uint8_t _byte)
{
	Buffer::Buffers->Fill_Adc_raw_buf(_byte); // Fill Adc_raw_buf
	if (Buffer::Buffers->GetAdc_Size() == 4096)
	{
		Buffer::Buffers->DataClassify(); // I,Q Classify
		Buffer::Buffers->Kill_Offset(); // Doppler I,Q && Kill offset
		Buffer::Buffers->WindowFunction(); // Apply window function
		Buffer::Buffers->FastFurierTransform(); // Apply FFT <enter 1 as the argument = reverse fft>
		Buffer::Buffers->Kill_DC_POINT(); //Kill Dc And Point
		Buffer::Buffers->HighLowCut(); //High,Low Data Cut
		Buffer::Buffers->FastFurierTransform(1); // Apply FFT <enter 1 as the argument = reverse fft>
		Buffer::Buffers->CalculateMagnitude(); //Apply Magnitude(Vector Size Calculate)
		Buffer::Buffers->BinFreqSpeedCalculate(); //Bin , Freq , Speed
		//Buffer::Buffers->Phase(); //Divide Phase
		Buffer::Buffers->Filtering(); //Filtering Graph
		Buffer::Buffers->IQPower_LPF(); //Get Realtime_IQ_Power_LPF_Buf
		Buffer::Buffers->ScailingAndAlgorithm(); //Scailing And Apply Integral Algorithm 
	}


	
}