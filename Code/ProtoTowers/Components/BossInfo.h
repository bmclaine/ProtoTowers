#pragma once
#include "UIComponents\UIComponent.h"

class UIRenderer;
class UIText;

class BossInfo : public UIComponent
{
private:
	// === Varaibles
	Texture* m_pHealthBarTexture;
	Texture* m_pLabelBackgroundTexture;
	UIElement* m_pLabelElement;
	UIRenderer* m_pHealthBar;
	UIText* m_pBossLbl;

public:
	// === Constructor / Destructor === //
	BossInfo(UIElement* _owner);
	virtual ~BossInfo();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	void UpdateHealthBar(float _healthRatio);
	// ================= //
};

