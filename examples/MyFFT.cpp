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
		// ���Ҽ� ��ü�� �����ϰ� �Ǽ��ο� ����θ� ����
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
	/*if()*/ //FFT����� �̸� �������� �ʴٸ� MsgAssert
	Cooley_Tukey_FFT(CalculateArray, 1);
	return CalculateArray;
}


void MyFFT::CalculateBasisFunction()
{
	//�����Լ� ���
	BasisFunctionData.resize(MDR_ADC_SampleSize);
	//0+1j (�� ��� j)
	std::complex<double> Basis(0, 1);

	for (auto i=0; i< MDR_ADC_SampleSize; ++i)
	{
		for (auto j = 0; j < MDR_ADC_SampleSize; ++j)
		{
			//i��° index�� ���� j(1024)���� �����Լ�
			double DotProduct = -2.0 * PI * i * j / 1024;
			//e�� ������
			std::complex<double> exponent = Basis * DotProduct;
			//2�����迭�� ���Ҽ������� �����Լ� push(memoization)
			BasisFunctionData[i].push_back(std::exp(exponent));
		}
	}
}


void MyFFT::Discrete_Fourier_Transform(std::vector<float>& _IArray, std::vector<float>& _QArray)
{
	//DFT ����

	//�����Լ� ���
	CalculateBasisFunction();

	//������� ���� float vector (I,Q)
	std::vector<float> IResult(MDR_ADC_SampleSize, 0);
	std::vector<float> QResult(MDR_ADC_SampleSize, 0);

	//�� ���ļ��� ���� �����Լ� ����
	//i���� ������(Sampling Data�� ����)�� ����
	for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
	{
		//SamplingDataSize-1 ������ �̻��ȣ�����Ϳ� �����Լ��� ���� ��
		for (auto k = 0; k < MDR_ADC_SampleSize; ++k)
		{
			//FFT������ = ���Ҽ� ���� ���� [ z1*z2 = (a + jb) * (c + jd) = (ac - bd) + j(ad + bc)
			//�Ǽ��� ac-bd ����� ad+bc
			IResult[i] += _IArray[k] * BasisFunctionData[i][k].real() - _QArray[k] * BasisFunctionData[i][k].imag();
			QResult[i] += _IArray[k] * BasisFunctionData[i][k].imag() + _QArray[k] * BasisFunctionData[i][k].real();
		}
	}
	//����� ����
	_IArray = IResult;
	_QArray = QResult;
}


void MyFFT::Cooley_Tukey_FFT(std::vector<std::complex<double>>& _IQArray, bool Inverse)
{
	int Size = _IQArray.size();
	float Dot = -1.0;
	if (Size <= 1)
		return;

	//�ݾ� �������� ��ͽ� ����(Ȧ,¦�� index)
	std::vector<std::complex<double>> even(Size / 2);
	std::vector<std::complex<double>> odd(Size / 2);

	//Ȧ���� ¦�� idx�� ����
	for (int i = 0; i < Size / 2; ++i) 
	{
		even[i] = _IQArray[i * 2];
		odd[i] = _IQArray[i * 2 + 1];
	}

	//¦ Ȧ������ ��� ���
	Cooley_Tukey_FFT(even);
	Cooley_Tukey_FFT(odd);

	if (Inverse)
		Dot = 1.0;

	// �����Լ��� ����Ͽ� ��� ����
	for (int k = 0; k < Size / 2; ++k)
	{
		//Ȧ�� idx�� ���� �����Լ� ������ �̸� ����
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