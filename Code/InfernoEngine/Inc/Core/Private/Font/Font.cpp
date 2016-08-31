#include "Font\Font.h"
#include "AssertHelper.h"
#include "GlobalIncludes.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Assets\Texture.h"

#include <d3d11.h>

FontManager* Font::s_Manager = nullptr;

bool operator==(const D2D1::ColorF& lhs, const D2D1::ColorF& rhs)
{
	return lhs.a == rhs.a && lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b;
}

void Font::SetFontManager(FontManager* manager) { s_Manager = manager; }

Font::Font(const wchar_t* FontFamily, float FontSize, Vector2 RenderTargetSize, DWRITE_FONT_WEIGHT FontWeight, DWRITE_FONT_STYLE FontStyle, DWRITE_FONT_STRETCH FontStretch) : m_d2dColor(D2D1::ColorF::White)
{
	m_pRenderTarget = nullptr;
	m_pBrush = nullptr;
	m_pTextFormat = nullptr;
	m_pImage = new Texture();
	m_pImage2 = new Texture();

	m_pRenderImage = m_pImage;

	if (s_Manager == nullptr || s_Manager->m_pDevice == nullptr)
		return;

	/*if (!ASSERT(s_Manager != nullptr, "Font Manager was not set! Cannot create Fonts without the Font Manager being assigned!"))
		return;

	if (s_Manager->m_pDevice == nullptr)
	{
		ALERT(L"Font Mananger has an invalid device attached!");

		return;
	}*/

	m_wsFontFamily = L"";
	m_fFontSize = 0.0f;
	m_d2dRect = { 0, 0, RenderTargetSize.x, RenderTargetSize.y };
	m_wsText = L"";
	m_bStringChanged = true;
	m_bBorderEffect = false;

	m_TextureInfo.TextureDimensionsX = m_TextureInfo.TextureDimensionsY = 0;
	m_TextureInfo.AlphaThreshold = 0.0f;
	m_TextureInfo.BorderThickness = 2;

	ASSERT(!SUCCEEDED(UpdateFont(FontSize, FontFamily, FontWeight, FontStyle, FontStretch)), (std::wstring(L"Failed to create font ") + std::wstring(FontFamily)).c_str());
	ASSERT(!SUCCEEDED(SetColor(D2D1::ColorF::White)), L"Failed to create Font Brush!");
	ASSERT(!SUCCEEDED(UpdateRenderTargetSize(RenderTargetSize)), L"Failed to create the render target and texture for the font!");
}

// Dtor
// Releases the Text Format and Brush and notifies the Font Manager it has been deleted
Font::~Font()
{
	SAFE_RELASE(m_pTextFormat);
	SAFE_RELASE(m_pBrush);
	SAFE_RELASE(m_pRenderTarget);
	SAFE_DELETE(m_pImage);
	SAFE_DELETE(m_pImage2);
}


// IN: wchar_t*    - String to display
//	   UINT32      - Number of characters in the string
//     D2D1_RECT_F - Rectangle for drawling. Order of rect variables is x, y, width, height
// OUT: void
//
// Draws the passed in string in the area of the passed in rectangle
void Font::DrawStringToBackBuffer(const wchar_t* Text, UINT32 TextCount, D2D1_RECT_F DrawRegion)
{
	// Make sure everything initialized correctly
	if (!m_pTextFormat || !m_pBrush)
		return;

	s_Manager->m_pRT->DrawText(
		Text,			  // The string to render.
		TextCount,		  // The string's length.
		m_pTextFormat,	  // The text format.
		DrawRegion,       // The region of the window where the text will be rendered.
		m_pBrush		  // The brush used to draw the text.
		);
}
/*
// IN: ID3D11Texture2D* - Texture to render to
//     wchar_t*         - String to display
//     UINT32           - Number of characters in the string
//     D2D1_RECT_F      - Rectangle for drawling. Order of rect variables is x, y, width, height
// OUT: void
//
// Renders the text passed in to the texture passed in in the rectangle region specified
void Font::RenderToTexture(ID3D11Texture2D* texture, const wchar_t* Text, UINT32 TextCount, D2D1_RECT_F DrawRegion)
{
	ID2D1RenderTarget* tempRT;
	IDXGISurface *pDxgiSurface = nullptr;
	HRESULT hr = texture->QueryInterface(&pDxgiSurface);

	if (SUCCEEDED(hr))
	{
		// Create a D2D render target which can draw into our offscreen D3D
		// surface. Given that we use a constant size for the texture, we
		// fix the DPI at 96.
		D2D1_RENDER_TARGET_PROPERTIES props =
			D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96
			);

		hr = FontManager::s_pD2DFactory->CreateDxgiSurfaceRenderTarget(
			pDxgiSurface,
			&props,
			&tempRT
			);

		if (SUCCEEDED(hr)) {
			tempRT->BeginDraw();
			tempRT->SetTransform(D2D1::IdentityMatrix());
			tempRT->DrawText(
				Text,
				TextCount,
				m_pTextFormat,
				DrawRegion,
				m_pBrush
				);
			tempRT->EndDraw();

			SafeRelease(tempRT);
		}
		else
			ALERT(L"Failed to create a DXGI Render Target from the DXGI Surface!");

		SafeRelease(pDxgiSurface);
	}
	else
		ALERT(L"Failed to grab DXGI Surface from Texture2D!");
}
*/

