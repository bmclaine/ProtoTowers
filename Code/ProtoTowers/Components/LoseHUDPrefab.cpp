#include "stdafx.h"
#include "LoseHUDPrefab.h"

#include "../InfernoUI.h"

// === Constructor / Destructor === //
LoseHUDPrefab::LoseHUDPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pLoseImage = new Texture;
	m_pLoseImage->SetImage(Inferno::GetImage(L"UI_LoseMsg.dds"));
}

LoseHUDPrefab::~LoseHUDPrefab()
{
	// delete Textures
	delete m_pLoseImage;

	// delete the Canvas
	Inferno_UI::DestroyCanvas("HUD");
}
// ================================ //

// === Interface === //
void LoseHUDPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void LoseHUDPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	hudCanvas = GetCanvasByName("HUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("HUD", &hudCanvas, &hudCanvasSlot);
	}

	SetTargetResolution(1024, 768);

	// === Win Info Element
	UIElement* playerInfo = new UIElement();
	RectTransform* transform = playerInfo->GetTransform();
	transform->SetBounds(Vector2(700, 140));
	//// Components
		UIRenderer* image = playerInfo->AddComponent<UIRenderer>();
		image->SetTexture(m_pLoseImage);
		image->SetLayer(3);

	AddToView(playerInfo, "HUD");
}
// ================= //

// === Protected Interface === //
IComponent& LoseHUDPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

LoseHUDPrefab& LoseHUDPrefab::operator=(const LoseHUDPrefab& _rhs)
{
	return *this;
}

IComponent* LoseHUDPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new LoseHUDPrefab(gameObject, transform);
}
// =========================== //