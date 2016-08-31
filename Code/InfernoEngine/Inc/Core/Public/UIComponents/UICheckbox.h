#pragma once

#include "UISelectable.h"

class Texture;
class UIRenderer;

class UICheckbox : public UISelectable
{
private:
	Texture* m_pBoxTexture;
	Texture* m_pCheckTexture;
	UIRenderer* m_pBoxRenderer;
	UIRenderer* m_pCheckRenderer;

	UINT m_uiLayer;
	bool m_bChecked;

public:
	UICheckbox(UIElement* _owner);
	virtual ~UICheckbox() override;

	virtual void OnSelect() override;
	void OnPressed();
	void OnRelease();
	virtual void OnDeselect() override;

	// === OnEnabled
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is Enabled or switching back to a 'Normal' state
	void OnEnabled();
	// === OnDisabled
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is disabled
	void OnDisabled();

	// === PostInit
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called after the component has been initialized. Can be overrriden by any derived UIComponents.
	virtual void PostInit();
	// === Update
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called every frame. Can be overriden by any derived UIComponents.
	virtual void Update();

	bool IsChecked() const;

	void SetChecked(bool checked);
	void SetLayer(UINT layer);
};

