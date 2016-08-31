#include "stdafx.h"
#include "WinHUDPrefab.h"

#include "../InfernoUI.h"

// === Constructor / Destructor === //
WinHUDPrefab::WinHUDPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pWinImage = new Texture();
	m_pWinImage->SetImage(Inferno::GetImage(L"UI_WinMsg.dds"));
}

WinHUDPrefab::~WinHUDPrefab()
{
	// delete Textures
	delete m_pWinImage;

	// delete the Canvas
	Inferno_UI::DestroyCanvas("HUD");
}
// ================================ //

// === Interface === //
void WinHUDPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void WinHUDPrefab::OnEnable()
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
		image->SetTexture(m_pWinImage);
		image->SetLayer(3);

	AddToView(playerInfo, "HUD");
}
// ================= //

// === Protected Interface === //
IComponent& WinHUDPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

WinHUDPrefab& WinHUDPrefab::operator=(const WinHUDPrefab& _rhs)
{
	return *this;
}

IComponent* WinHUDPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new WinHUDPrefab(gameObject, transform);
}
// =========================== //