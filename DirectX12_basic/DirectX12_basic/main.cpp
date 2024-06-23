#include <Windows.h>
#include <tchar.h>
#ifdef _DEBUG
#include <iostream>
#endif

#include <vector>

#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


//#define window_width 1280;
//#define window_height 860;

using namespace std;

void DebugOutputFormatString(const char* format, ...) {
#ifdef _DEBUG
	va_list valist;
	va_start(valist, format);
	printf(format, valist);
	va_end(valist);
#endif
}

LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	// 윈도우가 파기되면 불려지는 부분
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);	// os에게 이 프로그램을 종료한다고 전달
		return 0;
}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

#ifdef _DEBUG
int main() {
#else
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
#endif
	DebugOutputFormatString("Show window test.");
	getchar();

	int windowWidth = 1280, windowHeight = 860;

	// 윈도우 생성 & 등록
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	// 콜백 함수 지정
	w.lpszClassName = _T("DX12Sample");			// 프로그램 클래스 명
	w.hInstance = GetModuleHandle(nullptr);		// handle 취득

	RegisterClassEx(&w);	// 프로그램 클래스(윈도우 클래스를 os에 전달)

	RECT wrc = { 0, 0, windowWidth, windowHeight };	// 윈도우 사이즈를 결정

	// 함수를 사용해서 윈도우 사이즈를 보정
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// 윈도우 오브젝트 생성
	HWND hwnd = CreateWindow(w.lpszClassName,	// 클래스 명 지정
		_T("DX12 test"),						// 타이틀 바 이름
		WS_OVERLAPPEDWINDOW,					// 타이틀 바와 경계선이 있는 윈도우
		CW_USEDEFAULT,							// 표시 x좌표는 OS에 맡김
		CW_USEDEFAULT,							// 표시 y좌표는 OS에 맡김
		wrc.right - wrc.left,					// 윈도우 폭
		wrc.bottom - wrc.top,					// 윈도우 높이
		nullptr,								// 부모 윈도창 핸들
		nullptr,								// 메뉴 핸들
		w.hInstance,							// 호출 어플리케이션 핸들
		nullptr									// 추가 파라미터
	);

	ID3D12Device* _dev = nullptr;
	IDXGIFactory6* _dxgiFactory = nullptr;
	IDXGISwapChain4* _swapchain = nullptr;

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	D3D_FEATURE_LEVEL featureLevel;

	for (auto lv : levels) {
		if (D3D12CreateDevice(nullptr, lv, IID_PPV_ARGS(&_dev)) == S_OK) {
			featureLevel = lv;
			break;
		}
	}

	auto result = CreateDXGIFactory1(IID_PPV_ARGS(&_dxgiFactory));

	// 어댑터 열거형
	vector<IDXGIAdapter*> adapters;

	// 여기서 특정 이름을 가진 아댑터 오브젝트가 삽입
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);	// 어댑터의 설명 오브젝트 취득

		std::wstring strDesc = adesc.Description;

		// 찾고 싶은 어댑터의 이름 확인
		if (strDesc.find(L"NVIDIA") != std::string::npos) {
			tmpAdapter = adpt;
			break;
		}
	}

	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;

	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));

	/*
	* 리스트 생성 확인
	if (result == S_OK) {
		cout << "OK!" << endl;
	}
	*/

	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	
	/*
	* 얼로케이터 생성 확인
	if (result == S_OK) {
		cout << "OK!" << endl;
	}
	*/

	ID3D12CommandQueue* _cmdQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	// 시간제한 없음
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// 어댑터를 하나만 사용할 시에는 0으로 하면 됨
	cmdQueueDesc.NodeMask = 0;

	// 우선순위는 특별히 정하지 않음
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	// 커맨드 리스트와 맞추기
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// 큐 생성
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	/*
	* 큐 생성 확인
	if (result == S_OK) {
		cout << "OK!" << endl;
	}
	*/

	DXGI_SWAP_CHAIN_DESC1 swapchainDesc = {};

	swapchainDesc.Width = windowWidth;
	swapchainDesc.Height = windowHeight;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.Stereo = false;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.SampleDesc.Quality = 0;
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapchainDesc.BufferCount = 2;

	// 백버퍼는 크기 변경 가능
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;

	// 플립 후에는 빠르게 파기
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// 특별한 지정 없음
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	// 윈도우 <-> 풀 스크린 변경 가능
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue, hwnd, &swapchainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)&_swapchain);		// 원래는 QueryInterface등을 이용하여 IDXGISwapchain4*에 변환 체크를 하지만
												// 여기서는 알기 쉽게 하기 위해 캐스팅으로 대응

	/*
	if (result == S_OK) {
		cout << "OK!" << endl;
	} 
	*/


	ShowWindow(hwnd, SW_SHOW);

	MSG msg = {};

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT) {
			break;
		}
	}
	
	UnregisterClass(w.lpszClassName, w.hInstance);
	

	return 0;
}

