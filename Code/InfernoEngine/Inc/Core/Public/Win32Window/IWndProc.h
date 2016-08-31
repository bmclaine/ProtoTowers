#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct __declspec(novtable) IWndProc
{
	virtual LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) = 0;
	virtual void BeforeRegisterWindowClass(WNDCLASSEX &_wc) {}
};