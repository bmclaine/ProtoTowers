// Author : Jack Gillette
//
// Handles rendering text from a UIElement to the screen
#pragma once
#include "UIComponents\UIRenderer.h"

#include <string>
#include <d2d1.h>

// === Forward Declarations
class Font;
struct Color;

// === Enums
enum TextAlignment { LEADING = 0, TRAILING = 1, CENTER = 2, JUSTIFIED = 3 };
enum ParagraphAlignment { PARAGRAPH_NEAR = 0, PARAGRAPH_FAR = 1, PARAGRAPH_CENTER = 2 };

class UIText : public UIRenderer
{
private:
	// === Variables
	std::string m_sText;
	Font* m_Font;
	bool m_bStaticText;

public:
	// === Constructor / Destructor === //
	UIText(UIElement* _owner);
	virtual ~UIText() override;
	// ================================ //

	// === Interface === //
	virtual void PostInit() override;

	// === Update
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called every frame. Can be overriden by any derived UIComponents.
	virtual void Update() override;

	// === PreDraw
	//  IN : void
	//  OUT : void
	//
	//  Handles setting any custom settings / colors that the Graphics Manager may not know about.
	virtual void PreDraw();

	// === ResizeFontTexture
	// IN : void
	// OUT : void
	//
	// Resizes the fonts texture based on the current UIElement transform
	void ResizeFontTexture();
	// ================= //

	// === Accessors === //

	// === GetText
	//  IN : void
	//  OUT : string - The text
	//
	//  Returns the text that is currently being used.
	std::string GetText() const;
	// === GetColor
	//  IN : void
	//  OUT : Color - The color of the Text
	//
	//  Returns the color that the UIText is currently using
	virtual Color GetColor() const;
	// === GetFontSize
	//  IN : void
	//  OUT : float - The size of the Font
	//
	//  Returns the size of the Font that the UIText is currently using
	float GetFontSize() const;
	// === GetFontFamily
	//  IN : void
	//  OUT : const wchar_t* - The name of the Font Family
	//
	//  Returns the Name of the Font Family that the UIText is currently using
	const wchar_t* GetFontFamily() const;
	// === GetScreenRect
	//  IN : void
	//  OUT : D2D1_RECT_F - The Rectangle, on screen to draw the text at
	//
	//  Creates and Returns the rectangle on screen that the text will be drawn to.
	D2D1_RECT_F GetScreenRect();
	// ===IsStatic
	//  IN : void
	//  OUT : bool - Whether or not the UIText is static
	//
	//  Returns whether or not the UIText will ever be updated from the original text.
	bool IsStatic() const;
	// === GetTexture
	//  IN : void
	//  OUT : Texture* - Pointer to the Texture
	//
	//  Returns the pointer to the texture that the Image is using.
	virtual Texture* GetTexture() const override;
	// === GetString
	// IN : void
	// OUT: std::string - String that is being rendered
	//
	// Returns the string being rendered
	std::string GetString() const;
	// === GetStringW
	// IN : void
	// OUT: std::wstring - String that is being rendered
	//
	// Returns the string being rendered
	std::wstring GetStringW() const;
	// ================= //

	// === Mutators === //

	// === SetAlignment
	//  IN : DWRITE_TEXT_ALIGNMENT - The desired alignment
	//  OUT : void
	//
	//  Sets the Alignment of the Text.
	void SetAlignment(TextAlignment _alignment);
	// === SetColor
	//  IN : Color - The desired color
	//  OUT : void
	//
	//  Sets the color that the UIText will use.
	virtual void SetColor(Color _color);
	// === SetFontSize
	//  IN : float - The desired font size
	//  OUT : void
	//
	//  Sets the size of the Font.
	void SetFontSize(float _fontSize);
	// === SetFontFamily
	//  IN : whcar_t* - The desired font family 
	//  OUT : void 
	//
	//  Set the Font for the UIText to use
	void SetFontFamily(wchar_t* _fontFamily);
	// === SetParagraphAlignent
	//  IN : DWRITE_PARAGRAPH_ALIGNMENT - The desired alignment of the paragraph
	//  OUT : void
	//
	//  Sets the Alignment of the Paragraph
	void SetParagraphAlignment(ParagraphAlignment _alignment);
	// === SetText
	//  IN : string - The text to display
	//  OUT : void 
	// 
	//  Sets the text of the UIText
	void SetText(std::string _text);
	// === SetText
	//  IN : wstring - The text to display
	//  OUT : void 
	// 
	//  Sets the text of the UIText
	void SetText(std::wstring _text);
	// === SetStatic
	//  IN : bool - Whether or not the UIText is static
	//  OUT : void
	//
	//  Sets whether or not the UIText will ever update from the original text.
	void SetStatic(bool _isStatic);
	// === UseBorderEffect
	// IN : bool - Whether or not to use the border effect
	// OUT : void
	//
	// Updates the border effect boolean
	void UseBorderEffect(bool use);
	// === SetBorderSize
	// IN : UINT - Size of the border in texels
	// OUT: void
	//
	// Sets the border size if the border effect is being used
	void SetBorderSize(UINT size);
	// ================ //
};

