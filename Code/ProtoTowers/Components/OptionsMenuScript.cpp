#include "stdafx.h"
#include "OptionsMenuScript.h"
#include "MainMenuPrefab.h"
#include "../InfernoUI.h"
#include "UIElement.h"
#include "EventSystem.h"
#include "../Core/WWise_IDs.h"
#include "ThreadPool\WorkerTypes\WorkerThreadConfigSave.h"

#include "GlobalIncludes.h"
#include "Engine.h"
#include "ModuleInterface.h"

#include <iomanip>
#include <sstream>

OptionsMenuScript::OptionsMenuScript(UIElement* _owner) : UIComponent(_owner)
{
	m_sPrevCanvas = "";
	m_itAcceptTime.SetOnTimerEndFunction(std::bind(&OptionsMenuScript::CancelBtn_OnClick, this));
	m_pInitialSelectable = nullptr;
	m_pAcceptInitialSelectable = nullptr;
	m_pAcceptMenuElement = nullptr;
	m_pAcceptTimeText = nullptr;
	m_pMainMenu = nullptr;

	m_pResolutionDropdown = nullptr;
	m_pWindowDropdown = nullptr;
	m_pVSyncCheckbox = nullptr;
	m_pAnisotropicDropdown = nullptr;
	m_pGammaSlider = nullptr;
	m_pGammaText = nullptr;
	m_pMasterVolumeSlider = nullptr;
	m_pMasterVolumeText = nullptr;
	m_pMusicVolumeSlider = nullptr;
	m_pMusicVolumeText = nullptr;
	m_pSFXVolumeSlider = nullptr;
	m_pSFXVolumeText = nullptr;

	m_bUILocked = false;
}

OptionsMenuScript::~OptionsMenuScript()
{
}

void OptionsMenuScript::PostInit()
{

}

void OptionsMenuScript::Update()
{
	if (!m_itAcceptTime.IsFinished())
		m_pAcceptTimeText->SetText(std::to_wstring((int)m_itAcceptTime.TimeRemaining()));
}

void OptionsMenuScript::OnEnable()
{
	Inferno_UI::SetCurrentSelectable(m_pInitialSelectable);

	InfernoEngine_Input::RegisterEventToActionBinding("SecondaryInteract", this, BindActionFunction(&OptionsMenuScript::OnBackHit, this));
	EventSystem::GetInstance()->RegisterForEvent("UILocked", this, std::bind(&OptionsMenuScript::OnUILocked, this));
	EventSystem::GetInstance()->RegisterForEvent("UIUnlocked", this, std::bind(&OptionsMenuScript::OnUIUnlocked, this));
	EventSystem::GetInstance()->RegisterForEvent("FORCED_FULLSCREEN_CHANGE", this, std::bind(&OptionsMenuScript::OnForcedStyleChange, this, std::placeholders::_1));
}
void OptionsMenuScript::OnDisable()
{
	InfernoEngine_Input::UnregisterEventFromActionBinding("SecondaryInteract", this, BindActionFunction(&OptionsMenuScript::OnBackHit, this));
	EventSystem::GetInstance()->UnregisterFromEvent("UILocked", this, std::bind(&OptionsMenuScript::OnUILocked, this));
	EventSystem::GetInstance()->UnregisterFromEvent("UIUnlocked", this, std::bind(&OptionsMenuScript::OnUIUnlocked, this));
	EventSystem::GetInstance()->UnregisterFromEvent("FORCED_FULLSCREEN_CHANGE", this, std::bind(&OptionsMenuScript::OnForcedStyleChange, this, std::placeholders::_1));
}

