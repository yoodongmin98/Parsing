#include "Buffer.h"
#include "Calculator.h"
#include "DataClass.h"
#include "MyImGui.h"
#include <algorithm>
#include <iostream>
#include <math.h>
#include <numeric>
#include <random>
#include <stdlib.h>
#include <unordered_map>
#include <vector>


MyImGui::MyImGui()
{
	InstanceBuffer = std::make_shared<Buffer>();
	Calculators = std::make_shared<Calculator>();
	Speed.resize(20);
}
MyImGui::~MyImGui()
{

}


void MyImGui::InitializeImPlot()
{
	ImPlot::CreateContext();
}

void MyImGui::ShutdownImPlot()
{
	ImPlot::DestroyContext();
}


bool MyImGui::CreateDeviceD3D(HWND hWnd)
{
	if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr)
		return false;


	ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
	g_d3dpp.Windowed = TRUE;
	g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN; 
	g_d3dpp.EnableAutoDepthStencil = TRUE;
	g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;  
	if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
		return false;

	return true;
}

void MyImGui::CleanupDeviceD3D()
{
	if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
	if (g_pD3D) { g_pD3D->Release(); g_pD3D = nullptr; }
}

void MyImGui::ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);
	ImGui_ImplDX9_CreateDeviceObjects();
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
			return 0;
		g_ResizeWidth = (UINT)LOWORD(lParam); 
		g_ResizeHeight = (UINT)HIWORD(lParam);
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU) 
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}


void MyImGui::Instance()
{
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
	::RegisterClassExW(&wc);
	HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW, 0,0, 2500, 1280, nullptr, nullptr, wc.hInstance, nullptr);

	if (!CreateDeviceD3D(hwnd))
	{
		this->CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return;
	}

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      


	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX9_Init(g_pd3dDevice);

	InitializeImPlot();  // ImPlot 초기화

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	
	bool done = false;
	while (!done)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				done = true;
		}
		if (done)
			break;

		if (g_DeviceLost)
		{
			HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
			if (hr == D3DERR_DEVICELOST)
			{
				::Sleep(10);
				continue;
			}
			if (hr == D3DERR_DEVICENOTRESET)
				ResetDevice();
			g_DeviceLost = false;
		}

		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			g_d3dpp.BackBufferWidth = g_ResizeWidth;
			g_d3dpp.BackBufferHeight = g_ResizeHeight;
			g_ResizeWidth = g_ResizeHeight = 0;
			ResetDevice();
		}

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		///////////////////Calculator Start//////////////////////////
		CalculatorStartAndGraphDebug();
		/////////////////////////////////////////////////////////////

		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
		if (g_pd3dDevice->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			g_pd3dDevice->EndScene();
		}
		HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
		if (result == D3DERR_DEVICELOST)
			g_DeviceLost = true;
	}

	ShutdownImPlot();
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	this->CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}



