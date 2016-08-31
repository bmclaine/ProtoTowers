#pragma once
#include "UIComponents\UIComponent.h"

class UIRenderer;

class Crosshair : public UIComponent
{
private:
	// === Variables
	UIRenderer* m_pImage;

	// === Events === //
	void OnShowCrosshair(void* _pData);
	// ============== //

public:
	// === Constructor / Destructor === //
	Crosshair(UIElement* _owner);
	virtual ~Crosshair();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	// ================= //
};

