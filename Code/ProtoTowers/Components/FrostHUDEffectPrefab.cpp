#include "stdafx.h"
#include "FrostHUDEffectPrefab.h"

#include "../InfernoUI.h"
#include "FrostHUDEffect.h"

// === Constructor / Destructor === //
FrostHUDEffectPrefab::FrostHUDEffectPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pFrostImage = new Texture();
	m_pFrostImage->SetImage(Inferno::GetImage(L"UI_FrostEffect.dds"));
}

FrostHUDEffectPrefab::~FrostHUDEffectPrefab()
{
	// delete textures
	delete m_pFrostImage;

	// delete the Canvas
	Inferno_UI::DestroyCanvas("ExtendedHUD");
}
// ================================ //

// === Interface === //
void FrostHUDEffectPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void FrostHUDEffectPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	RectTransform* transform;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

	// === Freeze HUD Effect
	UIElement* freezeHUDEffect = new UIElement();
	freezeHUDEffect->SetName("Freeze HUD Effect");
	transform = freezeHUDEffect->GetTransform();
	//// Components
		// == UIRenderer ( Freeze HUD Overlay )
		UIRenderer* image = freezeHUDEffect->AddComponent<UIRenderer>();
		image->SetTexture(m_pFrostImage);
		image->SetColor(Color(1.0f, 1.0f, 1.0f, 0.6f));
		// == FrostHUDEffect Component
		freezeHUDEffect->AddComponent<FrostHUDEffect>();

	AddToView(freezeHUDEffect, "ExtendedHUD");
	image->SetEnabled(false);
}
// ================= //

// === Protected Interface === //
IComponent& FrostHUDEffectPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

FrostHUDEffectPrefab& FrostHUDEffectPrefab::operator=(const FrostHUDEffectPrefab& _rhs)
{
	return *this;
}

IComponent* FrostHUDEffectPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new FrostHUDEffectPrefab(gameObject, transform);
}
// =========================== //