#include "stdafx.h"
#include "CreditsMenuScript.h"

#include "UIElement.h"
#include "InfernoTime.h"
#include "InfernoTimer.h"

#include "ModuleInterface.h"
#include "InputManager.h"

CreditsMenuScript::CreditsMenuScript(UIElement* _owner) : UIComponent(_owner)
{
	m_pTransform = GetTransform();

	m_pSkipMessegeKeyboard = m_pSkipMessegeController = nullptr;

	m_fScrollSpeed = -83.0f;

	const float delayTime = 1.75f;
	m_fDelayTimer.SetTime(delayTime);
	m_fDelayTimer.Reset();

	m_bIsRunnable = false;
	m_bIsSkippable = false;
}

CreditsMenuScript::~CreditsMenuScript()
{
	InfernoEngine_Input::UnregisterEventFromActionBinding("Interact", this, BindActionFunction(&CreditsMenuScript::SkipCredits, this));
	InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&CreditsMenuScript::OnControllerChange, this));
}

void CreditsMenuScript::PostInit()
{

}

void CreditsMenuScript::OnEnable()
{

}

void CreditsMenuScript::SkipCredits(ACTION_STATUS _status)
{
	if (_status != ACTION_STATUS::AS_PRESSED || m_bIsSkippable == false)
	{
		return;
	}

	InfernoEngine_SceneManagement::LoadScene("MainMenuScene");
}

void CreditsMenuScript::Update()
{
	if (m_bIsRunnable == false)
	{
		UpdateDelay();
		return;
	}

	m_pTransform->Translate(Vector2(0.0f, m_fScrollSpeed * Time::DeltaTime));

	if (m_bIsSkippable == false)
	{
		UpdateSkip();
	}

	Vector2 bounds = m_pTransform->GetBounds();

	// get the position that would indicate end of credits (half of image height * -1 to account for +y being down in screen space)
	const float limit = -1120.0f;
	float posY = m_pTransform->GetPosition().y;

	if (posY < limit){
		InfernoEngine_SceneManagement::LoadScene("MainMenuScene");
	}
}

void CreditsMenuScript::UpdateDelay()
{
	if (m_fDelayTimer.IsFinished())
	{
		// start running
		m_bIsRunnable = true;

		// start skip timer
		const float skipTime = 3.0f;
		m_fSkipTimer.SetTime(skipTime);
	}
}

void CreditsMenuScript::UpdateSkip()
{
	if (m_fSkipTimer.IsFinished())
	{
		m_bIsSkippable = true;

		bool usingController = InputManager::UsingController();
		m_pSkipMessegeKeyboard->SetActive(!usingController);
		m_pSkipMessegeController->SetActive(usingController);

		InfernoEngine_Input::RegisterEventToActionBinding("Interact", this, BindActionFunction(&CreditsMenuScript::SkipCredits, this));
		InputManager::RegisterEventToControlChange(this, BindControlChangeFunction(&CreditsMenuScript::OnControllerChange, this));
	}
}

void CreditsMenuScript::SetSkipMessages(UIElement* _keyboardMessege, UIElement* _controllerMessege)
{
	m_pSkipMessegeKeyboard = _keyboardMessege;
	m_pSkipMessegeController = _controllerMessege;
}

void CreditsMenuScript::OnControllerChange(bool _usingController)
{
	m_pSkipMessegeKeyboard->SetActive(!_usingController);
	m_pSkipMessegeController->SetActive(_usingController);
}