void MyImGui::CalculatorStartAndGraphDebug()
{
	//////////////////////////CoreStart//////////////////////////
	/////													/////
	/////													/////
				Calculators->CalCulate();
	/////													/////
	/////													/////
	/////////////////////////////////////////////////////////////



	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////					Default I, Q															/////
	/////																							/////
			ImGui::Begin("CalCulate Window");
			Classify_I = Buffer::Buffers->GetMDR_I_Classify();
			Classify_Q = Buffer::Buffers->GetMDR_Q_Classify();
			if (ImPlot::BeginPlot("Default I, Q Data", "Index", "Value", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -100, 1024);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -300, 400);
				ImPlot::PlotLine("I Data", Classify_I.data(), (int)Classify_I.size());
				ImPlot::PlotLine("Q Data", Classify_Q.data(), (int)Classify_Q.size());
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////					Doppler I, Q															/////
	/////																							/////
			ImGui::Begin("CalCulate Window2");
			Doppler_I = Buffer::Buffers->GetMDR_I_Doppler();
			Doppler_Q = Buffer::Buffers->GetMDR_Q_Doppler();
			if (ImPlot::BeginPlot("Doppler I,Q Data", "Index", "Value", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -100, 1024);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -300, 400);
				ImPlot::PlotLine("I Data", Doppler_I.data(), (int)Doppler_I.size());
				ImPlot::PlotLine("Q Data", Doppler_Q.data(), (int)Doppler_Q.size());
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////					Window I, Q																/////
	/////																							/////
			ImGui::Begin("CalCulate Window3");
			Window_I = Buffer::Buffers->GetMDR_I_Window();
			Window_Q = Buffer::Buffers->GetMDR_Q_Window();
			if (ImPlot::BeginPlot("Window I,Q Data", "Index", "Value", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -100, 1024);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -300, 400);
				ImPlot::PlotLine("I Data", Window_I.data(), (int)Window_I.size());
				ImPlot::PlotLine("Q Data", Window_Q.data(), (int)Window_Q.size());
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////					FFT I, Q																/////
	/////																							/////
			ImGui::Begin("CalCulate Window4");
			FFT_I = Buffer::Buffers->GetMDR_I_FFT();
			FFT_Q = Buffer::Buffers->GetMDR_Q_FFT();
			if (ImPlot::BeginPlot("FFT I,Q Data", "Index", "Value", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -100, 1024);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -300, 400);
				ImPlot::PlotLine("I Data", FFT_I.data(), (int)FFT_I.size());
				ImPlot::PlotLine("Q Data", FFT_Q.data(), (int)FFT_Q.size());
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////					Kill DC, POINT I, Q														/////
	/////																							/////
			ImGui::Begin("CalCulate Window5");
			DCPOINT_I = Buffer::Buffers->GetMDR_I_DCPOINT();
			DCPOINT_Q = Buffer::Buffers->GetMDR_Q_DCPOINT();
			if (ImPlot::BeginPlot("Kill DC,POINT I,Q Data", "Index", "Value", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -100, 1024);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -300, 400);
				ImPlot::PlotLine("I Data", DCPOINT_I.data(), (int)DCPOINT_I.size());
				ImPlot::PlotLine("Q Data", DCPOINT_Q.data(), (int)DCPOINT_Q.size());
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////					High, Low Cut I, Q														/////
	/////																							/////
			ImGui::Begin("CalCulate Window6");
			HLCUT_I = Buffer::Buffers->GetMDR_I_HLCUT();
			HLCUT_Q = Buffer::Buffers->GetMDR_Q_HLCUT();
			if (ImPlot::BeginPlot("High, Low Cut I,Q Data", "Index", "Value", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -100, 1024);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -300, 400);
				ImPlot::PlotLine("I Data", HLCUT_I.data(), (int)HLCUT_I.size());
				ImPlot::PlotLine("Q Data", HLCUT_Q.data(), (int)HLCUT_Q.size());
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////					RFFT I, Q																/////
	/////																							/////
			ImGui::Begin("CalCulate Window7");
			RFFT_I = Buffer::Buffers->GetMDR_I_RFFT();
			RFFT_Q = Buffer::Buffers->GetMDR_Q_RFFT();
			if (ImPlot::BeginPlot("RFFT I,Q Data", "Index", "Value(x100)", ImVec2(500, 300), ImPlotFlags_None))
			{
				/*ImPlot::SetupAxisLimits(ImAxis_X1, -100, 1024);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -30000, 40000);*/
				ImPlot::SetupAxisLimits(ImAxis_X1, -100, 1024);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -300, 400);
				ImPlot::PlotLine("I Data", RFFT_I.data(), (int)RFFT_I.size());
				ImPlot::PlotLine("Q Data", RFFT_Q.data(), (int)RFFT_Q.size());
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////					Magnitude I, Q															/////
	/////																							/////
			ImGui::Begin("CalCulate Window8");
			MAG = Buffer::Buffers->GetMDR_I_Mag();
			if (ImPlot::BeginPlot("Magnitude I,Q Data", "Index", "Value(x100)", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -100, 512);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -50, 100000);
				ImPlot::PlotLine("I Data", MAG.data(), (int)MAG.size());
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////				Speed																		/////
	/////																							/////
			ImGui::Begin("CalCulate Window9");
			//저장된 Speed배열(size=20)을 가져옴
			Speed=Buffer::Buffers->GetDopplerSimpleResult()->Speed;
		
			sort(Speed.begin(), Speed.end(), std::less<float>());
			
			std::vector<int> Result;
			for (auto i = 1; i <= 5; ++i)
			{
				//대충 5개만뽑아봐
				Result.push_back(accumulate(Speed.begin()+10, Speed.begin()+11, 0)/2);
 			}
			//그래프 표기하는 부분
			if (ImPlot::BeginPlot("Speed Data", "Index", "Value", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -3, 15);
				ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 40);
				ImPlot::PlotLine("Speed Data", Result.data(), (int)Result.size());
				ImPlot::EndPlot();
			}
			ImGui::End();

			
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////


	/////////////////////////WindowStart/////////////////////////////////////////////////////////////////
	/////																							/////
	/////																							/////
	/////							Direction														/////
	/////																							/////
			std::unordered_map<int, int> frequencyMap;

			// 배열 내 각 원소의 빈도수를 계산
			for (EnumDirection num : Buffer::Buffers->GetDopplerSimpleResult()->Direction) {
				frequencyMap[static_cast<int>(num)]++;
			}

			EnumDirection mostFrequentElement = Buffer::Buffers->GetDopplerSimpleResult()->Direction[0];
			int maxFrequency = 0;

			// 빈도수 맵을 순회하면서 가장 빈도수가 높은 원소 찾기
			for ( auto& pair : frequencyMap) {
				if (pair.second > maxFrequency) {
					maxFrequency = pair.second;
					int(mostFrequentElement) = pair.first;
				}
			}


			ImGui::Begin("CalCulate Window10");
			if (ImPlot::BeginPlot("Freq Data", "Index", "Value(x100)", ImVec2(500, 300), ImPlotFlags_None))
			{
				ImPlot::SetupAxisLimits(ImAxis_X1, -2, 2);
				ImPlot::SetupAxisLimits(ImAxis_Y1, -2, 2);
				if (int(mostFrequentElement) == 1) {
					ImPlot::PlotText("Forward!", 0, 0);
				}
				else if (int(mostFrequentElement) == -1) {
					ImPlot::PlotText("Backward!", 0, 0);
				}
				ImPlot::EndPlot();
			}
			ImGui::End();
	/////																							/////
	/////																							/////	
	/////////////////////////////////////////////////////////////////////////////////////////////////////
}



