#include "AdcFilter.h"
#include "DataClass.h"
#include <algorithm>
#include <numeric>
#include <vector>


AdcFilter::AdcFilter()
{

}

AdcFilter::~AdcFilter()
{

}


void AdcFilter::AdcFilter_To_Float(std::vector<float>& Array, int Start, int Adc_Filtering_Size)
{

	if (Adc_Filtering_Size < 2)
		return;

	if (Start > Array.size() - Adc_Filtering_Size)
		return;

	if (Adc_Filtering_Size > AdcFilterSize_MAX)
		return;

	for (int i = Array.size() - ((Adc_Filtering_Size - 1) * 2); i < Array.size() - Adc_Filtering_Size + 1; i++)
	{
		means = std::accumulate(Array.begin()+i, Array.begin()+i + Adc_Filtering_Size, 0.0f) / Adc_Filtering_Size;
		Array[i + Adc_Filtering_Size - 1] = means;
	}

	for (int j = Start; j < Array.size() - Adc_Filtering_Size + 1; j++)
	{
		means = std::accumulate(Array.begin()+j, Array.begin()+j + Adc_Filtering_Size, 0.0f) / Adc_Filtering_Size;
		Array[j] = means;
	}
}


void AdcFilter::AdcFilter_To_Int()
{

}
