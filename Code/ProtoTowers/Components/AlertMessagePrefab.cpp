#include "stdafx.h"
#include "AlertMessagePrefab.h"

#include "AlertMessage.h"
#include "UIComponents\RectTransform.h"
#include "UIElement.h"

// ===== Constructor / Destructor ===== //
AlertMessagePrefab::AlertMessagePrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{

}

AlertMessagePrefab::~AlertMessagePrefab()
{

}
// ==================================== //

// ===== Interface ===== //
void AlertMessagePrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void AlertMessagePrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

	UIElement* AlertMessageElement = new UIElement();
	AlertMessageElement->SetName("Alert Message");
	RectTransform* transform = AlertMessageElement->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetYPosition(175.0f);
	transform->SetBounds(Vector2(500.0f, 30.0f));
	//// Components
		// == AlertMessage Component
		AlertMessage* aMsg = AlertMessageElement->AddComponent<AlertMessage>();

	AddToView(AlertMessageElement, "ExtendedHUD");
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& AlertMessagePrefab::operator=(IComponent& _rhs)
{
	return *this;
}

AlertMessagePrefab& AlertMessagePrefab::operator=(const AlertMessagePrefab& _rhs)
{
	return *this;
}

IComponent* AlertMessagePrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	AlertMessagePrefab* newPrefab = new AlertMessagePrefab(gameObject, transform);
	*newPrefab = *this;

	return newPrefab;
}
// =============================== //