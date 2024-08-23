#pragma once
#include <iostream>
#include <complex>
#include <vector>


class MyFFT
{
public:
	MyFFT();
	~MyFFT();

	void Foward_FFT(std::vector<float>& _IArray, std::vector<float>& _QArray);
	void Reverse_FFT(std::vector<float>&, std::vector<float>& _QArray);
protected:
private:
	void CalculateBasisFunction();

	void Discrete_Fourier_Transform(std::vector<float>& _IArray, std::vector<float>& _QArray);
	void Cooley_Tukey_FFT(std::vector<std::complex<double>> _IQArray);

	//Memoization
	std::vector<std::vector<std::complex<double>>> BasisFunctionData;
};
