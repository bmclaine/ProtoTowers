#include "stdafx.h"
#include "FadeAwayEffectPrefab.h"

#include "../InfernoUI.h"
#include "FadeAway.h"

// === Constructor / Destructor === //
FadeAwayEffectPrefab::FadeAwayEffectPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{

}

FadeAwayEffectPrefab::~FadeAwayEffectPrefab()
{
	// delete the Canvas
	Inferno_UI::DestroyCanvas("ExtendedHUD");
}
// ================================ //

// === Interface === //
void FadeAwayEffectPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void FadeAwayEffectPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

	// === Fade Away Effect
	UIElement* fadeAway = new UIElement();
	fadeAway->SetName("Fade Away Effect");
	fadeAway->GetTransform()->SetBounds(Vector2(1024.0f, 768.0f));
	//// Components
		// == FadeAway Component
		fadeAway->AddComponent<FadeAway>();

	AddToView(fadeAway, "ExtendedHUD");
}
// ================= //

// === Protected Interface === //
IComponent& FadeAwayEffectPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

FadeAwayEffectPrefab& FadeAwayEffectPrefab::operator=(const FadeAwayEffectPrefab& _rhs)
{
	return *this;
}

IComponent* FadeAwayEffectPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new FadeAwayEffectPrefab(gameObject, transform);
}
// =========================== //