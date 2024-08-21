#pragma once








class AdcFilter
{
public:
	AdcFilter();
	~AdcFilter();
	void AdcFilter_To_Float(std::vector<float>& Array, int Start = 0, int Adc_Filtering_Size = 5);
	void AdcFilter_To_Int();
protected:
private:
	float means = 0.0f;
};