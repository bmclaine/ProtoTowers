#include "../Public/UIComponents/UIText.h"

#include "Color.h"
#include "Font\Font.h"
#include "UIElement.h"
#include "UIComponents\RectTransform.h"
#include "Assets\Texture.h"

#include "../../Interface/Public/ModuleInterface.h"

// ===== Constructor / Destructor ===== //
UIText::UIText(UIElement* _owner) : UIRenderer(_owner)
{
	m_Font = new Font(L"Arial", 12, _owner->GetTransform()->GetTotalBounds());
}

UIText::~UIText()
{
	if (m_Font) {
		delete m_Font;
	}
}
// ==================================== //

// ===== Interface ===== //
void UIText::PostInit()
{
	using namespace InfernoEngine_Graphics;

	UIRenderer::PostInit();

	// TODO - Render the text to a texture
}

// === Update
//  IN : void
//  OUT : void
//
//  Function that gets called every frame. Can be overriden by any derived UIComponents.
void UIText::Update()
{
	if (m_Font->TextureUpdateNeeded())
		m_Font->RenderString();
}

// === PreDraw
//  IN : void
//  OUT : void
//
//  Handles setting any custom settings / colors that the Graphics Manager may not know about.
void UIText::PreDraw()
{
	struct Data {
		Vector2 CanvasX, CanvasY, ImageU, ImageV;
	};

	Vector4 screenRect = GetTransform()->GetScreenRectangle();
	Data data = { Vector2(screenRect.y, screenRect.w), Vector2(screenRect.x, screenRect.z), Vector2(0, 1), Vector2(0, 1) };

	InfernoEngine_Graphics::UpdateSubResource(m_iCanvasDetailsBufferID, &data, sizeof(Data));
	InfernoEngine_Graphics::SetUVOffsets(m_Font->GetTexture()->GetOffsets(), m_Font->GetTexture()->GetTiling());

	if (m_fImageFill >= 0.0f) {
		InfernoEngine_Graphics::UpdateSubResource(m_iFillRatioBufferID, &m_fImageFill, sizeof(float));
	}

	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_Font->GetTexture()->GetImage(), 0);
	InfernoEngine_Graphics::SetColor(m_Color);
	InfernoEngine_Graphics::SetWorldMatrix(GetTransform()->GetWorldMatrix());
}

// === ResizeFontTexture
// IN : void
// OUT : void
//
// Resizes the fonts texture based on the current UIElement transform
void UIText::ResizeFontTexture()
{
	if (m_Font)
		m_Font->UpdateRenderTargetSize(m_pRectTransform->GetTotalBounds());
}
// ===================== //

// ===== Accessors ===== //

// === GetText
//  IN : void
//  OUT : string - The text
//
//  Returns the text that is currently being used.
std::string UIText::GetText() const
{
	return m_sText;
}

// === GetColor
//  IN : void
//  OUT : Color - The color of the Text
//
//  Returns the color that the UIText is currently using
Color UIText::GetColor() const
{
	return m_Color;
}

// === GetFontSize
//  IN : void
//  OUT : float - The size of the Font
//
//  Returns the size of the Font that the UIText is currently using
float UIText::GetFontSize() const
{
	return m_Font->GetFontSize();
}

// === GetFontFamily
//  IN : void
//  OUT : const wchar_t* - The name of the Font Family
//
//  Returns the Name of the Font Family that the UIText is currently using
const wchar_t* UIText::GetFontFamily() const
{
	return m_Font->GetFontFamily();
}

// === GetScreenRect
//  IN : void
//  OUT : D2D1_RECT_F - The Rectangle, on screen to draw the text at
//
//  Creates and Returns the rectangle on screen that the text will be drawn to.
D2D1_RECT_F UIText::GetScreenRect()
{
	Vector2 screenPos = GetTransform()->GetScreenPosition();
	Vector2 bounds = GetTransform()->GetBounds();
	D2D1_RECT_F rect = { screenPos.x - bounds.x, screenPos.y - bounds.y, screenPos.x + bounds.x, screenPos.y + bounds.y };
	return rect;
}

