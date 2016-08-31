#include "stdafx.h"
#include "MainMenuPrefab.h"

#include "../InfernoUI.h"
#include "MainMenuScript.h"
#include "CreditsMenuScript.h"

#define BUTTON_WIDTH 192
#define BUTTON_HEIGHT 135

#define TEXT_WIDTH 57
#define TEXT_HEIGHT 40

// === Constructor / Destructor === //
MainMenuPrefab::MainMenuPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pTitleImage = new Texture();
	m_pTitleImage->SetImage(Inferno::GetImage(L"UI_ProtoTowers.dds"));
	m_pBtnImageN = new Texture();
	m_pBtnImageN->SetImage(Inferno::GetImage(L"UI_MM_ButtonN.dds"));
	m_pBtnImageS = new Texture();
	m_pBtnImageS->SetImage(Inferno::GetImage(L"UI_MM_ButtonS.dds"));
	m_pPlayTxtImage = new Texture();
	m_pPlayTxtImage->SetImage(Inferno::GetImage(L"UI_MM_PlayText.dds"));
	m_pOptionTxtImage = new Texture();
	m_pOptionTxtImage->SetImage(Inferno::GetImage(L"UI_Options_Text.dds"));
	m_pCreditsTxtImage = new Texture();
	m_pCreditsTxtImage->SetImage(Inferno::GetImage(L"UI_MM_CreditsText.dds"));
	m_pQuitTxtImage = new Texture();
	m_pQuitTxtImage->SetImage(Inferno::GetImage(L"UI_MM_QuitText.dds"));
}

MainMenuPrefab::~MainMenuPrefab()
{
	// delete Textures
	delete m_pTitleImage;
	delete m_pBtnImageN;
	delete m_pBtnImageS;
	delete m_pPlayTxtImage;
	delete m_pOptionTxtImage;
	delete m_pCreditsTxtImage;
	delete m_pQuitTxtImage;

	// delete the Canvas
	Inferno_UI::DestroyCanvas("Menu");
}
// ================================ //

// === Interface === //
void MainMenuPrefab::Init()
{
	
}
void MainMenuPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	
}

void MainMenuPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* menuCanvas = nullptr;
	unsigned int menuCanvasSlot;

	menuCanvas = GetCanvasByName("Menu");
	if (menuCanvas == nullptr) {
		CreateNewCanvas("Menu", &menuCanvas, &menuCanvasSlot);
	}

	// === Title
	UIElement* gameTitle = new UIElement();
	//// Components
	// == UIRenderer
	UIRenderer* image = gameTitle->AddComponent<UIRenderer>();
	image->SetTexture(m_pTitleImage);
	RectTransform* transform = gameTitle->GetTransform();
	transform->SetPosition(0, -192);
	transform->SetBounds(Vector2(700, 140));

	AddToView(gameTitle, "Menu");

	// === Buttons Element
	UIElement* menuButtons = new UIElement();
	//// Components
	// == MenuScript
	MainMenuScript* menuScript = menuButtons->AddComponent<MainMenuScript>();

	AddToView(menuButtons, "Menu");

	// === Play Button
	UIElement* playBtnObj = new UIElement();
	playBtnObj->SetName("MM Play Btn");
	transform = playBtnObj->GetTransform();
	transform->SetPosition(0, -58);
	transform->SetBounds(Vector2(BUTTON_WIDTH, BUTTON_HEIGHT));
	//// Components
	// == UIButton
	UIButton* playBtn = playBtnObj->AddComponent<UIButton>();
	playBtn->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	playBtn->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	playBtn->SetLayer(0);
	//// Post Setup
	playBtnObj->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&MainMenuScript::PlayBtn_OnClick, menuScript));
	//// Children
	// == Play Text
	UIElement* playTxt = new UIElement();
	transform = playTxt->GetTransform();
	transform->SetParent(playBtnObj->GetTransform());
	transform->SetBounds(Vector2(TEXT_WIDTH, TEXT_HEIGHT));
	//// Components
	// == UIRenderer
	UIRenderer* playRenderer = playTxt->AddComponent<UIRenderer>();
	playRenderer->SetTexture(m_pPlayTxtImage);
	playRenderer->SetLayer(2);

	AddToView(playBtnObj, "Menu");

	// === Options Button
	UIElement* optionsBtnObj = new UIElement();
	optionsBtnObj->SetName("MM Options Btn");
	transform = optionsBtnObj->GetTransform();
	transform->SetPosition(0, 50);
	transform->SetBounds(Vector2(BUTTON_WIDTH, BUTTON_HEIGHT));
	//// Components
	// == UIButton
	UIButton* optionsBtn = optionsBtnObj->AddComponent<UIButton>();
	optionsBtn->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	optionsBtn->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	optionsBtn->SetLayer(0);
	//// Post Setup
	optionsBtnObj->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&MainMenuScript::OptionBtn_OnClick, menuScript));
	//// Children
	// == Quit Text
	UIElement* optionsTxt = new UIElement();
	transform = optionsTxt->GetTransform();
	transform->SetParent(optionsBtnObj->GetTransform());
	transform->SetBounds(Vector2(TEXT_WIDTH + 25, TEXT_HEIGHT));
	//// Components
	// == UIRenderer
	UIRenderer* optionsRenderer = optionsTxt->AddComponent<UIRenderer>();
	optionsRenderer->SetTexture(m_pOptionTxtImage);
	optionsRenderer->SetLayer(2);

	AddToView(optionsBtnObj, "Menu");

	// === Credits Button
	UIElement* creditsBtnObj = new UIElement();
	creditsBtnObj->SetName("MM Credits Btn");
	transform = creditsBtnObj->GetTransform();
	transform->SetPosition(0, 158);
	transform->SetBounds(Vector2(BUTTON_WIDTH, BUTTON_HEIGHT));
	//// Components
	// == UIButton
	UIButton* creditsBtn = creditsBtnObj->AddComponent<UIButton>();
	creditsBtn->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	creditsBtn->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	creditsBtn->SetLayer(0);
	//// Post Setup
	creditsBtnObj->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&MainMenuScript::CreditsBtn_OnClick, menuScript));
	//// Children
	// == Quit Text
	UIElement* creditsTxt = new UIElement();
	transform = creditsTxt->GetTransform();
	transform->SetParent(creditsBtnObj->GetTransform());
	transform->SetBounds(Vector2(TEXT_WIDTH + 20, TEXT_HEIGHT));
	//// Components
	// == UIRenderer
	UIRenderer* creditsRenderer = creditsTxt->AddComponent<UIRenderer>();
	creditsRenderer->SetTexture(m_pCreditsTxtImage);
	creditsRenderer->SetLayer(2);

	AddToView(creditsBtnObj, "Menu");

	// === Quit Button
	UIElement* quitBtnObj = new UIElement();
	quitBtnObj->SetName("MM Quit Btn");
	transform = quitBtnObj->GetTransform();
	transform->SetPosition(0, 266);
	transform->SetBounds(Vector2(BUTTON_WIDTH, BUTTON_HEIGHT));
	//// Components
	// == UIButton
	UIButton* quitBtn = quitBtnObj->AddComponent<UIButton>();
	quitBtn->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
	quitBtn->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
	quitBtn->SetLayer(0);
	//// Post Setup
	quitBtnObj->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&MainMenuScript::QuitBtn_OnClick, menuScript));
	//// Children
	// == Quit Text
	UIElement* quitTxt = new UIElement();
	transform = quitTxt->GetTransform();
	transform->SetParent(quitBtnObj->GetTransform());
	transform->SetBounds(Vector2(TEXT_WIDTH, TEXT_HEIGHT));
	//// Components
	// == UIRenderer
	UIRenderer* quitRenderer = quitTxt->AddComponent<UIRenderer>();
	quitRenderer->SetTexture(m_pQuitTxtImage);
	quitRenderer->SetLayer(2);

	menuScript->SetInitialSelectable(playBtn);

	// === Navigation
	playBtn->SetUpSelectable(quitBtn);
	playBtn->SetDownSelectable(optionsBtn);
	optionsBtn->SetUpSelectable(playBtn);
	optionsBtn->SetDownSelectable(creditsBtn);
	creditsBtn->SetUpSelectable(optionsBtn);
	creditsBtn->SetDownSelectable(quitBtn);
	quitBtn->SetUpSelectable(creditsBtn);
	quitBtn->SetDownSelectable(playBtn);

	AddToView(quitBtnObj, "Menu");

	Inferno_UI::SetCurrentSelectable(playBtn);
}
// ================= //

// === Protected Interface === //
IComponent& MainMenuPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

MainMenuPrefab& MainMenuPrefab::operator=(const MainMenuPrefab& _rhs)
{
	return *this;
}

IComponent* MainMenuPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new MainMenuPrefab(gameObject, transform);
}
// =========================== //