#include "stdafx.h"
#include "ProtoTowers.h"

#include "InfernoEngine.h"

#define MAX_LOADSTRING 100

// Global Variables:							// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

int __stdcall wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PROTOTOWERS, szWindowClass, MAX_LOADSTRING);

	InfernoEngine::CreateInstance();
	InfernoEngine* engine = InfernoEngine::GetInstance();
	engine->Initialize(
		hInstance,
		MAKEINTRESOURCE(IDI_PROTOTOWERS),
		MAKEINTRESOURCE(IDI_SMALL),
		szTitle,
		szWindowClass
		);

	while (engine->Update())
	{

	}

	engine->Destroy();
	engine->DeleteInstance();

	return 0/*(int) msg.wParam*/;
}