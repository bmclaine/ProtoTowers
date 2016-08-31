// Author : Jack Gillette
//
// Handles updating a UIText Component with the Player's Resource Count.
#pragma once
#include "UIComponents\UIComponent.h"

class UIText;

class ResourceCounter : public UIComponent
{
public:
	// === Variables
	UIText* m_pUIText;

public:
	// === Constructor / Destructor === //
	ResourceCounter(UIElement* _owner);
	~ResourceCounter();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	virtual void OnDisable() override;

	void OnResourceUpdate(unsigned int _newAmount);
	// ================= //
};

