#include "UIComponents\UIComponent.h"

#include "UIElement.h"

// ===== Constructor / Destructor ===== //
UIComponent::UIComponent(UIElement* _owner) : m_fcFlags((unsigned int)UC_MAX_FLAGS)
{
	m_fcFlags.SetFlag((unsigned int)UC_FLAG_ISENABLED, (unsigned int)true);
	m_pUIElement = _owner;
	m_pRectTransform = _owner->GetTransform();
}

UIComponent::~UIComponent()
{

}
// ==================================== //

// === Interface === //

// === Init
//  IN : void
//  OUT : void
//
//  Function that gets called to initialize the component. Can be overrriden by any derived UIComponents.
void UIComponent::Init()
{

}

// === PostInit
//  IN : void
//  OUT : void
//
//  Function that gets called after the component has been initialized. Can be overrriden by any derived UIComponents.
void UIComponent::PostInit()
{

}

// === Update
//  IN : void
//  OUT : void
//
//  Function that gets called every frame. Can be overriden by any derived UIComponents.
void UIComponent::Update()
{

}

// === FixedUpdate
//  IN : void
//  OUT : void
//
//  Function that gets called at a fixed rate, every 1/60th of a second. Can be overrriden by any derived UIComponents.
void UIComponent::FixedUpdate()
{

}

// === LateUpdate
//  IN : void
//  OUT : void
// 
//  Function that gets called after all other update functions are finished. Can be overrriden by any derived UIComponents.
void UIComponent::LateUpdate()
{

}

void UIComponent::OnEnable()
{

}

void UIComponent::OnDisable()
{

}

void UIComponent::OnDestroy()
{

}
// ================= //

// === Accessors === //

// === IsEnabled
//  IN : void
//  OUT : bool - Whether or not this UIcomponent is currently enabled.
//
//  Returns whether or not this UIComponent is currently enabled.
bool UIComponent::IsEnabled() const
{
	return m_fcFlags.GetFlag((unsigned int)UC_FLAG_ISENABLED);
}

// === IsActiveInHierarchy
//  IN : void
//  OUT : bool - Whether or not this component is active in the game
//
//  Returns whether or not this component is active in the game, by checking up the parent chain for any unactive elements.
bool UIComponent::IsActiveInHierachry() const 
{
	if (!m_pUIElement->IsActiveInHierarchy())
		return false;

	return m_fcFlags.GetFlag((unsigned int)UC_FLAG_ISENABLED);
}

// === GetTransform
//  IN : void
//  OUT : RectTransform* - A pointer to the RectTransform that is attached to the owning UIElement.
//
//  Returns the RectTransform that is attached to the owning UIElement that this UIComponent is attached to.
RectTransform* UIComponent::GetTransform() const
{
	return m_pRectTransform;
}


// === GetUIElement
//  IN : void
//  OUT : UIElement* - Pointer to the UIElement that owns this UIComponent.
//
//  Returns the UIElement that this UIComponent is attached to.
UIElement* UIComponent::GetUIElement() const
{
	return m_pUIElement;
}
// ================= //

// === Mutators === //

// === SetEnabled
//  IN : bool - Whether or not this UIComponent should be enabled.
//  OUT : void
//
//  Sets this UIComponent to be either enabled or not.
void UIComponent::SetEnabled(bool _value)
{
	if (m_fcFlags.GetFlag((unsigned int)UC_FLAG_ISENABLED) != _value) {
		if (_value) {
			OnEnable();
		}
		else {
			OnDisable();
		}
	}

	m_fcFlags.SetFlag((unsigned int)UC_FLAG_ISENABLED, (unsigned int)_value);
}
// ================ //