#include "MyFFT.h"








MyFFT::MyFFT()
{
	CalculateBasisFunction();
}

MyFFT::~MyFFT()
{

}



void MyFFT::Foward_FFT(std::complex<float>& _IQArray)
{

}


void MyFFT::Reverse_FFT(std::complex<float>& _IQArray)
{
	//?
}


void MyFFT::CalculateBasisFunction()
{
	BasisFunctionData.resize(1024);
	//기저함수
}