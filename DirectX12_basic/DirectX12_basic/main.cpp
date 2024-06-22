#include <Windows.h>
#include <tchar.h>
#ifdef _DEBUG
#include <iostream>
#endif

#include <d3d12.h>
#include <dxgi1_6.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

ID3D12Device* _dev = nullptr;
IDXGIFactory6* _dxgiFactory = nullptr;
IDXGISwapChain4* _swapchain = nullptr;

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

	// ������ ���� & ���
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	// �ݹ� �Լ� ����
	w.lpszClassName = _T("DX12Sample");			// ���α׷� Ŭ���� ��
	w.hInstance = GetModuleHandle(nullptr);		// handle ���

	RegisterClassEx(&w);	// ���α׷� Ŭ����(������ Ŭ������ os�� ����)

	RECT wrc = { 0, 0, 1280, 860 };	// ������ ����� ����

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

