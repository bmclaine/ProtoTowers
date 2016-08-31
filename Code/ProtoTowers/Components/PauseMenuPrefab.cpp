#include "stdafx.h"
#include "PauseMenuPrefab.h"

#include "../InfernoUI.h"
#include "PauseMenu.h"

#define BUTTON_WIDTH 170
#define BUTTON_HEIGHT 119

// === Constructor / Destructor === //
PauseMenuPrefab::PauseMenuPrefab(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pPauseMenuImage = new Texture();
	m_pPauseMenuImage->SetImage(Inferno::GetImage(L"UI_Paused.dds"));
	m_pBtnImageN = new Texture();
	m_pBtnImageN->SetImage(Inferno::GetImage(L"UI_MM_ButtonN.dds"));
	m_pBtnImageS = new Texture();
	m_pBtnImageS->SetImage(Inferno::GetImage(L"UI_MM_ButtonS.dds"));
	m_pPMResumeImage = new Texture();
	m_pPMResumeImage->SetImage(Inferno::GetImage(L"UI_PM_Resume.dds"));
	m_pOptionTxtImage = new Texture();
	m_pOptionTxtImage->SetImage(Inferno::GetImage(L"UI_PM_Options.dds"));
	m_pPMHowToImage = new Texture();
	m_pPMHowToImage->SetImage(Inferno::GetImage(L"UI_PM_HowTo.dds"));
	m_pPMQuitImage = new Texture();
	m_pPMQuitImage->SetImage(Inferno::GetImage(L"UI_PM_Quit.dds"));
	m_pControlsInfoImage = new Texture();
	m_pControlsInfoImage->SetImage(Inferno::GetImage(L"UI_ControlsInfo.dds"));
}

PauseMenuPrefab::~PauseMenuPrefab()
{
	// delete Textures
	delete m_pPauseMenuImage;
	delete m_pBtnImageN;
	delete m_pBtnImageS;
	delete m_pPMResumeImage,
	delete m_pOptionTxtImage,
	delete m_pPMHowToImage,
	delete m_pPMQuitImage;
	delete m_pControlsInfoImage;

	// delete the Canvas
	Inferno_UI::DestroyCanvas("ExtendedHUD");
}
// ================================ //

