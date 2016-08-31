#pragma once

#include "UIComponents\UIComponent.h"
#include "InfernoTimer.h"

class UIElement;
class UICanvas;
class MainMenuPrefab;

class UIDropdown;
class UICheckbox;
class UISlider;
class UIText;

class OptionsMenuScript : public UIComponent
{
private:
	std::string m_sPrevCanvas;
	InfernoTimer m_itAcceptTime;
	UISelectable* m_pInitialSelectable;
	UISelectable* m_pAcceptInitialSelectable;
	UIElement* m_pAcceptMenuElement;
	UIText* m_pAcceptTimeText;
	MainMenuPrefab* m_pMainMenu;

	// Selectables to read data from
	UIDropdown* m_pResolutionDropdown;
	UIDropdown* m_pWindowDropdown;
	UICheckbox* m_pVSyncCheckbox;
	UIDropdown* m_pAnisotropicDropdown;
	UISlider* m_pGammaSlider;
	UISlider* m_pMasterVolumeSlider;
	UISlider* m_pMusicVolumeSlider;
	UISlider* m_pSFXVolumeSlider;

	UIText* m_pGammaText;
	UIText* m_pMasterVolumeText;
	UIText* m_pMusicVolumeText;
	UIText* m_pSFXVolumeText;

	bool m_bUILocked;

public:
	// === Constructor / Destructor === //
	OptionsMenuScript(UIElement* _owner);
	virtual ~OptionsMenuScript();
	// ================================ //

	// === Interface === //
	virtual void PostInit() override;

	virtual void Update() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;
	// ================= //

	// === Button Events === //
	void ApplyBtn_OnClick();
	void BackBtn_OnClick();
	void OKBtn_OnClick();
	void CancelBtn_OnClick();
	void DefaultBtn_OnClick();
	// ===================== //

	// === Other Events === //
	void OnUILocked();
	void OnUIUnlocked();
	void OnBackHit(ACTION_STATUS status);
	void OnWindowStyleChange();
	void OnForcedStyleChange(void* fullscreen);
	void OnGammaSliderChange();
	void OnMasterVolumeSliderChange();
	void OnMusicVolumeSliderChange();
	void OnSFXVolumeSliderChange();
	// ==================== //

	void SetPreviousCanvas(std::string canvas);
	void SetInitialSelectable(UISelectable* initial);
	void SetAcceptInitialSelectable(UISelectable* initial);
	void SetAcceptMenuElement(UIElement* element);
	void SetAcceptTimeText(UIText* text);
	void SetMainMenu(MainMenuPrefab* mainMenu);

	void SetResolutionDropdown(UIDropdown* dropdown);
	void SetWindowDropdown(UIDropdown* dropdown);
	void SetVSyncCheckbox(UICheckbox* checkbox);
	void SetAnisotropicDropdown(UIDropdown* dropdown);
	void SetGammaSlider(UISlider* slider);
	void SetMasterVolumeSlider(UISlider* slider);
	void SetMusicVolumeSlider(UISlider* slider);
	void SetSFXVolumeSlider(UISlider* slider);

	void SetGammaText(UIText* text);
	void SetMasterVolumeText(UIText* text);
	void SetMusicVolumeText(UIText* text);
	void SetSFXVolumeText(UIText* text);
};

