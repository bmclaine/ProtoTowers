#include "stdafx.h"
#include "ControllerDisconnectScript.h"
#include "../InfernoUI.h"
#include "InputManager.h"
#include "EventSystem.h"

#define WAIT_TIME 5.0f

ControllerDisconnectScript::ControllerDisconnectScript(UIElement* _owner) : UIComponent(_owner)
{
	m_pDisconnectedWindow = nullptr;
	m_pOKButton = nullptr;
	m_pPrevSelectable = nullptr;
	m_fPrevTimeScale = 1.0f;
}

ControllerDisconnectScript::~ControllerDisconnectScript()
{
}

void ControllerDisconnectScript::OnEnable()
{
	InputManager::RegisterEventToControlChange(this, BindControlChangeFunction(&ControllerDisconnectScript::OnControlChange, this));
	InputManager::RegisterEventToControllerLost(this, BindControllerLostFunction(&ControllerDisconnectScript::OnControllerDisconnect, this));
}
void ControllerDisconnectScript::OnDestroy()
{
	InputManager::UnregisterEventFromControlChange(this, BindControlChangeFunction(&ControllerDisconnectScript::OnControlChange, this));
	InputManager::UnregisterEventFromControllerLost(this, BindControllerLostFunction(&ControllerDisconnectScript::OnControllerDisconnect, this));
}

void ControllerDisconnectScript::OnControlChange(bool usingController)
{
	// Check to see if we are switching from using a controller to not using a controller only
	if (!usingController)
	{
		// Reset the timer, even if it hasn't finished
		m_itControlChangeTimer.SetTime(WAIT_TIME);
	}
}
void ControllerDisconnectScript::OnControllerDisconnect(unsigned int controllerID)
{
	if (controllerID != 0)
		return;

	if (!m_itControlChangeTimer.IsFinished() || InputManager::UsingController())
	{
		// If the window element was set, activate it and set our time scale to 0
		if (m_pDisconnectedWindow)
		{
			m_pDisconnectedWindow->SetActive(true);
			m_pPrevSelectable = Inferno_UI::GetCurrentSelectable();
			Inferno_UI::SetCurrentSelectable(m_pOKButton);
			m_fPrevTimeScale = Time::TimeScale;
			Time::TimeScale = 0.0f;
		}
	}
}
void ControllerDisconnectScript::OKBtn_OnClick()
{
	// If the window element was set, de-activate it and set our time scale back to what it was
	if (m_pDisconnectedWindow)
	{
		m_pDisconnectedWindow->SetActive(false);
		Inferno_UI::SetCurrentSelectable(m_pPrevSelectable);
		Time::TimeScale = m_fPrevTimeScale;
	}
}

void ControllerDisconnectScript::SetDisconnectedWindow(UIElement* window)
{
	m_pDisconnectedWindow = window;
}
void ControllerDisconnectScript::SetOKButton(UIButton* button)
{
	m_pOKButton = button;
}