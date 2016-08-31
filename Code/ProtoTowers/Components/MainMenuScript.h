#pragma once
#include "UIComponents\UIComponent.h"

#include "UIEvents.h"

class UIElement;
class OptionsMenuPrefab;

class MainMenuScript : public UIComponent
{
private:
	UISelectable* m_pInitialSelectable;

public:
	// === Constructor / Destructor === //
	MainMenuScript(UIElement* _owner);
	virtual ~MainMenuScript();
	// ================================ //

	// === Interface === //
	virtual void PostInit() override;
	virtual void OnEnable() override;
	// ================= //

	// === Button Events === //
	void PlayBtn_OnClick();
	void OptionBtn_OnClick();
	void CreditsBtn_OnClick();
	void QuitBtn_OnClick();
	// ===================== //

	// === Mutators === //
	void SetInitialSelectable(UISelectable* initial);
	// ================ //
};

