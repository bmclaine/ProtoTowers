#pragma once
#include "UIComponents\UIComponent.h"

class UIRenderer;

class FrostHUDEffect : public UIComponent
{
private:
	// === Variables
	UIRenderer* m_pFrostImage;
	Vector2 m_v2GrowthSpeed;
	unsigned int m_uiEffectCounter;

	// === Events === //
	void OnToggleFrost(void* _pData);
	// ============== //

public:
	// === Constructor / Destructor === //
	FrostHUDEffect(UIElement* _owner);
	virtual ~FrostHUDEffect();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //
};

