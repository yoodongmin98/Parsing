#pragma once
#include <iostream>
#include <complex>
#include <vector>


class MyFFT
{
public:
	MyFFT();
	~MyFFT();

	void Foward_FFT(std::complex<float>& _IQArray);
	void Reverse_FFT(std::complex<float>& _IQArray);
	void CalculateBasisFunction();
protected:
private:
	//Memoization
	std::vector<std::complex<float>> BasisFunctionData;
};