// ===IsStatic
//  IN : void
//  OUT : bool - Whether or not the UIText is static
//
//  Returns whether or not the UIText will ever be updated from the original text.
bool UIText::IsStatic() const
{
	return m_bStaticText;
}

// === GetTexture
//  IN : void
//  OUT : Texture* - Pointer to the Texture
//
//  Returns the pointer to the texture that the Image is using.
Texture* UIText::GetTexture() const
{
	return m_Font->GetTexture();
}
// === GetString
// IN : void
// OUT: std::string - String that is being rendered
//
// Returns the string being rendered
std::string UIText::GetString() const
{
	return Inferno::ToAsciiString(m_Font->m_wsText);
}
// === GetStringW
// IN : void
// OUT: std::wstring - String that is being rendered
//
// Returns the string being rendered
std::wstring UIText::GetStringW() const
{
	return m_Font->m_wsText;
}
// ===================== //

// ===== Mutators ===== //

// === SetAlignment
//  IN : DWRITE_TEXT_ALIGNMENT - The desired alignment
//  OUT : void
//
//  Sets the Alignment of the Text.
void UIText::SetAlignment(TextAlignment _alignment)
{
	m_Font->SetTextAlignment((DWRITE_TEXT_ALIGNMENT)_alignment);
}

// === SetColor
//  IN : Color - The desired color
//  OUT : void
//
//  Sets the color that the UIText will use.
void UIText::SetColor(Color _color)
{
	m_Color = _color;
}

// === SetFontSize
//  IN : float - The desired font size
//  OUT : void
//
//  Sets the size of the Font.
void UIText::SetFontSize(float _fontSize)
{
	m_Font->UpdateFont(_fontSize, m_Font->GetFontFamily());
}

// === SetFontFamily
//  IN : whcar_t* - The desired font family 
//  OUT : void 
//
//  Set the Font for the UIText to use
void UIText::SetFontFamily(wchar_t* _fontFamily)
{
	m_Font->UpdateFont(m_Font->GetFontSize(), _fontFamily);
}

// === SetParagraphAlignent
//  IN : DWRITE_PARAGRAPH_ALIGNMENT - The desired alignment of the paragraph
//  OUT : void
//
//  Sets the Alignment of the Paragraph
void UIText::SetParagraphAlignment(ParagraphAlignment _alignment)
{
	m_Font->SetParagraphAlignment((DWRITE_PARAGRAPH_ALIGNMENT)_alignment);
}

// === SetText
//  IN : string - The text to display
//  OUT : void 
// 
//  Sets the text of the UIText
void UIText::SetText(std::string _text)
{
	m_sText = _text;
	m_Font->SetDrawString(_text);
}
// === SetText
//  IN : wstring - The text to display
//  OUT : void 
// 
//  Sets the text of the UIText
void UIText::SetText(std::wstring _text)
{
	m_sText = Inferno::ToAsciiString(_text);
	m_Font->SetDrawString(_text);
}

// === SetStatic
//  IN : bool - Whether or not the UIText is static
//  OUT : void
//
//  Sets whether or not the UIText will ever update from the original text.
void UIText::SetStatic(bool _isStatic)
{
	m_bStaticText = _isStatic;
}

// === UseBorderEffect
// IN : bool - Whether or not to use the border effect
// OUT : void
//
// Updates the border effect boolean
void UIText::UseBorderEffect(bool use)
{
	m_Font->UseBorderEffect(use);
}
// === SetBorderSize
// IN : UINT - Size of the border in texels
// OUT: void
//
// Sets the border size if the border effect is being used
void UIText::SetBorderSize(UINT size)
{
	m_Font->SetBorderSize(size);
}
// ==================== //


