#include "../Public/UIComponents//UIRenderer.h"

#include "../../Interface/Public/Engine.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Assets\Texture.h"
#include "UIComponents\RectTransform.h"

#include "UIElement.h"

// === Constructor / Destructor === //
UIRenderer::UIRenderer(UIElement* _owner) : UIComponent(_owner)
{
	using namespace Inferno;
	m_uiEffectID = Inferno::GetEffectByName("UI_Unlit");
	m_uiLayer = 0;
	m_fImageFill = 1.0;
	m_eEffect = UI_UNLIT_EFFECT;
	m_bRegistered = false;

	SetRadialOrigin(ORIGIN_LEFT);
}

UIRenderer::~UIRenderer()
{
	OnDisable();
}
// ================================ //

// === Interface === //
void UIRenderer::Init()
{

}

void UIRenderer::PostInit()
{
	using namespace InfernoEngine_Graphics;

	m_iCanvasDetailsBufferID = GetConstantBuffer(L"CanvasDetails");
	m_iFillRatioBufferID = GetConstantBuffer(L"FillRatio");
	m_iRadialInfoBufferID = GetConstantBuffer(L"RadialOriginInfo");
	m_iRotationInfoBufferID = GetConstantBuffer(L"RotationInfo");
}

void UIRenderer::OnEnable()
{
	if (!m_bRegistered) {
		using namespace InfernoEngine_Graphics;
		RegisterUIRenderComponent(this);
		m_bRegistered = true;
	}
}

void UIRenderer::OnDisable()
{
	if (m_bRegistered) {
		using namespace InfernoEngine_Graphics;
		UnregisterUIRenderComponent(this);
		m_bRegistered = false;
	}
}

// === PreDraw
//  IN : void
//  OUT : void
//
//  Handles setting any custom settings / colors that the Graphics Manager may not know about.
void UIRenderer::PreDraw()
{
	struct Data {
		Vector2 CanvasX, CanvasY, ImageU, ImageV;
	};

	Vector4 screenRect = GetTransform()->GetScreenRectangle();
	Data data = { Vector2(screenRect.y, screenRect.w), Vector2(screenRect.x, screenRect.z), Vector2(0, 1), Vector2(0, 1) };
	float rotation = m_pRectTransform->GetRotation();

	InfernoEngine_Graphics::UpdateSubResource(m_iCanvasDetailsBufferID, &data, sizeof(Data));
	InfernoEngine_Graphics::UpdateSubResource(m_iRotationInfoBufferID, &rotation, sizeof(rotation));
	InfernoEngine_Graphics::SetUVOffsets(m_pTexture->GetOffsets(), m_pTexture->GetTiling());

	if (m_eEffect != UI_UNLIT_EFFECT) {
		InfernoEngine_Graphics::UpdateSubResource(m_iFillRatioBufferID, &m_fImageFill, sizeof(float));

		if (m_eEffect & (UI_FILL_RADIAL | UI_FILL_RADIAL_CLOCKWISE))
			InfernoEngine_Graphics::UpdateSubResource(m_iRadialInfoBufferID, &m_RadialOriginInfo, sizeof(RadialOriginInfo));
	}
	
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_pTexture->GetImage(), 0);
	InfernoEngine_Graphics::SetColor(m_Color);
	InfernoEngine_Graphics::SetWorldMatrix(GetTransform()->GetWorldMatrix());
}
// ================= //

// ===== Accessors ===== //

// === GetLayer
//  IN : void
//  OUT : unsigned int - The layer of the UI. Higher number is in front.
//
//  Returns the Layer of the UIRenderer, higher numbers will render in front of lower numbers.
unsigned int UIRenderer::GetLayer() const
{
	return m_uiLayer;
}

// === GetColor
//  IN : void
//  OUT : Color - The color offset of the Image
//
//  Returns the color offset of the Image.
Color UIRenderer::GetColor() const
{
	return m_Color;
}

// === GetTexture
//  IN : void
//  OUT : Texture* - Pointer to the Texture
//
//  Returns the pointer to the texture that the Image is using.
Texture* UIRenderer::GetTexture() const
{
	return m_pTexture;
}
// ===================== //

// ===== Mutators ===== //

// === SetLayer
//  IN : unsigned int - The layer to put the UI on
//  OUT : void
//
//  Sets the Layer of the UIRenderer. Higher numbers will be drawn in front of lower numbers.
void UIRenderer::SetLayer(unsigned int _layer)
{
	m_uiLayer = _layer;
}