// === Interface === //
void PauseMenuPrefab::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void PauseMenuPrefab::OnEnable()
{
	using namespace Inferno_UI;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	RectTransform* transform = nullptr;
	UIRenderer* image = nullptr;
	UIText* uiText = nullptr;

	hudCanvas = GetCanvasByName("PauseHUD");
	if (hudCanvas == nullptr) {
		CreateNewCanvas("PauseHUD", &hudCanvas, &hudCanvasSlot);
	}

	// === Pause Menu
	UIElement* pauseMenu = new UIElement();
	pauseMenu->SetName("Pause Menu");
	//// Components
		// == PauseMenu Component
		PauseMenu* pMenuCom = pauseMenu->AddComponent<PauseMenu>();

	//// Children
		// == Paused Image
		UIElement* pauseImage = new UIElement();
		pauseImage->SetName("PauseMenu Element");
		transform = pauseImage->GetTransform();
		transform->SetParent(pauseMenu->GetTransform());
		transform->SetPosition(0.0f, -150.0f);
		transform->SetBounds(Vector2(400.0f, 80.0f));
		//// Components
			// == UIRenderer (Pause Image)
			image = pauseImage->AddComponent<UIRenderer>();
			image->SetTexture(m_pPauseMenuImage);
		// == Resume Btn
		UIElement* resumeElement = new UIElement();
		resumeElement->SetName("Resume Btn");
		transform = resumeElement->GetTransform();
		transform->SetParent(pauseImage->GetTransform());
		transform->SetPosition(0.0f, 90.0f);
		transform->SetBounds(Vector2(BUTTON_WIDTH, BUTTON_HEIGHT));
		//// Components
			// == UIButton (Resume Btn)
			UIButton* resumeBtn = resumeElement->AddComponent<UIButton>();
			resumeBtn->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
			resumeBtn->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
			resumeElement->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&PauseMenu::OnResumeBtnClick, pMenuCom));
			pMenuCom->SetFirstSelectable(resumeBtn);
			// == UIRenderer (Resume Text)
			UIElement* resumeText = new UIElement();
			transform = resumeText->GetTransform();
			transform->SetParent(resumeElement->GetTransform());
			transform->SetBounds(Vector2(160.0f, 72.0f));
			image = resumeText->AddComponent<UIRenderer>();
			image->SetTexture(m_pPMResumeImage);

		// == Options Btn
		UIElement* optionsElement = new UIElement();
		optionsElement->SetName("Options Btn");
		transform = optionsElement->GetTransform();
		transform->SetParent(pauseImage->GetTransform());
		transform->SetPosition(0.0f, 190.0f);
		transform->SetBounds(Vector2(BUTTON_WIDTH, BUTTON_HEIGHT));
		//// Components
			// == UIButton (Options Btn)
			UIButton* optionsBtn = optionsElement->AddComponent<UIButton>();
			optionsBtn->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
			optionsBtn->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
			optionsElement->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&PauseMenu::OnOptionsBtnClick, pMenuCom));
			// == UIRenderer (Options Text)
			UIElement* optionsText = new UIElement();
			transform = optionsText->GetTransform();
			transform->SetParent(optionsElement->GetTransform());
			transform->SetBounds(Vector2(130.0f, 50.0f));
			image = optionsText->AddComponent<UIRenderer>();
			image->SetTexture(m_pOptionTxtImage);

		// == How to Play Btn
		UIElement* howToElement = new UIElement();
		howToElement->SetName("HowToPlay Btn");
		transform = howToElement->GetTransform();
		transform->SetParent(pauseImage->GetTransform());
		transform->SetPosition(0.0f, 290.0f);
		transform->SetBounds(Vector2(BUTTON_WIDTH, BUTTON_HEIGHT));
		//// Components
			// == UIButton (Resume Btn)
			UIButton* howToBtn = howToElement->AddComponent<UIButton>();
			howToBtn->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
			howToBtn->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
			howToElement->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&PauseMenu::OnHowToPlayBtnClick, pMenuCom));
			// == UIRenderer (Resume Text)
			UIElement* howToText = new UIElement;
			transform = howToText->GetTransform();
			transform->SetParent(howToBtn->GetTransform());
			transform->SetBounds(Vector2(150.0f, 55.0f));
			image = howToText->AddComponent<UIRenderer>();
			image->SetTexture(m_pPMHowToImage);

		// == Quit Btn
		UIElement* quitElement = new UIElement();
		quitElement->SetName("Quit Btn");
		transform = quitElement->GetTransform();
		transform->SetParent(pauseImage->GetTransform());
		transform->SetPosition(0.0f, 390.0f);
		transform->SetBounds(Vector2(BUTTON_WIDTH, BUTTON_HEIGHT));
		//// Components
			// == UIButton (Resume Btn)
			UIButton* quitBtn = quitElement->AddComponent<UIButton>();
			quitBtn->SetNormalImage(m_pBtnImageN, Color(0.5f, 0.5f, 0.5f, 0.9f));
			quitBtn->SetHoverImage(m_pBtnImageS, Color(0.8f, 0.8f, 0.8f, 0.9f));
			quitElement->GetEventSystem()->RegisterEvent(UI_EVENTS::UI_EVENT_ONCLICK, std::bind(&PauseMenu::OnQuitBtnQuit, pMenuCom));
			// == UIRenderer (Resume Text)
			UIElement* quitText = new UIElement;
			transform = quitText->GetTransform();
			transform->SetParent(quitElement->GetTransform());
			transform->SetBounds(Vector2(160.0f, 55.0f));
			image = quitText->AddComponent<UIRenderer>();
			image->SetTexture(m_pPMQuitImage);

		// == Controls Info
		UIElement* controlsImage = new UIElement();
		controlsImage->SetName("PM Controls Info");
		transform = controlsImage->GetTransform();
		transform->SetParent(pauseImage->GetTransform());
		transform->SetPosition(75.0f, 490.0f);
		transform->SetBounds(Vector2(384.0f, 48.0f));
		//// Components
			// == UIRenderer (Controls Image)
			image = controlsImage->AddComponent<UIRenderer>();
			image->SetTexture(m_pControlsInfoImage);

	//// Post Setup
	pMenuCom->SetFirstSelectable(resumeBtn);
	resumeBtn->SetUpSelectable(quitBtn); resumeBtn->SetDownSelectable(optionsBtn);
	optionsBtn->SetUpSelectable(resumeBtn); optionsBtn->SetDownSelectable(howToBtn);
	howToBtn->SetUpSelectable(optionsBtn); howToBtn->SetDownSelectable(quitBtn);
	quitBtn->SetUpSelectable(howToBtn); quitBtn->SetDownSelectable(resumeBtn);
	AddToView(pauseMenu, "PauseHUD");
}
// ================= //

// === Protected Interface === //
IComponent& PauseMenuPrefab::operator=(IComponent& _rhs)
{
	return *this;
}

PauseMenuPrefab& PauseMenuPrefab::operator=(const PauseMenuPrefab& _rhs)
{
	return *this;
}

IComponent* PauseMenuPrefab::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	return new PauseMenuPrefab(gameObject, transform);
}
// =========================== //