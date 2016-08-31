#include "stdafx.h"
#include "CreditsMenuPrefab.h"
#include "CreditsMenuScript.h"
#include "ModuleInterface.h"
#include "../InfernoUI.h"

CreditsMenuPrefab::CreditsMenuPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pCreditsList = new Texture();
	m_pCreditsList->SetImage(Inferno::GetImage(L"CreditsList.dds"));

	m_pSkipKeyboard = new Texture();
	m_pSkipKeyboard->SetImage(Inferno::GetImage(L"UI_SkipKeyboard.dds"));

	m_pSkipController = new Texture();
	m_pSkipController->SetImage(Inferno::GetImage(L"UI_SkipController.dds"));
}

CreditsMenuPrefab::~CreditsMenuPrefab()
{
	delete m_pCreditsList;
	delete m_pSkipKeyboard;
	delete m_pSkipController;
}

CreditsMenuPrefab& CreditsMenuPrefab::operator=(const CreditsMenuPrefab& _rhs)
{
	return *this;
}

IComponent* CreditsMenuPrefab::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	CreditsMenuPrefab* CMP = new CreditsMenuPrefab(_gameObject, _transform);

	return CMP;
}

void CreditsMenuPrefab::Init()
{
	
}

void CreditsMenuPrefab::PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap)
{

}

void CreditsMenuPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* creditsCanvas = nullptr;

	unsigned int creditsCanvasSlot;

	creditsCanvas = GetCanvasByName("Credits");
	if (creditsCanvas == nullptr)
	{
		CreateNewCanvas("Credits", &creditsCanvas, &creditsCanvasSlot);
	}

	// Credits List Image
	UIElement* creditsElement = new UIElement();
	creditsElement->SetName("CreditsList");

	RectTransform* transform = creditsElement->GetTransform();
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	Vector2 dimensions = Vector2(1024.0f, 768.0f); //InfernoEngine_Graphics::GetWindowDimensions();
	dimensions.y *= 3.0f;
	transform->SetBounds(dimensions);

	UIRenderer* image = creditsElement->AddComponent<UIRenderer>();
	image->SetTexture(m_pCreditsList);
	image->SetLayer(1);

	CreditsMenuScript* creditsScript = creditsElement->AddComponent<CreditsMenuScript>();

	// Skip with Keyboard Image
	UIElement* skipCreditsKeyboard = new UIElement();
	skipCreditsKeyboard->SetName("SkipCreditsKeyboard");
	skipCreditsKeyboard->SetActive(false);

	transform = skipCreditsKeyboard->GetTransform();
	//transform->SetParent(creditsElement->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMRIGHT);
	transform->SetBounds(Vector2(224.0f, 128.0f));
	transform->SetPosition(-112.0f, -64.0f);

	image = skipCreditsKeyboard->AddComponent<UIRenderer>();
	image->SetTexture(m_pSkipKeyboard);
	image->SetLayer(2);

	// Skip with Controller Image
	UIElement* skipCreditsController = new UIElement();
	skipCreditsController->SetName("SkipCreditsController");
	skipCreditsController->SetActive(false);
	transform = skipCreditsController->GetTransform();
	//transform->SetParent(creditsElement->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMRIGHT);
	transform->SetBounds(Vector2(224.0f, 128.0f));
	transform->SetPosition(-112.0f, -64.0f);

	image = skipCreditsController->AddComponent<UIRenderer>();
	image->SetTexture(m_pSkipController);
	image->SetLayer(2);

	creditsScript->SetSkipMessages(skipCreditsKeyboard, skipCreditsController);

	AddToView(creditsElement, "Credits");
	AddToView(skipCreditsKeyboard, "Credits");
	AddToView(skipCreditsController, "Credits");

	// offset needed to center credits at start. can't seem to get this number mathematically.
	float yPosOffset = 1425.0f;
	//Vector2 scale = transform->GetCanvasScale();
	//yPosOffset *= scale.y;
	transform = creditsElement->GetTransform();
	transform->SetPosition(0.0f, yPosOffset);
}