// IN: void
// OUT: void
//
// Renders the current string using the current draw region
void Font::RenderString()
{
	if (!m_bStringChanged || !m_pTextFormat || !m_pRenderTarget || !m_pBrush)
		return;

	m_pRenderTarget->BeginDraw();

	m_pRenderTarget->Clear(D2D1::ColorF::ColorF(D2D1::ColorF::White, 0.0f));

	m_pRenderTarget->DrawText(
		m_wsText.c_str(),
		(UINT32)m_wsText.length(),
		m_pTextFormat,
		m_d2dRect,
		m_pBrush
		);
	HRESULT hr = m_pRenderTarget->EndDraw();

	if (m_bBorderEffect)
		BorderEffect();

	m_bStringChanged = false;
}

// IN: void
// OUT: void
//
// Applies the border effect to the font texture
void Font::BorderEffect()
{
	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, -1);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);
	InfernoEngine_Graphics::SetShader(PIXEL_SHADER, -1);
	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, s_Manager->m_vFontEffects[FontManager::FontEffects::BORDER_EFFECT]);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, -1, 0);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_pImage->GetImage(), 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iUAV2, 0);

	InfernoEngine_Graphics::UpdateSubResource(s_Manager->m_vBuffers[FontManager::FontEffects::BORDER_EFFECT], &m_TextureInfo, sizeof(TextureInfo));

	InfernoEngine_Graphics::Disptach(align(m_TextureInfo.TextureDimensionsX, 32) / 32, align(m_TextureInfo.TextureDimensionsY, 32) / 32, 1);

	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, -1);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);

	InfernoEngine_Graphics::ApplyPipelineState();
}

// IN: string - string to draw
// OUT: void
//
// Sets the draw string if it's different
void Font::SetDrawString(std::string string)
{
	SetDrawString(Inferno::ToWideString(string));
}
// IN: wstring - string to draw
// OUT: void
//
// Sets the draw string if it's different
void Font::SetDrawString(std::wstring string)
{
	if (m_wsText == string)
		return;

	m_wsText = string;
	m_bStringChanged = true;
}

// IN: DWRITE_TEXT_ALIGNMENT - The text alignment
// OUT: void
//
// Sets the text alignment for the font
void Font::SetTextAlignment(DWRITE_TEXT_ALIGNMENT Alignment)
{
	if (m_pTextFormat)
		HRESULT hr = m_pTextFormat->SetTextAlignment(Alignment);
}

// IN: DWRITE_PARAGRAPH_ALIGNMENT - The paragraph alignment
// OUT: void
//
// Sets the paragraph alignment for the font
void Font::SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT Alignment)
{
	if (m_pTextFormat)
		HRESULT hr = m_pTextFormat->SetParagraphAlignment(Alignment);
}

// IN: float - size of the spacing
// OUT: void
//
// Sets the line spacing to the incoming value
void Font::SetLineSpacing(float value)
{
	m_pTextFormat->SetLineSpacing(DWRITE_LINE_SPACING_METHOD_DEFAULT, value, value * 0.8f);
}

// IN : D2D1::ColorF - The desired color of the Font
// OUT : void
//
// Sets the Color of the Font.
HRESULT Font::SetColor(D2D1::ColorF _color)
{
	if (!s_Manager || (_color == m_d2dColor && m_pBrush))
		return S_OK;

	m_d2dColor = _color;

	SAFE_RELEASE(m_pBrush);
	return s_Manager->m_pRT->CreateSolidColorBrush(m_d2dColor, &m_pBrush);
}

// IN: float               - The desired size 
//     wchar_t*            - Name of the desired Font Family
//     DWRITE_FONT_WEIGHT  - The weight style of the font, ex. Bold
//     DWRITE_FONT_STYLE   - The style of the font, ex. Italics
//     DWRITE_FONT_STRETCH - The stretching of the font
// OUT: HRESULT - Result of updating the font
//
// Sets the size of the Font to the desired size.
HRESULT Font::UpdateFont(float _size, const wchar_t* _fontFamily, DWRITE_FONT_WEIGHT FontWeight, DWRITE_FONT_STYLE FontStyle, DWRITE_FONT_STRETCH FontStretch)
{
	if (!s_Manager || (_size == m_fFontSize && _fontFamily == m_wsFontFamily && FontWeight == m_eFontWeight && FontStyle == m_eFontStyle && FontStretch == m_eFontStretch))
		return S_OK;

	m_fFontSize = _size;
	m_wsFontFamily = _fontFamily;
	m_eFontWeight = FontWeight;
	m_eFontStyle = FontStyle;
	m_eFontStretch = FontStretch;

	DWRITE_TEXT_ALIGNMENT alignment = DWRITE_TEXT_ALIGNMENT::DWRITE_TEXT_ALIGNMENT_LEADING;
	if (m_pTextFormat != nullptr) {
		alignment = m_pTextFormat->GetTextAlignment();
	}

	SAFE_RELASE(m_pTextFormat);

	HRESULT result = s_Manager->m_pDWriteFactory->CreateTextFormat(
		_fontFamily,
		nullptr,
		FontWeight,
		FontStyle,
		FontStretch,
		_size,
		L"en-us",
		&m_pTextFormat);

	if (result == S_OK) {
		m_pTextFormat->SetTextAlignment(alignment);
	}

	return result;
}

