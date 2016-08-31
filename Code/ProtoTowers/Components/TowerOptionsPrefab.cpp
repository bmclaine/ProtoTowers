#include "stdafx.h"
#include "TowerOptionsPrefab.h"

#include "TowerOptionsScript.h"
#include "UIElement.h"

// ===== Constructor / Destructor ===== //
TowerOptionsPrefab::TowerOptionsPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{

}

TowerOptionsPrefab::~TowerOptionsPrefab()
{

}
// ==================================== //

// ===== Interface ===== //
void TowerOptionsPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void TowerOptionsPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

	UIElement* towerOptions = new UIElement();
	towerOptions->SetName("Tower Options");
	towerOptions->GetTransform()->SetBounds(Vector2(1024.0f, 768.0f));
	//// Components
		// == TowerOptionsScript
		towerOptions->AddComponent<TowerOptionsScript>();

	AddToView(towerOptions, "ExtendedHUD");

	towerOptions->SetActive(false);
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& TowerOptionsPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

TowerOptionsPrefab& TowerOptionsPrefab::operator=(const TowerOptionsPrefab& _rhs)
{
	return *this;
}

IComponent* TowerOptionsPrefab::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	TowerOptionsPrefab* newTOP = new TowerOptionsPrefab(_gameObject, _transform);
	*newTOP = *this;

	return newTOP;
}
// =============================== //
