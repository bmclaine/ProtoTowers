#pragma once

#include "UIComponents\UIComponent.h"

class InfernoTimer;

class CreditsMenuScript : public UIComponent
{
private:
	RectTransform* m_pTransform;

	UIElement * m_pSkipMessegeKeyboard, * m_pSkipMessegeController;
	
	InfernoTimer m_fDelayTimer, m_fSkipTimer;

	float m_fScrollSpeed;

	bool m_bIsSkippable;
	bool m_bIsRunnable;

	void UpdateDelay();
	void UpdateSkip();

	void OnControllerChange(bool _usingController);

protected:

public:
	CreditsMenuScript(UIElement* _owner);
	virtual ~CreditsMenuScript();

	virtual void PostInit() override;

	virtual void OnEnable();

	void SkipCredits(ACTION_STATUS _status);
	void Update();

	void SetSkipMessages(UIElement* _keyboardMessege, UIElement* _controllerMessege);
};