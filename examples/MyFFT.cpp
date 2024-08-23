#include "MyFFT.h"
#include "DataClass.h"







MyFFT::MyFFT()
{
	CalculateBasisFunction();
}

MyFFT::~MyFFT()
{

}



void MyFFT::Foward_FFT(std::vector<float>& _IArray, std::vector<float>& _QArray)
{
	//FFT
	try
	{
		std::vector<std::complex<double>> CalculateArray(MDR_ADC_SampleSize);
		for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
		{
			// 복소수 객체를 생성하고 실수부와 허수부를 설정
			CalculateArray[i] = std::complex<double>(_IArray[i], _QArray[i]);
		}
		if (_IArray.size() == _QArray.size())
			Cooley_Tukey_FFT(CalculateArray);
		else
			throw;
	}
	catch (int n)
	{
		//DFT
		Discrete_Fourier_Transform(_IArray, _QArray);
	}
}


void MyFFT::Reverse_FFT(std::vector<float>& _IArray, std::vector<float>& _QArray)
{
	
}


void MyFFT::CalculateBasisFunction()
{
	//기저함수 계산
	BasisFunctionData.resize(MDR_ADC_SampleSize);
	std::complex<double> Basis(0, 1);

	for (auto i=0; i< MDR_ADC_SampleSize; ++i)
	{
		for (auto j = 0; j < MDR_ADC_SampleSize; ++j)
		{
			//i번째 index에 대한 j(1024)개의 기저함수
			double DotProduct = -2.0 * PI * i * j / 1024;
			std::complex<double> exponent = Basis * DotProduct;
			BasisFunctionData[i].push_back(std::exp(exponent));
		}
	}
}


void MyFFT::Discrete_Fourier_Transform(std::vector<float>& _IArray, std::vector<float>& _QArray)
{
	//FFT구현(일단 DFT)
	std::vector<float> IResult(MDR_ADC_SampleSize, 0);
	std::vector<float> QResult(MDR_ADC_SampleSize, 0);

	//각 주파수에 대한 기저함수 내적 합
	for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
	{
		for (auto k = 0; k < MDR_ADC_SampleSize; ++k)
		{
			IResult[i] += _IArray[k] * BasisFunctionData[i][k].real() - _QArray[k] * BasisFunctionData[i][k].imag();
			QResult[i] += _IArray[k] * BasisFunctionData[i][k].imag() + _QArray[k] * BasisFunctionData[i][k].real();
		}
	}
	_IArray = IResult;
	_QArray = QResult;
}


void MyFFT::Cooley_Tukey_FFT(std::vector<std::complex<double>> _IQArray)
{
	int Size = _IQArray.size();
	if (Size <= 1)
		return;


	//반씩 나눠가며 재귀식 구현(홀,짝수 index)
	std::vector<std::complex<double>> even(Size / 2);
	std::vector<std::complex<double>> odd(Size / 2);

	for (int i = 0; i < Size / 2; ++i) {
		even[i] = _IQArray[i * 2];
		odd[i] = _IQArray[i * 2 + 1];
	}

	Cooley_Tukey_FFT(even);
	Cooley_Tukey_FFT(odd);

	//병합코드
}