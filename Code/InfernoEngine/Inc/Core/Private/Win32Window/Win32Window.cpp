#include "Win32Window\Win32Window.h"
#include "Win32Window\IWndProc.h"
#include "AssertHelper.h"

LRESULT CALLBACK GlobalWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	LONG_PTR ObjPtr = GetWindowLongPtr(hWnd, 0);

	if (0 == ObjPtr) {
		return(DefWindowProc(hWnd, uMsg, wParam, lParam));
	}
	else {
		return reinterpret_cast<IWndProc*>(ObjPtr)->WndProc(hWnd, uMsg, wParam, lParam);
	}
}

Win32Window::Win32Window(int width, int height, const wchar_t* name) : m_iWidth(width), m_iHeight(height), m_hWnd(0), m_hInsance(0), m_wsName(name),
	m_dStyle(WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX)), m_eWindowStyle(WINDOW_STYLE_WINDOWED)
{
}

Win32Window::~Win32Window()
{
	if (!DestroyWindow(m_hWnd))
		ALERT("Failed to destory the Window!");

	if (!UnregisterClass(m_wsName, m_hInsance))
		ALERT("Failed to unregister the Window Class!");
}

bool Win32Window::Initialize(IWndProc* object, LPCWSTR IconName, LPCWSTR SmallIconName)
{
	// Create our window class
	WNDCLASSEX wcex;
	memset(&wcex, 0, sizeof(wcex));

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = GlobalWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(&GlobalWndProc);
	wcex.hInstance = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = m_wsName;
	wcex.hIcon = (IconName != nullptr) ? LoadIcon(NULL, IconName) : LoadIcon(NULL, IDI_APPLICATION);
	wcex.hIconSm = (SmallIconName != nullptr) ? LoadIcon(NULL, SmallIconName) : LoadIcon(NULL, IDI_APPLICATION);

	if (object)
		object->BeforeRegisterWindowClass(wcex);

	if (ASSERT(!RegisterClassEx(&wcex), L"Failed to register the Windows Class!"))
	{
		// If the window class fails to register
		return false;
	}

	// Grabs the desktop dimensions
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);

	RECT ClientRect = { 0, 0, m_iWidth, m_iHeight };
	RECT WindowRect = { 0, 0, 0, 0 };
	// Adjust our windows rect based on the set flags
	AdjustWindowRect(&WindowRect, m_dStyle, FALSE);

	int windowWidth = (WindowRect.right - WindowRect.left) + ClientRect.right;
	int windowHeight = (WindowRect.bottom - WindowRect.top) + ClientRect.bottom;
	int left = (desktop.right - windowWidth) >> 1;
	int top = (desktop.bottom - windowHeight) >> 1;

	m_hWnd = CreateWindow(m_wsName, m_wsName, m_dStyle,
		left, top, windowWidth, windowHeight, NULL, NULL, NULL, NULL);

	if (!m_hWnd)
		return false;

	SetWindowLongPtr(m_hWnd, 0, reinterpret_cast<LONG_PTR>(object));
	m_hInsance = wcex.hInstance;

	ShowWindow(m_hWnd, SW_SHOW);
	SetFocus(m_hWnd);

	return true;
}

const HWND Win32Window::GetHWnd() const { return m_hWnd; }
int Win32Window::GetWindowWidth() const { return m_iWidth; }
int Win32Window::GetWindowHeight() const { return m_iHeight; }
int Win32Window::GetMaxWidth() const { return GetSystemMetrics(SM_CXSCREEN); }
int Win32Window::GetMaxHieght() const { return GetSystemMetrics(SM_CYSCREEN); }
WINDOW_STYLE Win32Window::GetWindowStyle() const { return m_eWindowStyle; }

void Win32Window::SetWindowStyle(WINDOW_STYLE style)
{
	if (!m_hWnd)
		return;

	m_eWindowStyle = style;

	switch (style)
	{
	case WINDOW_STYLE_WINDOWED:
	{
		m_dStyle = WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX);
		SetWindowLong(m_hWnd, GWL_STYLE, m_dStyle);

		// Grabs the desktop dimensions
		RECT desktop;
		GetWindowRect(GetDesktopWindow(), &desktop);

		// Set up a default window Rect
		RECT ClientRect = { 0, 0, m_iWidth, m_iHeight };
		RECT WindowRect = { 0, 0, 0, 0 };
		// Adjust our windows rect based on the set flags
		AdjustWindowRect(&WindowRect, m_dStyle, FALSE);

		int windowWidth = (WindowRect.right - WindowRect.left) + ClientRect.right;
		int windowHeight = (WindowRect.bottom - WindowRect.top) + ClientRect.bottom;
		int left = (desktop.right - windowWidth) >> 1;
		int top = (desktop.bottom - windowHeight) >> 1;

		if (SetWindowPos(m_hWnd, HWND_NOTOPMOST, left, top, windowWidth, windowHeight, SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW) == 0)
			PRINT("Error in setting window to windowed!");
	}
		break;
	case WINDOW_STYLE_BORDERLESS:
		m_dStyle = WS_POPUP;
		SetWindowLong(m_hWnd, GWL_STYLE, m_dStyle);

		if (SetWindowPos(m_hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW) == 0)
			PRINT("Error in setting window to borderless!");

		break;
	case WINDOW_STYLE_FULLSCREEN:
		m_dStyle &= ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
		SetWindowLong(m_hWnd, GWL_STYLE, m_dStyle);

		int width = GetSystemMetrics(SM_CXSCREEN);
		int height = GetSystemMetrics(SM_CYSCREEN);

		if (SetWindowPos(m_hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW) == 0)
			PRINT("Error in setting window to fullscreen!");

		break;
	}
}
void Win32Window::SetSize(int width, int height)
{
	RECT WindowRect;
	GetWindowRect(m_hWnd, &WindowRect);

	if (WindowRect.right == m_iWidth && WindowRect.bottom == m_iHeight)
		return;

	m_iWidth = width;
	m_iHeight = height;

	UpdateWindowState();
}
void Win32Window::UpdateWindowState()
{
	if (!m_hWnd)
		return;

	// Grabs the desktop dimensions
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);

	// Set up a default window Rect
	RECT ClientRect = { 0, 0, m_iWidth, m_iHeight };
	RECT WindowRect = { 0, 0, 0, 0 };
	// Adjust our windows rect based on the set flags
	AdjustWindowRect(&WindowRect, m_dStyle, FALSE);

	int windowWidth = (WindowRect.right - WindowRect.left) + ClientRect.right;
	int windowHeight = (WindowRect.bottom - WindowRect.top) + ClientRect.bottom;
	int left = (desktop.right - windowWidth) >> 1;
	int top = (desktop.bottom - windowHeight) >> 1;

	MoveWindow(m_hWnd, left, top, windowWidth, windowHeight, true);
}