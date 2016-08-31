#pragma once
#include "UIComponents\UIComponent.h"

class Texture;
class UIText;

class TowerDescription : public UIComponent
{
private:
	// === Variables
	Texture* m_pBackgroundImg;
	UIText* m_pText;

public:
	// === Constructor / Destructor === //
	TowerDescription(UIElement* _owner);
	virtual ~TowerDescription();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	virtual void DisplayDescription(std::string& _message, Vector2 _position);
	// ================= //
};