// IN: Vector2 - size of the render target
// OUT: HRESULT - Status of updating the render target size
//
// Sets the size of the render target
HRESULT Font::UpdateRenderTargetSize(Vector2 rtSize)
{
	if (!s_Manager || m_f2RenderTargetSize == rtSize)
		return S_OK;

	m_f2RenderTargetSize = rtSize;
	m_TextureInfo.TextureDimensionsX = (UINT)rtSize.x;
	m_TextureInfo.TextureDimensionsY = (UINT)rtSize.y;

	HRESULT hr = S_OK;

	ID3D11Texture2D *pTexture;
	D3D11_TEXTURE2D_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(D3D11_TEXTURE2D_DESC));
	tDesc.ArraySize = 1;
	tDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	tDesc.MipLevels = 1;
	tDesc.Width = (UINT)rtSize.x;
	tDesc.Height = (UINT)rtSize.y;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	s_Manager->m_pDevice->CreateTexture2D(&tDesc, nullptr, &pTexture);

	ID3D11ShaderResourceView* FontSRV = nullptr;
	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MipLevels = 1;

	s_Manager->m_pDevice->CreateShaderResourceView(pTexture, &SRVDesc, &FontSRV);
	SAFE_RELEASE(pTexture);				// Release the temp texture

	// Grab the texture that was created by the SRV
	FontSRV->GetResource(reinterpret_cast<ID3D11Resource**>(&pTexture));
	if (pTexture == nullptr)
	{
		ALERT(L"Failed to grab the texture from the Shader Resource View");

		return ERROR_GRAPHICS_DATASET_IS_EMPTY;
	}

	m_pImage->SetImage(InfernoEngine_Graphics::AddImage(L"", FontSRV));

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
		s_Manager->m_uiDpiX,
		s_Manager->m_uiDpiY
		);

	IDXGISurface *pDxgiSurface = nullptr;
	if (!SUCCEEDED(hr = pTexture->QueryInterface(&pDxgiSurface)))
	{
		ALERT(L"Failed to get the DXGI Surface from the font texture!");
		SAFE_RELEASE(pTexture);
		return hr;
	}

	if (!SUCCEEDED(hr = s_Manager->m_pD2DFactory->CreateDxgiSurfaceRenderTarget(
		pDxgiSurface,
		&props,
		&m_pRenderTarget
		)))
	{
		ALERT(L"Error creating render target for font!");
	}

	SAFE_RELASE(pDxgiSurface);
	SAFE_RELEASE(pTexture);

	m_iUAV = InfernoEngine_Graphics::CreateUnorderedAccessView(m_pImage->GetImage(), 0);

	// Create the second texture for use with addition effects on the text
	s_Manager->m_pDevice->CreateTexture2D(&tDesc, nullptr, &pTexture);
	s_Manager->m_pDevice->CreateShaderResourceView(pTexture, &SRVDesc, &FontSRV);
	SAFE_RELEASE(pTexture);				// Release the temp texture

	m_pImage2->SetImage(InfernoEngine_Graphics::AddImage(L"", FontSRV));
	m_iUAV2 = InfernoEngine_Graphics::CreateUnorderedAccessView(m_pImage2->GetImage(), 0);

	return hr;
}

// IN: void
// OUT: bool - If texture needs updating
//
// States if the texture needs to be updated
bool Font::TextureUpdateNeeded() const { return m_bStringChanged; }

// IN : void
// OUT : D2D1::ColorF - The current color of the Font.
//
// Returns the current color of the Font.
D2D1::ColorF Font::GetColor() const
{
	return m_d2dColor;
}

// IN : void
// OUT : float - The current size of the Font.
//
// Returns the current size of the Font.
float Font::GetFontSize() const
{
	return m_fFontSize;
}

// IN : void
// OUT : wchar_t* - The current Font Family of the Font.
//
// Returns the current Font Family of the Font.
const wchar_t* Font::GetFontFamily() const
{
	return m_wsFontFamily.c_str();
}

// IN: void
// OUT: int - ID of the texture the render target is writing to
//
// Returns the texture ID for the texture that the font is being rendered to
Texture* Font::GetTexture() const { return m_pRenderImage; }

// IN: bool - Whether or not to use the border effect
// OUT: void
//
// Updates the border effect boolean
void Font::UseBorderEffect(bool use)
{
	if (m_bBorderEffect == use)
		return;

	m_bBorderEffect = use;

	if (use)
		m_pRenderImage = m_pImage2;
	else
		m_pRenderImage = m_pImage;

	m_bStringChanged = true;
}

void Font::SetBorderSize(UINT size)
{
	m_TextureInfo.BorderThickness = (int)size;
}