void OptionsMenuScript::ApplyBtn_OnClick()
{
	// Check to make sure that the Accept Menu is not active
	if (m_pAcceptMenuElement && m_pAcceptMenuElement->IsActive())
		return;

	WorkerThreadConfigSave* ConfigSave = new WorkerThreadConfigSave();
	auto& changedParameters = ConfigSave->ConfigVector;

	if (m_pResolutionDropdown)
	{
		std::vector<std::string> split = Inferno::StringParse(Inferno::ToAsciiString(m_pResolutionDropdown->GetSelectedValue()), 'x');
		float width = (float)atof(split[0].c_str()), height = (float)atof(split[1].c_str());
		Vector2 oldRes = InfernoEngine_Graphics::GetWindowDimensions();

		if (oldRes != Vector2(width, height))
			InfernoEngine_Graphics::SetWindowDimensions(width, height);
	}
	if (m_pWindowDropdown)
	{
		int selection = m_pWindowDropdown->GetSelectionID();
		if (selection >= 0 && selection < 3 && selection != InfernoEngine_Graphics::GetWindowStyle())
			InfernoEngine_Graphics::SetWindowStyle((UINT)m_pWindowDropdown->GetSelectionID());
	}
	if (m_pVSyncCheckbox)
	{
		bool checked = m_pVSyncCheckbox->IsChecked();
		if (checked != InfernoEngine_Graphics::VSyncEnabled())
		{
			InfernoEngine_Graphics::SetVSync(checked);

			changedParameters.push_back(std::pair<std::string, std::string>("bVsyncEnabled", std::to_string(checked ? 1 : 0)));
		}
	}
	if (m_pAnisotropicDropdown)
	{
		int selection = _wtoi(m_pAnisotropicDropdown->GetSelectedValue().c_str());
		if (selection != InfernoEngine_Graphics::GetAnisotropicFiltering())
		{
			InfernoEngine_Graphics::SetAnisotropicFiltering(selection);

			changedParameters.push_back(std::pair<std::string, std::string>("uAnisotropic", std::to_string(selection)));
		}
	}

	//Inferno::UpdateConfigFile(changedParameters);
	Inferno::PushWork(ConfigSave);

	if (InfernoEngine_Graphics::IsWindowDirty())
	{
		if (m_pAcceptMenuElement && m_pAcceptTimeText)
		{
			m_pAcceptMenuElement->SetActive(true);
			m_pAcceptTimeText->SetText(L"15");
			m_itAcceptTime.SetTime(15.0f);

			Inferno_UI::SetCurrentSelectable(m_pAcceptInitialSelectable);
		}
	}

	//InfernoEngine_Graphics::WindowUpdate();
	//InfernoEngine_Graphics::ApplyWindowChanges();
}
void OptionsMenuScript::BackBtn_OnClick()
{
	using namespace Inferno_UI;

	// Check to make sure that the Accept Menu is not active
	if (m_pAcceptMenuElement && m_pAcceptMenuElement->IsActive())
		return;

	UICanvas* optionsCanvas = GetCanvasByName("Options");

	WorkerThreadConfigSave* ConfigSave = new WorkerThreadConfigSave();
	auto& changedParameters = ConfigSave->ConfigVector;

	if (m_pGammaSlider)
	{
		changedParameters.push_back(std::pair<std::string, std::string>("fGamma", std::to_string(m_pGammaSlider->GetFill()  * 2.5f + 0.2f)));
	}
	if (m_pMasterVolumeSlider)
	{
		changedParameters.push_back(std::pair<std::string, std::string>("fMasterVolume", std::to_string(m_pMasterVolumeSlider->GetFill())));
	}
	if (m_pMusicVolumeSlider)
	{
		changedParameters.push_back(std::pair<std::string, std::string>("fMusicVolume", std::to_string(m_pMusicVolumeSlider->GetFill())));
	}
	if (m_pSFXVolumeSlider)
	{
		changedParameters.push_back(std::pair<std::string, std::string>("fSFXVolume", std::to_string(m_pSFXVolumeSlider->GetFill())));
	}

	//Inferno::UpdateConfigFile(changedParameters);
	Inferno::PushWork(ConfigSave);

	UICanvas* prevCanvas = Inferno_UI::GetCanvasByName(m_sPrevCanvas);

	if (optionsCanvas && prevCanvas)
	{
		optionsCanvas->GetUIElement()->SetActive(false);
		prevCanvas->GetUIElement()->SetActive(true);
		
		if (m_pMainMenu)
			m_pMainMenu->OnEnable();
	}
}
void OptionsMenuScript::OKBtn_OnClick()
{
	WorkerThreadConfigSave* ConfigSave = new WorkerThreadConfigSave();
	auto& changedParameters = ConfigSave->ConfigVector;

	if (m_pAcceptMenuElement)
		m_pAcceptMenuElement->SetActive(false);

	std::vector<std::string> split = Inferno::StringParse(Inferno::ToAsciiString(m_pResolutionDropdown->GetSelectedValue()), 'x');
	float width = (float)atof(split[0].c_str()), height = (float)atof(split[1].c_str());
	int selection = m_pWindowDropdown->GetSelectionID();

	changedParameters.push_back(std::pair<std::string, std::string>("fWidth", std::to_string(width)));
	changedParameters.push_back(std::pair<std::string, std::string>("fHeight", std::to_string(height)));
	changedParameters.push_back(std::pair<std::string, std::string>("uWindowMode", std::to_string(selection)));

	//Inferno::UpdateConfigFile(changedParameters);
	Inferno::PushWork(ConfigSave);

	InfernoEngine_Graphics::ApplyWindowChanges();
	Inferno_UI::SetCurrentSelectable(m_pInitialSelectable);

	m_itAcceptTime.FullReset();
}
void OptionsMenuScript::CancelBtn_OnClick()
{
	if (m_pAcceptMenuElement)
		m_pAcceptMenuElement->SetActive(false);

	InfernoEngine_Graphics::RevertWindowChanges();

	Vector2 res = InfernoEngine_Graphics::GetWindowDimensions();
	m_pResolutionDropdown->SetSelectedValue(std::to_wstring((int)res.x) + L"x" + std::to_wstring((int)res.y));
	m_pWindowDropdown->SetSelectedValue(InfernoEngine_Graphics::GetWindowStyle());

	Inferno_UI::SetCurrentSelectable(m_pInitialSelectable);

	m_itAcceptTime.FullReset();
}
void OptionsMenuScript::DefaultBtn_OnClick()
{
	WorkerThreadConfigSave* ConfigSave = new WorkerThreadConfigSave();
	auto& changedParameters = ConfigSave->ConfigVector;

	if (m_pResolutionDropdown)
		m_pResolutionDropdown->SetSelectedValue(L"1024x768");

	if (m_pWindowDropdown)
		m_pWindowDropdown->SetSelectedValue(0);

	if (m_pVSyncCheckbox)
	{
		m_pVSyncCheckbox->SetChecked(false);
		changedParameters.push_back(std::pair<std::string, std::string>("bVsyncEnabled", "0"));
		InfernoEngine_Graphics::SetVSync(false);
	}

	if (m_pAnisotropicDropdown)
	{
		m_pAnisotropicDropdown->SetSelectedValue(0);
		changedParameters.push_back(std::pair<std::string, std::string>("uAnisotropic", "1"));
		InfernoEngine_Graphics::SetAnisotropicFiltering(1);
	}

	if (m_pGammaSlider)
	{
		m_pGammaSlider->SetFill(0.32f);

		if (m_pGammaText)
			m_pGammaText->SetText("1.00");

		changedParameters.push_back(std::pair<std::string, std::string>("uAnisotropic", std::to_string(m_pGammaSlider->GetFill())));
		InfernoEngine_Graphics::SetGamma(1.0f);
	}

	if (m_pMasterVolumeSlider)
	{
		m_pMasterVolumeSlider->SetFill(1.0f);

		if (m_pMasterVolumeText)
			m_pMasterVolumeText->SetText(L"100");

		changedParameters.push_back(std::pair<std::string, std::string>("fMasterVolume", std::to_string(m_pMasterVolumeSlider->GetFill())));
		InfernoEngine_Audio::SetMasterVolume(1.0f);
	}

	if (m_pMusicVolumeSlider)
	{
		m_pMusicVolumeSlider->SetFill(1.0f);

		if (m_pMusicVolumeText)
			m_pMusicVolumeText->SetText(L"100");

		changedParameters.push_back(std::pair<std::string, std::string>("fMusicVolume", std::to_string(m_pMasterVolumeSlider->GetFill())));
		InfernoEngine_Audio::SetBackgroundMusicVolume(1.0f);
	}

	if (m_pSFXVolumeSlider)
	{
		m_pSFXVolumeSlider->SetFill(1.0f);

		if (m_pSFXVolumeText)
			m_pSFXVolumeText->SetText(L"100");

		changedParameters.push_back(std::pair<std::string, std::string>("fSFXVolume", std::to_string(m_pMasterVolumeSlider->GetFill())));
		InfernoEngine_Audio::SetSFXVolume(1.0f);
	}

	Inferno::PushWork(ConfigSave);
}

