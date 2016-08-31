#include "stdafx.h"
#include "HelpScreenPrefab.h"

#include "../InfernoUI.h"
#include "HelpScreen.h"

// === Constructor / Destructor === //
HelpScreenPrefab::HelpScreenPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{

}

HelpScreenPrefab::~HelpScreenPrefab()
{
	// delete the Canvas
	Inferno_UI::DestroyCanvas("ExtendedHUD");
}
// ================================ //

// === Interface === //
void HelpScreenPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void HelpScreenPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	RectTransform* transform;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

	// === Help Screen
	UIElement* helpScreen = new UIElement();
	helpScreen->SetName("Help Screen");
	transform = helpScreen->GetTransform();
	transform->SetBounds(Vector2(1024.0f, 768.0f));
	//// Components
		// == HelpScreen Component
		helpScreen->AddComponent<HelpScreen>();

	AddToView(helpScreen, "ExtendedHUD");
}
// ================= //

// === Protected Interface === //
IComponent& HelpScreenPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

HelpScreenPrefab& HelpScreenPrefab::operator=(const HelpScreenPrefab& _rhs)
{
	return *this;
}

IComponent* HelpScreenPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new HelpScreenPrefab(gameObject, transform);
}
// =========================== //