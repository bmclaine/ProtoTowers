#include "stdafx.h"
#include "MainMenuScript.h"
#include "OptionsMenuPrefab.h"

#include "../Public/ModuleInterface.h"
#include "Engine.h"
#include "InfernoTime.h"
#include "InputManager.h"
#include "UIElement.h"
#include "UIComponents\UIEventSystem.h"
#include "UIComponents\UICanvas.h"

// ===== Constructor / Destructor ===== //
MainMenuScript::MainMenuScript(UIElement* _owner) : UIComponent(_owner)
{
	m_pInitialSelectable = nullptr;
}

MainMenuScript::~MainMenuScript()
{

}
// =================================== //

// ===== Interface ===== //
void MainMenuScript::PostInit()
{
	
}

void MainMenuScript::OnEnable()
{
	InputManager::s_CenterMouse = false;
	InputManager::ShowMouse(true);
	Inferno_UI::SetCurrentSelectable(m_pInitialSelectable);
}
// ===================== //

// ===== Button Events ===== //
void MainMenuScript::PlayBtn_OnClick()
{
	Time::TimeScale = 1.0f;

#if 1
	//InfernoEngine_SceneManagement::LoadScene("GameScene");
	//InfernoEngine_SceneManagement::LoadScene("NewGameScene");
	InfernoEngine_SceneManagement::LoadScene("Tutorial");
#else
	InfernoEngine_SceneManagement::LoadScene("Jack_TestScene");
#endif
}

void MainMenuScript::OptionBtn_OnClick()
{
	using namespace Inferno_UI;

	UICanvas* menuCanvas = GetCanvasByName("Menu");
	UICanvas* optionsCanvas = GetCanvasByName("Options");

	if (menuCanvas && optionsCanvas)
	{
		menuCanvas->GetUIElement()->SetActive(false);
		optionsCanvas->GetUIElement()->SetActive(true);
	}
}

void MainMenuScript::CreditsBtn_OnClick()
{
	InfernoEngine_SceneManagement::LoadScene("CreditsMenu");
}

void MainMenuScript::QuitBtn_OnClick()
{
	Inferno::ExitApp();
}
// ========================= //

// === Mutators === //
void MainMenuScript::SetInitialSelectable(UISelectable* initial)
{
	m_pInitialSelectable = initial;
}
// ================ //