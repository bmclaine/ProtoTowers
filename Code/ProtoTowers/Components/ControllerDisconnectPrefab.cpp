#include "stdafx.h"
#include "ControllerDisconnectPrefab.h"
#include "ControllerDisconnectScript.h"
#include "Assets\Texture.h"

#include "Engine.h"
#include "ModuleInterface.h"
#include "../InfernoUI.h"

ControllerDisconnectPrefab::ControllerDisconnectPrefab(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	int background = Inferno::GetImage(L"ControllerDisconnectedBackground");

	if (background == -1)
		background = InfernoEngine_Graphics::CreateSolidColorImage(L"ControllerDisconnectedBackground", 300, 150, 2, Color(0.0f, 0.0f, 0.0f, 0.4f), Color(0.0f, 0.0f, 0.0f, 1.0f));

	m_pBackground = new Texture();
	m_pBackground->SetImage(background);
	m_pDpDwnImageN = new Texture();
	m_pDpDwnImageN->SetImage(Inferno::GetImage(L"UI_MM_ButtonN.dds"));
	m_pDpDwnImageH = new Texture();
	m_pDpDwnImageH->SetImage(Inferno::GetImage(L"UI_MM_ButtonN.dds"));
	m_pDpDwnImageS = new Texture();
	m_pDpDwnImageS->SetImage(Inferno::GetImage(L"UI_MM_ButtonS.dds"));
	m_pOKText = new Texture();
	m_pOKText->SetImage(Inferno::GetImage(L"UI_OK_Text.dds"));
}

ControllerDisconnectPrefab::~ControllerDisconnectPrefab()
{
	SAFE_DELETE(m_pBackground);
	SAFE_DELETE(m_pDpDwnImageN);
	SAFE_DELETE(m_pDpDwnImageH);
	SAFE_DELETE(m_pDpDwnImageS);
	SAFE_DELETE(m_pOKText);
}

ControllerDisconnectPrefab& ControllerDisconnectPrefab::operator=(ControllerDisconnectPrefab&)
{
	return *this;
}
IComponent* ControllerDisconnectPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new ControllerDisconnectPrefab(gameObject, transform);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void ControllerDisconnectPrefab::Init() {}
// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void ControllerDisconnectPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) {}

void ControllerDisconnectPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* disconnectedCanvas = nullptr;
	unsigned int disconnectedCanvasSlot;

	disconnectedCanvas = GetCanvasByName("Controller");
	if (disconnectedCanvas == nullptr) {
		CreateNewCanvas("Controller", &disconnectedCanvas, &disconnectedCanvasSlot);
		disconnectedCanvas = GetCanvasByName("Controller");
	}

	// Set up the Disconnected Controller script
	UIElement* ButtonsElement = new UIElement();
	ControllerDisconnectScript* controllerScript = ButtonsElement->AddComponent<ControllerDisconnectScript>();

	AddToView(ButtonsElement, "Controller");

	// Background
	UIElement* Background = new UIElement();
	RectTransform* transform = Background->GetTransform();
	transform->SetBounds(Vector2(300, 150));
	controllerScript->SetDisconnectedWindow(Background);

	UIRenderer* image = Background->AddComponent<UIRenderer>();
	image->SetTexture(m_pBackground);
	image->SetLayer(50);

	// Disconnected Text
	UIElement* DisconnectedText = new UIElement();
	transform = DisconnectedText->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_TOPCENTER);
	transform->SetBounds(Vector2(290, 64));
	transform->SetPosition(Vector2(0, 42));

	UIText* Text = DisconnectedText->AddComponent<UIText>();
	Text->SetAlignment(TextAlignment::CENTER);
	Text->SetParagraphAlignment(PARAGRAPH_CENTER);
	Text->SetFontSize(24);
	Text->SetLayer(51);
	Text->SetText(L"Controller Was Disconnected!");

	// OK Button
	UIElement* OKButton = new UIElement();
	transform = OKButton->GetTransform();
	transform->SetParent(Background->GetTransform());
	transform->SetAnchors(Anchors::ANCHOR_BOTTOMCENTER);
	transform->SetBounds(Vector2(192, 64));
	transform->SetPosition(Vector2(0, -42));

	UIRenderer* OKText = OKButton->AddComponent<UIRenderer>();
	OKText->SetTexture(m_pOKText);
	OKText->SetLayer(52);

	UIButton* okButton = OKButton->AddComponent<UIButton>();
	okButton->SetNormalImage(m_pDpDwnImageN);
	okButton->SetHoverImage(m_pDpDwnImageH);
	okButton->SetPressedImage(m_pDpDwnImageS);
	okButton->SetLayer(51);
	OKButton->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&ControllerDisconnectScript::OKBtn_OnClick, controllerScript));
	controllerScript->SetOKButton(okButton);

	AddToView(Background, "Controller");
	Background->SetActive(false);
}