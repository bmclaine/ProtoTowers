// Author : Jack Gillette
//
// Base UIComponent class for any UIComponents that will add interaction functionality to an UIElement.

#pragma once
#include "UIComponent.h"

#include <Windows.h>

class UISelectable : public UIComponent
{
protected:
	// === Variables
	UISelectable* m_pUpSelectable;
	UISelectable* m_pDownSelectable;
	UISelectable* m_pRightSelectable;
	UISelectable* m_pLeftSelectable;
	unsigned int m_uiSelectSound;
	unsigned int m_uiPressedSound;
	bool m_bIsDisabled;

public:
	// === Constructor / Destructor === //
	UISelectable(UIElement* _owner);
	~UISelectable();
	// ================================ //

	// === Interface === //
	virtual void PostInit();
	// ================= //

	// === Event Functions === //
	virtual void OnSelect() = 0;
	virtual void OnDeselect() = 0;
	virtual void OnMouseOver(tagPOINT& _cursorPosition, tagPOINT& _displacement);
	// ======================= //

	// === Accessors === //
	virtual UISelectable* GetUpSelectable() const;
	virtual UISelectable* GetDownSelectable() const;
	virtual UISelectable* GetRightSelectable() const;
	virtual UISelectable* GetLeftSelectable() const;
	bool GetDisabled() const;
	// ================= //

	// === Mutators === //
	void SetUpSelectable(UISelectable* _selectable);
	void SetDownSelectable(UISelectable* _selectable);
	void SetRightSelectable(UISelectable* _selectable);
	void SetLeftSelectable(UISelectable* _selectable);
	void SetPressedSound(unsigned int _soundID);
	void SetSelectSound(unsigned int _soundID);
	void SetDisabled(bool disabled);
	// ================ //
};

