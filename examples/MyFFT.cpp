#include "MyFFT.h"
#include "DataClass.h"







MyFFT::MyFFT()
{
	CalculateArray.resize(MDR_ADC_SampleSize);
}

MyFFT::~MyFFT()
{

}



void MyFFT::Foward_FFT(std::vector<float>& _IArray, std::vector<float>& _QArray)
{
	//FFT
	try
	{
		for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
		{
			// ���Ҽ� ��ü�� �����ϰ� �Ǽ��ο� ����θ� ����
			CalculateArray[i] = std::complex<double>(_IArray[i], _QArray[i]);
		}
		if (_IArray.size() == _QArray.size())
			Cooley_Tukey_FFT(CalculateArray);
		else
			throw;
		for (auto i = 0; i < MDR_ADC_SampleSize; ++i)
		{
			// ���Ҽ� ��ü�� �����ϰ� �Ǽ��ο� ����θ� ����
			_IArray[i] = CalculateArray[i].real();
			_QArray[i] = CalculateArray[i].imag();
		}
		return;
	}
	catch (int n)
	{
		//DFT
		Discrete_Fourier_Transform(_IArray, _QArray);
	}
}


void MyFFT::Reverse_FFT(std::vector<float>& _IArray, std::vector<float>& _QArray)
{
	/*if()*/ //FFT����� �̸� �������� �ʴٸ� MsgAssert
	//Cooley_Tukey_FFT(CalculateArray);
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

//bool�� �߰��� reverse�Ǻ�(���߿�)
void MyFFT::Cooley_Tukey_FFT(std::vector<std::complex<double>> _IQArray)
{
	int Size = _IQArray.size();
	if (Size <= 1)
		return;

	//�ݾ� �������� ��ͽ� ����(Ȧ,¦�� index)
	std::vector<std::complex<double>> even(Size / 2);
	std::vector<std::complex<double>> odd(Size / 2);

	for (int i = 0; i < Size / 2; ++i) 
	{
		even[i] = _IQArray[i * 2];
		odd[i] = _IQArray[i * 2 + 1];
	}

	//¦ Ȧ������ ��� ���
	Cooley_Tukey_FFT(even);
	Cooley_Tukey_FFT(odd);

	// �����Լ��� ����Ͽ� ��� ����
	for (int k = 0; k < Size / 2; ++k)
	{
		std::complex<double> t = std::polar(1.0, -2.0 * PI * k / Size) * odd[k];
		_IQArray[k] = even[k] + t;
		_IQArray[k + Size / 2] = even[k] - t;
	}

}