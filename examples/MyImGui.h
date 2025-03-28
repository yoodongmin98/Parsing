#pragma once

#include "DataClass.h"
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include "implot.h"  
#include <d3d9.h>
#include <iostream>
#include <queue>
#include <tchar.h>
#include <vector>

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static bool                     g_DeviceLost = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


class Calculator;
class Buffer;
class MyImGui
{
public:
	MyImGui();
	~MyImGui();

	void Instance();
	
protected:
private:
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void InitializeImPlot();
	void ShutdownImPlot();
	void ResetDevice();

	void CalculatorStartAndGraphDebug();


	std::shared_ptr<Buffer> InstanceBuffer = nullptr;
	std::shared_ptr<Calculator> Calculators = nullptr;

	//Graph ���� ���� ���� ������ ����
	std::vector<float> Classify_I;
	std::vector<float> Classify_Q;

	std::vector<float> Doppler_I;
	std::vector<float> Doppler_Q;

	std::vector<float> Window_I;
	std::vector<float> Window_Q;

	std::vector<float> FFT_I;
	std::vector<float> FFT_Q;

	std::vector<float> DCPOINT_I;
	std::vector<float> DCPOINT_Q;


	std::vector<float> HLCUT_I;
	std::vector<float> HLCUT_Q;

	std::vector<float> RFFT_I;
	std::vector<float> RFFT_Q;


	//Calculate
	std::vector<float> MAG;
	std::vector<float> Freq;
	std::vector<float> Speed;
};