#pragma once
#include <iostream>
#include <complex>
#include <vector>


class MyFFT
{
public:
	MyFFT();
	~MyFFT();

	void FFTInit(std::vector<float>& _IArray, std::vector<float>& _QArray);
	std::vector<std::complex<double>> Foward_FFT();
	std::vector<std::complex<double>> Reverse_FFT();
protected:
private:
	void CalculateBasisFunction();

	void Discrete_Fourier_Transform(std::vector<float>& _IArray, std::vector<float>& _QArray);
	void Cooley_Tukey_FFT(std::vector<std::complex<double>>& _IQArray, bool Inverse = 0);

	//Memoization(�����Լ�, DFT����)
	std::vector<std::vector<std::complex<double>>> BasisFunctionData;
	
	std::vector<std::complex<double>> CalculateArray;

};
