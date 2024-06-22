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

	// 윈도우 생성 & 등록
	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProcedure;	// 콜백 함수 지정
	w.lpszClassName = _T("DX12Sample");			// 프로그램 클래스 명
	w.hInstance = GetModuleHandle(nullptr);		// handle 취득

	RegisterClassEx(&w);	// 프로그램 클래스(윈도우 클래스를 os에 전달)

	RECT wrc = { 0, 0, 1280, 860 };	// 윈도우 사이즈를 결정

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