void OptionsMenuScript::OnUILocked()
{
	m_bUILocked = true;
}
void OptionsMenuScript::OnUIUnlocked()
{
	m_bUILocked = false;
}
void OptionsMenuScript::OnBackHit(ACTION_STATUS status)
{
	if ((Inferno_UI::IsFocusedElementLocked() || m_bUILocked) || status != ACTION_STATUS::AS_PRESSED)
		return;

	BackBtn_OnClick();
}
void OptionsMenuScript::OnWindowStyleChange()
{
	if (m_pWindowDropdown && m_pResolutionDropdown)
	{
		if (m_pWindowDropdown->GetSelectionID() == 1)
		{
			std::vector<std::wstring> resolutions;
			InfernoEngine_Graphics::GetSupportedResolutions(resolutions);

			m_pResolutionDropdown->SetDisabled(true);
			m_pResolutionDropdown->SetSelectedValue(resolutions[resolutions.size() - 1]);
		}
		else
		{
			if (m_pResolutionDropdown->GetDisabled())
				m_pResolutionDropdown->SetDisabled(false);
		}
	}
}
void OptionsMenuScript::OnForcedStyleChange(void* fullscreen)
{
	UINT WindowStyle = *(static_cast<bool*>(fullscreen)) ? 2 : 0;
	if (m_pResolutionDropdown && m_pResolutionDropdown->GetDisabled())
		m_pResolutionDropdown->SetDisabled(false);

	if (m_pWindowDropdown)
		m_pWindowDropdown->SetSelectedValue(WindowStyle);

	WorkerThreadConfigSave* ConfigSave = new WorkerThreadConfigSave();
	auto& changedParameters = ConfigSave->ConfigVector;
	changedParameters.push_back(std::pair<std::string, std::string>("uWindowMode", std::to_string(WindowStyle)));

	//Inferno::UpdateConfigFile(changedParameters);
	Inferno::PushWork(ConfigSave);
}
void OptionsMenuScript::OnGammaSliderChange()
{
	if (m_pGammaSlider && m_pGammaText)
	{
		float gammaValue = m_pGammaSlider->GetFill() * 2.5f + 0.2f;
		InfernoEngine_Graphics::SetGamma(gammaValue);
		
		std::wstringstream str;
		str << std::fixed << std::setprecision(2) << gammaValue;

		m_pGammaText->SetText(str.str());
	}
}
void OptionsMenuScript::OnMasterVolumeSliderChange()
{
	if (m_pMasterVolumeSlider && m_pMasterVolumeText)
	{
		InfernoEngine_Audio::SetMasterVolume(m_pMasterVolumeSlider->GetFill());
		m_pMasterVolumeText->SetText(std::to_wstring(int(m_pMasterVolumeSlider->GetFill() * 100)));
	}
}
void OptionsMenuScript::OnMusicVolumeSliderChange()
{
	if (m_pMusicVolumeSlider && m_pMusicVolumeText)
	{
		InfernoEngine_Audio::SetBackgroundMusicVolume(m_pMusicVolumeSlider->GetFill());
		m_pMusicVolumeText->SetText(std::to_wstring(int(m_pMusicVolumeSlider->GetFill() * 100)));	
	}
}
void OptionsMenuScript::OnSFXVolumeSliderChange()
{
	if (m_pSFXVolumeSlider && m_pSFXVolumeText)
	{
		InfernoEngine_Audio::SetSFXVolume(m_pSFXVolumeSlider->GetFill());
		m_pSFXVolumeText->SetText(std::to_wstring(int(m_pSFXVolumeSlider->GetFill() * 100)));
		Inferno::PostAudioEvent(OPTIONS_SFX_TEST);
	}
}

