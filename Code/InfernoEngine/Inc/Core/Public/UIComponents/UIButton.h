// Author : Jack Gillette
//
// Derived class from UISelectable, UIButton provides basic button functionality to a UIElement. Has Events for switching the
// texture of the button for OnHover, OnClick, OnDisabled, and OnSelect
#pragma once
#include "UISelectable.h"

#include "Color.h"

class Texture;
class UIRenderer;

class UIButton : public UISelectable
{
private:
	// === Structures
	struct Image {
		Color ColorOffset;
		Texture* Texture;
	};

	// === Variables
	Image m_ImageNormal;
	Image m_ImageHover;
	Image m_ImagePressed;
	Image m_ImageDisabled;
	UIRenderer* m_pImage;

public:
	// === Constructor / Destructor === //
	UIButton(UIElement* _owner);
	~UIButton();
	// ================================ //

	// === Interface === //

	// === PostInit
	//  IN : void
	//  OUT : void
	//
	//  Handles Registering all the functions to the EventSystem that is on the UIElement, and calls it's parent PostInit function.
	virtual void PostInit() override;
	// ================= //

	// === Event Functions === //

	// === OnSelect
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is Selected / Hovered over
	virtual void OnSelect() override;
	// === OnDeselect
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is Deselected / done being hovered over
	virtual void OnDeselect() override;
	// === OnPressed
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is clicked 
	void OnPressed();
	// === OnDisabled
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is disabled
	void OnDisabled();
	// === OnEnabled
	//  IN : void
	//  OUT : void
	//
	//  Event Function that is triggered when the Button is Enabled or switching back to a 'Normal' state
	void OnEnabled();
	// ================= //

	// === Mutators === //
	void SetNormalImage(Texture* _texture, Color _colorOffset = Color());
	void SetHoverImage(Texture* _texture, Color _colorOffset = Color());
	void SetPressedImage(Texture* _texture, Color _colorOffset = Color());
	void SetDisabledImage(Texture* _texture, Color _colorOffset = Color());
	void SetLayer(unsigned int _layer);
	// ================ //
};

