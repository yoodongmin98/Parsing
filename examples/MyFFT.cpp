#include "MyFFT.h"
#include "DataClass.h"







MyFFT::MyFFT()
{
	CalculateArray.resize(MDR_ADC_SampleSize);
}

MyFFT::~MyFFT()
{

}


void MyFFT::FFTInit(std::vector<float>& _IArray, std::vector<float>& _QArray)
{
	for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
	{
		// 복소수 객체를 생성하고 실수부와 허수부를 설정
		CalculateArray[i] = std::complex<double>(_IArray[i], _QArray[i]);
	}
}


std::vector<std::complex<double>> MyFFT::Foward_FFT()
{
	//FFT
	Cooley_Tukey_FFT(CalculateArray);
	return CalculateArray;
}


std::vector<std::complex<double>> MyFFT::Reverse_FFT()
{
	/*if()*/ //FFT계산이 미리 들어와있지 않다면 MsgAssert
	Cooley_Tukey_FFT(CalculateArray, 1);
	return CalculateArray;
}


void MyFFT::CalculateBasisFunction()
{
	//기저함수 계산
	BasisFunctionData.resize(MDR_ADC_SampleSize);
	//0+1j (순 허수 j)
	std::complex<double> Basis(0, 1);

	for (auto i=0; i< MDR_ADC_SampleSize; ++i)
	{
		for (auto j = 0; j < MDR_ADC_SampleSize; ++j)
		{
			//i번째 index에 대한 j(1024)개의 기저함수
			double DotProduct = -2.0 * PI * i * j / 1024;
			//e의 지수부
			std::complex<double> exponent = Basis * DotProduct;
			//2차원배열에 복소수형태의 기저함수 push(memoization)
			BasisFunctionData[i].push_back(std::exp(exponent));
		}
	}
}


void MyFFT::Discrete_Fourier_Transform(std::vector<float>& _IArray, std::vector<float>& _QArray)
{
	//DFT 구현

	//기저함수 계산
	CalculateBasisFunction();

	//결과값을 담을 float vector (I,Q)
	std::vector<float> IResult(MDR_ADC_SampleSize, 0);
	std::vector<float> QResult(MDR_ADC_SampleSize, 0);

	//각 주파수에 대한 기저함수 내적
	//i개의 데이터(Sampling Data의 개수)에 대한
	for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
	{
		//SamplingDataSize-1 까지의 이산신호데이터와 기저함수의 내적 합
		for (auto k = 0; k < MDR_ADC_SampleSize; ++k)
		{
			//FFT누적합 = 복소수 내적 공식 [ z1*z2 = (a + jb) * (c + jd) = (ac - bd) + j(ad + bc)
			//실수부 ac-bd 허수부 ad+bc
			IResult[i] += _IArray[k] * BasisFunctionData[i][k].real() - _QArray[k] * BasisFunctionData[i][k].imag();
			QResult[i] += _IArray[k] * BasisFunctionData[i][k].imag() + _QArray[k] * BasisFunctionData[i][k].real();
		}
	}
	//결과값 대입
	_IArray = IResult;
	_QArray = QResult;
}


void MyFFT::Cooley_Tukey_FFT(std::vector<std::complex<double>>& _IQArray, bool Inverse)
{
	int Size = _IQArray.size();
	float Dot = -1.0;
	if (Size <= 1)
		return;

	//반씩 나눠가며 재귀식 구현(홀,짝수 index)
	std::vector<std::complex<double>> even(Size / 2);
	std::vector<std::complex<double>> odd(Size / 2);

	//홀수와 짝수 idx로 나눔
	for (int i = 0; i < Size / 2; ++i) 
	{
		even[i] = _IQArray[i * 2];
		odd[i] = _IQArray[i * 2 + 1];
	}

	//짝 홀수끼리 재귀 계산
	Cooley_Tukey_FFT(even);
	Cooley_Tukey_FFT(odd);

	if (Inverse)
		Dot = 1.0;

	// 기저함수를 계산하여 결과 병합
	for (int k = 0; k < Size / 2; ++k)
	{
		//홀수 idx에 대한 기저함수 내적을 미리 구함
		std::complex<double> t = std::polar(1.0, Dot * PI * k / Size) * odd[k];
		_IQArray[k] = even[k] + t;
		_IQArray[k + Size / 2] = even[k] - t;
	}

	if (_IQArray.size() == MDR_ADC_SampleSize && Inverse)
	{
		for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
		{
			_IQArray[i] /= MDR_ADC_SampleSize;
		}
	}
}