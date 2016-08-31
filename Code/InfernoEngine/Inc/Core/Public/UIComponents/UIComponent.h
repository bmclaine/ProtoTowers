// Author : Jack Gillette
//
// Base Component class for all components that will be used alongside with UIElements.
#pragma once

#include "Flags.h"

#include <functional>

class RectTransform;
class UIElement;

// === Type Defs
typedef std::function<void()> EventFunction;

// === Defines
#define BindFunction(EventFunction) { std::bind(EventFunction, this) }

class UIComponent
{
protected:
	// === Enumerations
	enum UICOMPONENT_FLAGS { UC_FLAG_ISENABLED, UC_MAX_FLAGS };

	// === Variables
	Flags<char> m_fcFlags;
	RectTransform* m_pRectTransform;
	UIElement* m_pUIElement;

public:
	// === Constructor / Destructor === //
	UIComponent(UIElement* _owner);
	virtual ~UIComponent();
	// ================================ //

	// === Interface === //

	// === Init
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called to initialize the component. Can be overrriden by any derived UIComponents.
	virtual void Init();
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
	// === FixedUpdate
	//  IN : void
	//  OUT : void
	//
	//  Function that gets called at a fixed rate, every 1/60th of a second. Can be overrriden by any derived UIComponents.
	virtual void FixedUpdate();
	// === LateUpdate
	//  IN : void
	//  OUT : void
	// 
	//  Function that gets called after all other update functions are finished. Can be overrriden by any derived UIComponents.
	virtual void LateUpdate();
	virtual void OnEnable();
	virtual void OnDisable();
	virtual void OnDestroy();
	// ================= //

	// === Accessors === //

	// === IsEnabled
	//  IN : void
	//  OUT : bool - Whether or not this UIcomponent is currently enabled.
	//
	//  Returns whether or not this UIComponent is currently enabled.
	bool IsEnabled() const;
	// === IsActiveInHierarchy
	//  IN : void
	//  OUT : bool - Whether or not this component is active in the game
	//
	//  Returns whether or not this component is active in the game, by checking up the parent chain for any unactive elements.
	bool IsActiveInHierachry() const;
	// === GetTransform
	//  IN : void
	//  OUT : RectTransform* - A pointer to the RectTransform that is attached to the owning UIElement.
	//
	//  Returns the RectTransform that is attached to the owning UIElement that this UIComponent is attached to.
	RectTransform* GetTransform() const;
	// === GetUIElement
	//  IN : void
	//  OUT : UIElement* - Pointer to the UIElement that owns this UIComponent.
	//
	//  Returns the UIElement that this UIComponent is attached to.
	UIElement* GetUIElement() const;
	// ================= //

	// === Mutators === //

	// === SetEnabled
	//  IN : bool - Whether or not this UIComponent should be enabled.
	//  OUT : void
	//
	//  Sets this UIComponent to be either enabled or not.
	void SetEnabled(bool _value);
	// ================ //
};