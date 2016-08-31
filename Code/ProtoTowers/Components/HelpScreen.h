// Author : Jack Gillette
//
// Handles displaying the Help Screen to the player when needed.
#pragma once
#include "UIComponents\UIComponent.h"

class UIRenderer;

class HelpScreen : public UIComponent
{
private:
	// === Variables
	UIRenderer* m_pImage;
	UIRenderer* m_pCloseBtnImage, *m_pNextBtnImage, *m_pPrevBtnImage;
	Texture* m_pHelpImages[6];		//  <-- Should probably move once we have a texture manager
	Texture* m_pCloseBtnTexture;
	Texture* m_pNextBtnTexture, *m_pPrevBtnTexture;
	float m_fDelayTime;
	unsigned int m_uiCurrentPage;
	bool m_bRecieveInput;

	// === Private Interface === //
	void OpenScreen();
	// ========================= //

	// === Events === //
	void OnOpenHelpScreen(void* _pData);
	void OnSetRecieveInput(void* _pData);
	// ============== //
	
public:
	// === Constructor / Destructor === //
	HelpScreen(UIElement* _owner);
	virtual ~HelpScreen();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //

	// === Input Events === //
	void OnToggleHelpScreen(ACTION_STATUS _status);
	void OnNextPage(float _axisValue);
	void OnPrevPage(float _axisValue);
	// ==================== //
};

