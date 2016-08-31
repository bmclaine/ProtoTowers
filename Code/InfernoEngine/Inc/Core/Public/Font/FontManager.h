// Author: Jonathan Gyurkovics
//
// Handles the creation of of the DirectWrite Factory and the Direct2D Factory and Render Target. Holds onto any created Fonts and handles cleaning up any left over.

#pragma once

#include "InfernoEngine.h"

#include <dwrite.h>
#include <d2d1_1helper.h>
#include <unordered_map>

#pragma comment(lib, "Dwrite.lib")

struct IDXGISwapChain;
struct ID3D11Device;
struct IDWriteFactory;
struct ID2D1Factory;
struct ID2D1RenderTarget;
class Font;

#define SafeRelease(x) if(x) { x->Release(); x = nullptr; }

class FontManager
{
private:
	enum FontEffects {
		BORDER_EFFECT = 0,
	};

private:
	std::vector<int> m_vFontEffects;			// Holds a list of Font Effects
	std::vector<int> m_vBuffers;				// Holds a list of Constant Buffers for the effects

	MANAGER_STATUS m_eFontStatus;				// Stores the status of the font manager
	IDWriteFactory* m_pDWriteFactory;			// Pointer to the Direct Write Factory
	ID2D1Factory* m_pD2DFactory;				// Pointer to the Direct 2D Factory
	ID2D1RenderTarget* m_pRT;					// Pointer to the current Direct 2D Render Target
	ID3D11Device* m_pDevice;					// Pointer to the current device
	float m_uiDpiX;								// The monitor's X DPI
	float m_uiDpiY;								// The monitor's Y DPI

	// Remove Copy and Assignment operators
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;

	void ReleaseRTV();
	void RebuildRTV(void* pdata);

public:
	// Ctor and Dtor
	FontManager();
	~FontManager();

	// IN: HWND            - Handle to the window to draw to
	//     IDXGISwapChain* - Pointer to the swap chain. Needed only if working with DirectX
	// OUT: bool - Status of the initialization
	//
	// Sets up the Font Manager
	bool Initialize(HWND window, ID3D11Device* device = nullptr, IDXGISwapChain* backbuffer = nullptr);
	// IN: void
	// OUT: bool - Status of the destroy
	//
	// Destroys the data of the current instance
	bool Destroy();

	// IN: ColorF - Color to clear to
	// OUT: void
	//
	// Clears the Direct2D backbuffer. Only use if not using DirectX
	void Clear(D2D1::ColorF ClearColor);

	// IN: void
	// OUT: void
	//
	// Sets the backbuffer up to draw text to
	void BeginDraw();
	// IN: D2D_MATRIX_3X2_F - transform to use when drawling the text
	// OUT: void
	//
	// Sets the render transform for the text
	void SetTransform(D2D1_MATRIX_3X2_F Transform);
	// IN: void
	// OUT: HRESULT - Result of ending the draw
	//
	// Finalizes all draw operations on the back buffer
	HRESULT EndDraw();

	friend class Font;
};