void OptionsMenuScript::SetPreviousCanvas(std::string canvas)
{
	m_sPrevCanvas = canvas;
}
void OptionsMenuScript::SetInitialSelectable(UISelectable* initial)
{
	m_pInitialSelectable = initial;
}
void OptionsMenuScript::SetAcceptInitialSelectable(UISelectable* initial)
{
	m_pAcceptInitialSelectable = initial;
}
void OptionsMenuScript::SetAcceptMenuElement(UIElement* element)
{
	m_pAcceptMenuElement = element;
}
void OptionsMenuScript::SetAcceptTimeText(UIText* text)
{
	m_pAcceptTimeText = text;
}
void OptionsMenuScript::SetMainMenu(MainMenuPrefab* mainMenu)
{
	m_pMainMenu = mainMenu;
}

void OptionsMenuScript::SetResolutionDropdown(UIDropdown* dropdown)
{
	m_pResolutionDropdown = dropdown;
}
void OptionsMenuScript::SetWindowDropdown(UIDropdown* dropdown)
{
	m_pWindowDropdown = dropdown;
}
void OptionsMenuScript::SetVSyncCheckbox(UICheckbox* checkbox)
{
	m_pVSyncCheckbox = checkbox;
}
void OptionsMenuScript::SetAnisotropicDropdown(UIDropdown* dropdown)
{
	m_pAnisotropicDropdown = dropdown;
}
void OptionsMenuScript::SetGammaSlider(UISlider* slider)
{
	m_pGammaSlider = slider;
}
void OptionsMenuScript::SetMasterVolumeSlider(UISlider* slider)
{
	m_pMasterVolumeSlider = slider;
}
void OptionsMenuScript::SetMusicVolumeSlider(UISlider* slider)
{
	m_pMusicVolumeSlider = slider;
}
void OptionsMenuScript::SetSFXVolumeSlider(UISlider* slider)
{
	m_pSFXVolumeSlider = slider;
}

void OptionsMenuScript::SetGammaText(UIText* text)
{
	m_pGammaText = text;
}
void OptionsMenuScript::SetMasterVolumeText(UIText* text)
{
	m_pMasterVolumeText = text;
}
void OptionsMenuScript::SetMusicVolumeText(UIText* text)
{
	m_pMusicVolumeText = text;
}
void OptionsMenuScript::SetSFXVolumeText(UIText* text)
{
	m_pSFXVolumeText = text;
}