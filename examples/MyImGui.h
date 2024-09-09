#pragma once

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"
#include <d3d9.h>
#include <tchar.h>
#include "implot.h"  
#include <queue>

#include <iostream>
#include <vector>

// Data
static LPDIRECT3D9              g_pD3D = nullptr;
static LPDIRECT3DDEVICE9        g_pd3dDevice = nullptr;
static bool                     g_DeviceLost = false;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Buffer;
class Core;
class MyImGui
{
public:
	MyImGui();
	~MyImGui();

	void Instance(int argc, char** argv);
	
protected:
private:
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void InitializeImPlot();
	void ShutdownImPlot();
	void ResetDevice();

	void CoreStartAndGraphDebug(int argc, char** argv);


	std::shared_ptr<Core> Cores = nullptr;
	std::shared_ptr<Buffer> InstanceBuffer = nullptr;

	//Graph 따로 띄우기 위해 나눠서 저장
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

	




	//Test
	std::vector<int> MaxIndex;
	std::vector<int> SpeedVector;
	std::vector<int> SpeedVector2;
	std::vector<int> SpeedVector3;
	int MaxValue = 0;
	int Count = 0;
	bool TestBool = true;
};