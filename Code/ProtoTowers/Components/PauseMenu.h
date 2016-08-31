// Author : Jack Gillette
//
// Handles all functionality of the Pause Menu
#pragma once
#include "UIComponents\UIComponent.h"

class PauseMenu : public UIComponent
{
private:
	// === Variables
	UIElement* m_pMenuElement;
	UIRenderer* m_pSelectImage;
	UIRenderer* m_pBackImage;
	UISelectable* m_pFirstSelectable;
	Texture* m_pSelectBtnTexture;
	Texture* m_pBackBtnTexture;
	bool m_bAllInputToggled;

public:
	// === Constructor / Destructor === //
	PauseMenu(UIElement* _owner);
	virtual ~PauseMenu();
	// ================================ //

	// === Interface === //
	virtual void PostInit() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	// ================= //

	// === Events === //

	// === OnTogglePauseMenuInput
	//  IN : void* - Event Data, this should just have a bool stored inside of it
	//  OUT : void
	//
	//  An event that will be invoked when the pause menu should toggle itself on or off. This would be for cases where another UI menu is up, where the 
	//  pause menu should not react to certain input
	void OnTogglePauseMenuInput(void* _pData);
	// === OnTogglePauseMenuFullInput
	//  IN : void* - Event Data that just has a bool, for whether or not to toggle the FullInput of the PauseMenu
	//  OUT : void
	//
	//  Toggles the Full Input of the PauseMenu, meaning that if it's turned off, the Pause Menu cannot be activated at all
	void OnTogglePauseMenuFullInput(void* _pData);
	// === OnTogglePauseMenu
	//  IN : ACTION_STATUS - Status of the Input Binding
	//  OUT : void
	//
	//  Input Event function for toggling the Pause Menu on and Off.
	void OnTogglePauseMenu(ACTION_STATUS _status);
	// ============== //

	// === Button Functions === //
	void OnResumeBtnClick();
	void OnOptionsBtnClick();
	void OnHowToPlayBtnClick();
	void OnQuitBtnQuit();
	// ======================== //

	// === Mutators === //
	void SetFirstSelectable(UISelectable* _firstSelectable);
	// ================ //
};

