#pragma once

#include "UIComponents\UIComponent.h"
#include "InfernoTimer.h"

class UIButton;
class UISelectable;

class ControllerDisconnectScript : public UIComponent
{
private:
	InfernoTimer m_itControlChangeTimer;
	UIElement* m_pDisconnectedWindow;
	UIButton* m_pOKButton;
	UISelectable* m_pPrevSelectable;
	float m_fPrevTimeScale;

public:
	ControllerDisconnectScript(UIElement* _owner);
	virtual ~ControllerDisconnectScript() override;

	virtual void OnEnable() override;
	virtual void OnDestroy() override;

	void OnControlChange(bool usingController);
	void OnControllerDisconnect(unsigned int controllerID);
	void OKBtn_OnClick();

	void SetDisconnectedWindow(UIElement* window);
	void SetOKButton(UIButton* button);
};

