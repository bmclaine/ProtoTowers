#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct IWndProc;

enum WINDOW_STYLE {
	WINDOW_STYLE_WINDOWED,
	WINDOW_STYLE_BORDERLESS,
	WINDOW_STYLE_FULLSCREEN,
};

class Win32Window
{
private:
	HWND			m_hWnd;
	HINSTANCE		m_hInsance;
	const wchar_t*	m_wsName;
	int				m_iWidth;
	int				m_iHeight;
	DWORD			m_dStyle;
	WINDOW_STYLE	m_eWindowStyle;

public:
	Win32Window(int width, int height, const wchar_t* name);
	~Win32Window();

	bool Initialize(IWndProc* object, LPCWSTR IconName = nullptr, LPCWSTR SmallIconName = nullptr);

	const HWND GetHWnd() const;
	int GetWindowWidth() const;
	int GetWindowHeight() const;
	int GetMaxWidth() const;
	int GetMaxHieght() const;
	WINDOW_STYLE GetWindowStyle() const;

	void SetWindowStyle(WINDOW_STYLE style);
	void SetSize(int width, int height);
	void UpdateWindowState();
};

