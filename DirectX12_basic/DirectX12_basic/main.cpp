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
	// �����찡 �ı�Ǹ� �ҷ����� �κ�
	if (msg == WM_DESTROY) {
		PostQuitMessage(0);	// os���� �� ���α׷��� �����Ѵٰ� ����
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

	// ������ ���� & ���
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	// �ݹ� �Լ� ����
	w.lpszClassName = _T("DX12Sample");			// ���α׷� Ŭ���� ��
	w.hInstance = GetModuleHandle(nullptr);		// handle ���

	RegisterClassEx(&w);	// ���α׷� Ŭ����(������ Ŭ������ os�� ����)

	RECT wrc = { 0, 0, windowWidth, windowHeight };	// ������ ����� ����

	// �Լ��� ����ؼ� ������ ����� ����
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ������ ������Ʈ ����
	HWND hwnd = CreateWindow(w.lpszClassName,	// Ŭ���� �� ����
		_T("DX12 test"),						// Ÿ��Ʋ �� �̸�
		WS_OVERLAPPEDWINDOW,					// Ÿ��Ʋ �ٿ� ��輱�� �ִ� ������
		CW_USEDEFAULT,							// ǥ�� x��ǥ�� OS�� �ñ�
		CW_USEDEFAULT,							// ǥ�� y��ǥ�� OS�� �ñ�
		wrc.right - wrc.left,					// ������ ��
		wrc.bottom - wrc.top,					// ������ ����
		nullptr,								// �θ� ����â �ڵ�
		nullptr,								// �޴� �ڵ�
		w.hInstance,							// ȣ�� ���ø����̼� �ڵ�
		nullptr									// �߰� �Ķ����
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

	// ����� ������
	vector<IDXGIAdapter*> adapters;

	// ���⼭ Ư�� �̸��� ���� �ƴ��� ������Ʈ�� ����
	IDXGIAdapter* tmpAdapter = nullptr;

	for (int i = 0; _dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		adapters.push_back(tmpAdapter);
	}

	for (auto adpt : adapters) {
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);	// ������� ���� ������Ʈ ���

		std::wstring strDesc = adesc.Description;

		// ã�� ���� ������� �̸� Ȯ��
		if (strDesc.find(L"NVIDIA") != std::string::npos) {
			tmpAdapter = adpt;
			break;
		}
	}

	ID3D12CommandAllocator* _cmdAllocator = nullptr;
	ID3D12GraphicsCommandList* _cmdList = nullptr;

	result = _dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&_cmdAllocator));

	/*
	* ����Ʈ ���� Ȯ��
	if (result == S_OK) {
		cout << "OK!" << endl;
	}
	*/

	result = _dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, _cmdAllocator, nullptr, IID_PPV_ARGS(&_cmdList));
	
	/*
	* ��������� ���� Ȯ��
	if (result == S_OK) {
		cout << "OK!" << endl;
	}
	*/

	ID3D12CommandQueue* _cmdQueue = nullptr;

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {};

	// �ð����� ����
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	// ����͸� �ϳ��� ����� �ÿ��� 0���� �ϸ� ��
	cmdQueueDesc.NodeMask = 0;

	// �켱������ Ư���� ������ ����
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	// Ŀ�ǵ� ����Ʈ�� ���߱�
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	// ť ����
	result = _dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&_cmdQueue));

	/*
	* ť ���� Ȯ��
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

	// ����۴� ũ�� ���� ����
	swapchainDesc.Scaling = DXGI_SCALING_STRETCH;

	// �ø� �Ŀ��� ������ �ı�
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	// Ư���� ���� ����
	swapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	// ������ <-> Ǯ ��ũ�� ���� ����
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	result = _dxgiFactory->CreateSwapChainForHwnd(
		_cmdQueue, hwnd, &swapchainDesc, nullptr, nullptr,
		(IDXGISwapChain1**)&_swapchain);		// ������ QueryInterface���� �̿��Ͽ� IDXGISwapchain4*�� ��ȯ üũ�� ������
												// ���⼭�� �˱� ���� �ϱ� ���� ĳ�������� ����

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