// === SetImageFill
//  IN : float - The Fill Ratio of the Image. Clamps between 0 and 1.
//  OUT : void
//
//  Sets the Fill Ratio of this image. If it's the first time that the Fill is being set, it'll set up the UIRenderer to have the correct 
//  effect on it.
void UIRenderer::SetImageFill(float _fillRatio)
{
	if (m_fImageFill < 0) {
		m_uiEffectID = Inferno::GetEffectByName("UI_Fill_Unlit");
		m_iFillRatioBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"FillRatio");
	}

	m_fImageFill = Clamp(0.0f, 1.0f, _fillRatio);
}
// === SetEffect
// IN : UIEffect - ID of the effect to use
// OUT : void
//
// Sets the effect for the UIRenderer to use
void UIRenderer::SetEffect(UIEffects effect)
{
	if (effect == m_eEffect)
		return;

	m_eEffect = effect;

	switch (effect)
	{
	case UI_FILL_EFFECT:
		m_uiEffectID = Inferno::GetEffectByName("UI_Fill_Unlit");
		break;
	case UI_FILL_RIGHT_EFFECT:
		m_uiEffectID = Inferno::GetEffectByName("UI_FillRight_Unlit");
		break;
	case UI_FILL_TOP_EFFECT:
		m_uiEffectID = Inferno::GetEffectByName("UI_FillTop_Unlit");
		break;
	case UI_FILL_RADIAL:
		m_uiEffectID = Inferno::GetEffectByName("UI_RadialFill_Unlit");
		break;
	case UI_FILL_RADIAL_CLOCKWISE:
		m_uiEffectID = Inferno::GetEffectByName("UI_RadialFillClockwise_Unlit");
		break;
	case UI_UNLIT_EFFECT:
		m_uiEffectID = Inferno::GetEffectByName("UI_Unlit");
		break;
	}
}
// === SetRadialOrigin
// IN : RadialOrigin - ID of the origin to use
// OUT: void
//
// Sets the origin for radial fill
void UIRenderer::SetRadialOrigin(RadialOrigins origin)
{
	switch (origin)
	{
	case ORIGIN_LEFT:
		m_RadialOriginInfo.TopLeft = Vector2(-1, 1);
		m_RadialOriginInfo.TopRight = Vector2(1, 1);
		m_RadialOriginInfo.BottomLeft = Vector2(-1, -1);
		m_RadialOriginInfo.BottomRight = Vector2(1, -1);
		break;
	case ORIGIN_BOTTOM:
		m_RadialOriginInfo.TopLeft = Vector2(1, 1);
		m_RadialOriginInfo.TopRight = Vector2(1, -1);
		m_RadialOriginInfo.BottomLeft = Vector2(-1, 1);
		m_RadialOriginInfo.BottomRight = Vector2(-1, -1);
		break;
	case ORIGIN_RIGHT:
		m_RadialOriginInfo.TopLeft = Vector2(1, -1);
		m_RadialOriginInfo.TopRight = Vector2(-1, -1);
		m_RadialOriginInfo.BottomLeft = Vector2(1, 1);
		m_RadialOriginInfo.BottomRight = Vector2(-1, 1);
		break;
	case ORIGIN_TOP:
		m_RadialOriginInfo.TopLeft = Vector2(-1, -1);
		m_RadialOriginInfo.TopRight = Vector2(-1, 1);
		m_RadialOriginInfo.BottomLeft = Vector2(1, -1);
		m_RadialOriginInfo.BottomRight = Vector2(1, 1);
		break;
	}
}

// === SetFillFromLeft
//  IN : bool - Whether or not to fill from the left side.
//  OUT : void
//
//  Sets the correct variables for Filling from Left to Right or Right to Left
//void  UIRenderer::SetFillFromLeft(bool _true)
//{
//	if (m_fImageFill < 0) {
//		m_fImageFill = 1.0f;
//	}
//
//	if (_true) {
//		m_uiEffectID = Inferno::GetEffectByName("UI_Fill_Unlit");
////		m_iFillRatioBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"FillRatio");
//	}
//	else {
//		m_uiEffectID = Inferno::GetEffectByName("UI_FillRight_Unlit");
////		m_iFillRatioBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"FillRightRatio");
//	}
//}

// === SetColor
//  IN : Color - The color offset to use
//  OUT : void
//
//  Sets the color offset of the image.
void UIRenderer::SetColor(Color _color)
{
	m_Color = _color;
}

// === SetTexture
//  IN : Texture* - A pointer to the Texture to use
//  OUT : void
//
//  Sets the Texture for the Image to use.
void UIRenderer::SetTexture(Texture* _texture)
{
	m_pTexture = _texture;
}
// ==================== //