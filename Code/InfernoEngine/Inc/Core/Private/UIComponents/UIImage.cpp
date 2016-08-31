#include "../Public/UIComponents/UIImage.h"
#include "MathLib.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "UIComponents\RectTransform.h"

// ===== Constructor / Destructor ===== //
UIImage::UIImage(UIElement* _owner) : UIRenderer(_owner)
{

}

UIImage::~UIImage()
{

}
// ==================================== //

// ===== Interface ===== //
// === PreDraw
//  IN : void
//  OUT : void
//
//  Handles setting any custom settings / colors that the Graphics Manager may not know about.
void UIImage::PreDraw()
{
	struct Data {
		Vector2 CanvasX, CanvasY, ImageU, ImageV;
	};

	Vector4 screenRect = GetTransform()->GetScreenRectangle();
	Data data = { Vector2(screenRect.y, screenRect.w), Vector2(screenRect.x, screenRect.z), Vector2(0, 1), Vector2(0, 1) };
	float rotation = m_pRectTransform->GetRotation();

	InfernoEngine_Graphics::UpdateSubResource(m_iCanvasDetailsBufferID, &data, sizeof(Data));
	InfernoEngine_Graphics::UpdateSubResource(m_iRotationInfoBufferID, &rotation, sizeof(rotation));
	InfernoEngine_Graphics::SetUVOffsets(m_pMaterial->GetOffsets(), m_pMaterial->GetTiling());

	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_pMaterial->DiffuseMapID, 0);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_pMaterial->OcclusionMapID, 2);
	InfernoEngine_Graphics::SetColor(m_Color);
	InfernoEngine_Graphics::SetWorldMatrix(GetTransform()->GetWorldMatrix());
}
// ===================== //

// ===== Accessors ===== //

// === GetColor
//  IN : void
//  OUT : Color - The color offset of the Image
//
//  Returns the color offset of the Image.
Color UIImage::GetColor() const
{
	return m_Color;
}
// === GetTexture
//  IN : void
//  OUT : Texture* - Pointer to the Texture
//
//  Returns the pointer to the texture that the Image is using.
Material* UIImage::GetMaterial() const { return m_pMaterial; }

Texture* UIImage::GetTexture() const { return nullptr; }

// ===================== //

// ===== Mutators ===== //

// === SetColor
//  IN : Color - The color offset to use
//  OUT : void
//
//  Sets the color offset of the image.
void UIImage::SetColor(Color _color)
{
	m_Color = _color;
}
// === SetTexture
//  IN : Texture* - A pointer to the Texture to use
//  OUT : void
//
//  Sets the Texture for the Image to use.
void UIImage::SetMaterial(Material* _texture)
{
	m_pMaterial = _texture;
	m_uiEffectID = _texture->EffectID;
}

void UIImage::SetTexture(Texture*) {}
// ==================== //