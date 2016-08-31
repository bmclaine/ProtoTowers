#include "Font\FontManager.h"
#include "Font\Font.h"
#include "AssertHelper.h"
#include "GlobalIncludes.h"
#include "EventSystem.h"
#include "../../Interface/Public/ModuleInterface.h"

#include <string>

#include <d2d1_1.h>

#pragma comment(lib, "d2d1.lib")

// Default Ctor
// Initializes all variables to default
FontManager::FontManager()
{
	m_vFontEffects.reserve(5);
	m_vBuffers.reserve(5);

	m_eFontStatus = MANAGER_STATUS_UNINITALIZED;
	m_pDWriteFactory = nullptr;
	m_pD2DFactory = nullptr;
	m_pRT = nullptr;
	m_pDevice = nullptr;
	m_uiDpiX = 0;
	m_uiDpiY = 0;

	EventSystem::GetInstance()->RegisterForEvent("ReleaseSwapChainResources", this, std::bind(&FontManager::ReleaseRTV, this));
	EventSystem::GetInstance()->RegisterForEvent("SwapChainResized", this, BindEventFunction(&FontManager::RebuildRTV, this));
}

// Defualt Dtor
// Calls Destroy to clean up data
FontManager::~FontManager()
{
	Destroy();

	EventSystem::GetInstance()->UnregisterFromEvent("ReleaseSwapChainResources", this, std::bind(&FontManager::ReleaseRTV, this));
	EventSystem::GetInstance()->UnregisterFromEvent("SwapChainResized", this, BindEventFunction(&FontManager::RebuildRTV, this));
}

void FontManager::ReleaseRTV()
{
	SAFE_RELASE(m_pRT);
}
void FontManager::RebuildRTV(void* pdata)
{
	IDXGISwapChain* backbuffer = static_cast<IDXGISwapChain*>(pdata);
	
	// Grab the DXGI Surface from the backbuffer
	IDXGISurface* pDxgiSurface = nullptr;
	HRESULT hr = backbuffer->GetBuffer(0, IID_PPV_ARGS(&pDxgiSurface));

	if (ASSERT(!SUCCEEDED(hr), L"Failed to grab the DXGI Surface from the DirectX Backbuffer!"))
	{
		m_eFontStatus = MANAGER_STATUS_ERROR;
		return;
	}

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		m_uiDpiX,
		m_uiDpiY
		);

	// Create a DXGI Render Target
	hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
		pDxgiSurface,
		&props,
		&m_pRT
		);

	SafeRelease(pDxgiSurface);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the DXGI Render Target for Direct2D!"))
	{
		m_eFontStatus = MANAGER_STATUS_ERROR;
		return;
	}
}

// IN: HWND            - Handle to the window to draw to
//     IDXGISwapChain* - Pointer to the swap chain. Needed only if working with DirectX
// OUT: bool - Status of the initialization
//
// Sets up the Font Manager
bool FontManager::Initialize(HWND window, ID3D11Device* device, IDXGISwapChain* backbuffer)
{
	if (m_eFontStatus != MANAGER_STATUS_UNINITALIZED || window == nullptr)
		return false;

	PRINT("Initializing Font Manager...");

	m_pDevice = device;

	// Create our Direct Write Factory
	HRESULT hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&m_pDWriteFactory)
		);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the DirectWrite Factory!"))
	{
		m_eFontStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	// Create our Direct2D Factory
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		&m_pD2DFactory
		);

	if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Direct2D Factory"))
	{
		m_eFontStatus = MANAGER_STATUS_ERROR;
		return false;
	}

	m_pD2DFactory->GetDesktopDpi(&m_uiDpiX, &m_uiDpiY);		// Grab the desktops DPI

	// Determine if we are using DirectX or not
	if (backbuffer == nullptr)
	{
		RECT rc;
		GetClientRect(window, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

		ID2D1HwndRenderTarget* hrt = nullptr;

		// Create a window based render target
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(window, size),
			&hrt);

		if (ASSERT(!SUCCEEDED(hr), L"Failed to create the Direct2D Render Target"))
		{
			m_eFontStatus = MANAGER_STATUS_ERROR;
			return false;
		}

		m_pRT = hrt;
	}
	else
	{
		// Grab the DXGI Surface from the backbuffer
		IDXGISurface* pDxgiSurface = nullptr;
		hr = backbuffer->GetBuffer(0, IID_PPV_ARGS(&pDxgiSurface));

		if (ASSERT(!SUCCEEDED(hr), L"Failed to grab the DXGI Surface from the DirectX Backbuffer!"))
		{
			m_eFontStatus = MANAGER_STATUS_ERROR;
			return false;
		}

		D2D1_RENDER_TARGET_PROPERTIES props =
			D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			m_uiDpiX,
			m_uiDpiY
			);

		// Create a DXGI Render Target
		hr = m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
			pDxgiSurface,
			&props,
			&m_pRT
			);

		SafeRelease(pDxgiSurface);

		if (ASSERT(!SUCCEEDED(hr), L"Failed to create the DXGI Render Target for Direct2D!"))
		{
			m_eFontStatus = MANAGER_STATUS_ERROR;
			return false;
		}
	}

	m_vFontEffects.push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"OutlineFontComputeShader"));
	m_vBuffers.push_back(InfernoEngine_Graphics::GetConstantBuffer(L"TextureInfo"));

	Font::SetFontManager(this);

	PRINT("Success!\n");

	m_eFontStatus = MANAGER_STATUS_INITIALIZED;
	return true;
}

// IN: void
// OUT: bool - Status of the destroy
//
// Destroys the data of the current instance
bool FontManager::Destroy()
{
	if (m_eFontStatus != MANAGER_STATUS_INITIALIZED)
		return false;

	PRINT("Destroying Font Manager...");

	SafeRelease(m_pD2DFactory);
	SafeRelease(m_pRT);
	SafeRelease(m_pDWriteFactory);

	PRINT("Success!\n");

	m_eFontStatus = MANAGER_STATUS_DESTORYED;
	return true;
}

// IN: ColorF - Color to clear to
// OUT: void
//
// Clears the Direct2D backbuffer. Only use if not using DirectX
void FontManager::Clear(D2D1::ColorF ClearColor)
{
	m_pRT->Clear(ClearColor);
}

// IN: void
// OUT: void
//
// Sets the backbuffer up to draw text to
void FontManager::BeginDraw()
{
	m_pRT->BeginDraw();
}

// IN: D2D_MATRIX_3X2_F - transform to use when drawling the text
// OUT: void
//
// Sets the render transform for the text
void FontManager::SetTransform(D2D1_MATRIX_3X2_F Transform)
{
	m_pRT->SetTransform(Transform);
}

// IN: void
// OUT: HRESULT - Result of ending the draw
//
// Finalizes all draw operations on the back buffer
HRESULT FontManager::EndDraw()
{
	return m_pRT->EndDraw();
}