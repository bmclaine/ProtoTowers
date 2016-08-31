#include "stdafx.h"
#include "AbilityFlyInPrefab.h"
#include "AbilityFlyIn.h"
#include "../InfernoUI.h"

AbilityFlyInPrefab::AbilityFlyInPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pAbilityTexture = new Texture();
	m_pAbilityTexture->SetImage(Inferno::GetImage(L"StunGernadeAbilityIcon.dds"));
}

AbilityFlyInPrefab::~AbilityFlyInPrefab()
{
	delete m_pAbilityTexture;
}

IComponent* AbilityFlyInPrefab::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	return new AbilityFlyInPrefab(_gameObject, _transform);
}

IComponent& AbilityFlyInPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

AbilityFlyInPrefab& AbilityFlyInPrefab::operator=(const AbilityFlyInPrefab& _rhs)
{
	return *this;
}

void AbilityFlyInPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void AbilityFlyInPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	RectTransform* transform;

	hudCanvas = GetCanvasByName("ExtendedHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("ExtendedHUD", &hudCanvas, &hudCanvasSlot);
	}

	UIElement* abilityFlyInElement = new UIElement();
	abilityFlyInElement->SetName("AbilityFlyIn");
	transform = abilityFlyInElement->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_CENTERLEFT);
	transform->SetPosition(Vector2(0, 128));
	transform->SetBounds(Vector2(300, 50));


	UIRenderer* image = abilityFlyInElement->AddComponent<UIRenderer>();
	image->SetTexture(m_pAbilityTexture);
	image->SetEffect(UI_FILL_EFFECT);
	image->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	image->SetLayer(0);

	AbilityFlyIn* abilityFlyIn = abilityFlyInElement->AddComponent<AbilityFlyIn>();
	abilityFlyIn->SetTexture(m_pAbilityTexture);
	abilityFlyIn->SetRenderer(image);

	AddToView(abilityFlyInElement, "ExtendedHUD");

}

