// Author: Jonathan Gyurkovics
//
// Handles writing font based textures to the screen or to passed in textures

#pragma once

#include "FontManager.h"

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11Device;
class Texture;

class Font
{
private:
	struct TextureInfo
	{
		UINT TextureDimensionsX;
		UINT TextureDimensionsY;
		float  AlphaThreshold;
		int   BorderThickness;
	};

private:
	D2D1_RECT_F m_d2dRect;					// Holds the draw region of the text
	D2D1::ColorF m_d2dColor;				// Holds the current Font Color
	TextureInfo m_TextureInfo;				// Holds the current texture info

	ID2D1RenderTarget* m_pRenderTarget;		// Pointer to the render target
	IDWriteTextFormat* m_pTextFormat;		// Pointer to the Direct Write Text Format
	ID2D1SolidColorBrush* m_pBrush;			// Pointer to the brush for this text

	static FontManager* s_Manager;			// Static pointer to the font manager;

	std::wstring m_wsFontFamily;			// Wide string holding the font family name
	float m_fFontSize;						// Holds the font size
	std::wstring m_wsText;					// String to draw to the texture
	Vector2 m_f2RenderTargetSize;			// Holds the size of the render target
	DWRITE_FONT_WEIGHT m_eFontWeight;		// Holds the font weight style
	DWRITE_FONT_STYLE m_eFontStyle;			// Holds the font style
	DWRITE_FONT_STRETCH m_eFontStretch;		// Holds the font stretch style

	Texture* m_pImage;						// Holds the ID of the image when rendering
	int m_iUAV;								// ID for the UAV for the texture for post processing effects on the font
	Texture* m_pImage2;						// Holds the ID of the second image if any addition effects are used
	int m_iUAV2;							// ID for the second UAV for addition effects

	Texture* m_pRenderImage;				// Holds the pointer to the current texture which will be rendered

	bool m_bStringChanged;					// Tells the system when the string has changed
	bool m_bBorderEffect;					// Tells the system to use the border effect or not
	
	// Ctor and Dtor
	Font(const wchar_t* FontFamily, float FontSize, Vector2 RenderTargetSize, DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE FontStyle = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH FontStretch = DWRITE_FONT_STRETCH_NORMAL);
	~Font();

	// Removed the Copy and Assignment operators
	Font(const Font&) = delete;
	Font& operator=(const Font&) = delete;

	// IN: D2D1::ColorF - The desired color of the Font
	// OUT: HRESULT - Status of setting the color
	//
	// Sets the Color of the Font.
	HRESULT SetColor(D2D1::ColorF _color);

	static void SetFontManager(FontManager* manager);

	// IN: void
	// OUT: void
	//
	// Applies the border effect to the font texture
	void BorderEffect();

public:
	// IN: wchar_t*    - String to display
	//	   UINT32      - Number of characters in the string
	//     D2D1_RECT_F - Rectangle for drawling. Order of rect variables is x, y, width, height
	// OUT: void
	//
	// Draws the passed in string in the area of the passed in rectangle straight to the back buffer
	void DrawStringToBackBuffer(const wchar_t* Text, UINT32 TextCount, D2D1_RECT_F DrawRegion);
	// IN: ID3D11Texture2D* - Texture to render to
	//     wchar_t*         - String to display
	//     UINT32           - Number of characters in the string
	//     D2D1_RECT_F      - Rectangle for drawling. Order of rect variables is x, y, width, height
	// OUT: void
	//
	// Renders the text passed in to the texture passed in in the rectangle region specified
	//void RenderToTexture(ID3D11Texture2D* texture, const wchar_t* Text, UINT32 TextCount, D2D1_RECT_F DrawRegion);

	// IN: void
	// OUT: void
	//
	// Renders the current string using the current draw region
	void RenderString();

	// IN: string - string to draw
	// OUT: void
	//
	// Sets the draw string if it's different
	void SetDrawString(std::string);
	// IN: wstring - string to draw
	// OUT: void
	//
	// Sets the draw string if it's different
	void SetDrawString(std::wstring);

	// IN: DWRITE_TEXT_ALIGNMENT - The text alignment
	// OUT: void
	//
	// Sets the text alignment for the font
	void SetTextAlignment(DWRITE_TEXT_ALIGNMENT Alignment);
	// IN: DWRITE_PARAGRAPH_ALIGNMENT - The paragraph alignment
	// OUT: void
	//
	// Sets the paragraph alignment for the font
	void SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT Alignment);

	// IN: float - size of the spacing
	// OUT: void
	//
	// Sets the line spacing to the incoming value
	void SetLineSpacing(float value);

	// IN: float               - The desired size 
	//     wchar_t*            - Name of the desired Font Family
	//     DWRITE_FONT_WEIGHT  - The weight style of the font, ex. Bold
	//     DWRITE_FONT_STYLE   - The style of the font, ex. Italics
	//     DWRITE_FONT_STRETCH - The stretching of the font
	// OUT: HRESULT - Status of updating to font
	//
	// Sets the size of the Font to the desired size.
	HRESULT UpdateFont(float _size, const wchar_t* _fontFamily, DWRITE_FONT_WEIGHT FontWeight = DWRITE_FONT_WEIGHT_NORMAL,
		DWRITE_FONT_STYLE FontStyle = DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH FontStretch = DWRITE_FONT_STRETCH_NORMAL);

	// IN: Vector2 - size of the render target
	// OUT: HRESULT - Status of updating the render target size
	//
	// Sets the size of the render target
	HRESULT UpdateRenderTargetSize(Vector2 rtSize);

	// IN: void
	// OUT: bool - If texture needs updating
	//
	// States if the texture needs to be updated
	bool TextureUpdateNeeded() const;

	// IN: void
	// OUT: D2D1::ColorF - The current color of the Font.
	//
	// Returns the current color of the Font.
	D2D1::ColorF GetColor() const;

	// IN: void
	// OUT: float - The current size of the Font.
	//
	// Returns the current size of the Font.
	float GetFontSize() const;

	// IN: void
	// OUT: wchar_t* - The current Font Family of the Font.
	//
	// Returns the current Font Family of the Font.
	const wchar_t* GetFontFamily() const;

	// IN: void
	// OUT: Texture* - Pointer to the texture the font is writing to
	//
	// Returns the texture pointer for the texture that the font is being rendered to
	Texture* GetTexture() const;

	// IN: bool - Whether or not to use the border effect
	// OUT: void
	//
	// Updates the border effect boolean
	void UseBorderEffect(bool use);

	void SetBorderSize(UINT size);

	friend class FontManager;
	friend class InfernoEngine;
	friend class UIText;
};

