// Author : Jack Gillette
//
// A GameObject like class for UI objects that will be drawn in 2D space to the screen. Handles it's position with a RectTransform
// and can have UIComponents attached to it. Must be attached to a Canvas in order to be rendered to the screen

#pragma once

#include "Flags.h"
#include "UIComponents\RectTransform.h"
#include "UIComponents\UIComponent.h"

#include <string>
#include <vector>

class UIEventSystem;
class RectTransform;

class UIElement
{
private:
	// === Enumerations
	enum UIELEMENT_FLAGS { UE_ACTIVE_FLAG = 0, UE_POSTINIT_CALLED_FLAG, UE_MAX_FLAGS };

	// === Variables
	std::string m_sName;
	std::string m_sTag;
	std::string m_sCanvasName;
	Flags<char> m_fcFlags = Flags<char>(UE_MAX_FLAGS);
	UIEventSystem* m_pEventSystem;
	RectTransform* m_pRectTransform;
	std::vector<UIComponent*> m_vComponents;

	// === Friends
	friend void RectTransform::SetParent(RectTransform* _parent);
	friend void RectTransform::SetCanvasName(std::string& _canvasName);

public:
	// === Constructor / Destructor === //
	UIElement();
	~UIElement();
	// ================================ //

	// === Interface === //

	// === Init
	//  IN : void
	//  OUT : void
	//
	//  Handles Initialization of the UIElement and calls Init() on the UIComponents that are attached to this UIElement.
	void Init();
	// === PostInit
	//  IN : void
	//  OUT : void
	// 
	//  Handles Post-Initialization of the UIElement and calls PostInit() on the UIComponents that are attached to this UIElement.
	void PostInit();
	// === Update
	//  IN : void
	//  OUT : void
	//
	//  Handles Updating the UIComponents and children attached to this UIElement
	void Update();
	// === LateUpdate
	//  IN : void
	//  OUT : void
	//
	// Handles the LateUpdate on all UIComponents and children attached to this UIElement
	void LateUpdate();
	// === CompareTag
	//  IN - string - string to compare against this UIElement's tag.
	//  OUT : bool - Whether or not the two tags match.
	//
	//  Checks to see if the specified tag matches the tag of this UIElement.
	bool CompareTag(std::string _tag) const;
	void OnEnable();
	void OnDisable();
	void OnDestroy();

	// === AddComponent
	//  IN : void
	//  OUT : type* - Pointer to the Object Type that was specified and created in the function.
	//
	//  Returns a Pointer to the Object that was created and attached to this component. Handles Initialization of the Component, as
	//  well as the Post-Initialization, if the UIElement itself has already completed it's Post-Initialization.
	template<typename type>
	type* AddComponent();
	// === GetComponent
	//  IN : bool - Whether or not it should consider a component that is disabled
	//  OUT : type* - Pointer to the first found component of the desired type.
	//
	//  Searched through the components that are attached to this UIElement and returns the first component that matches the desired
	//  type.
	template<typename type>
	type* GetComponent(bool _getDisabledComponents = false);
	// === GetComponents
	//  IN : bool - Whether or not it should consider a component that is disabled
	//  OUT : vector<type*> - Vector of the UIComponents that were found to have matched the desired UIComponent type.
	//
	//  Searched through the components that are attached to this UIElement and returns the first component that matches the desired
	//  type.
	template<typename type>
	vector<type*>& GetComponents(bool _getDisabledComponents = false);
	template<typename type>
	// === GetComponentInChildren
	//  IN : bool - Whether or not it should consider a component that is disabled
	//  OUT : type* - Pointer to the first found component attached to a child of this UIElement of the desired type. 
	//
	//  Searched through the components that are attached to this UIElement and returns the first component that matches the desired
	//  type.
	type* GetComponentInChildren(bool _getDisabledComponents = false);
	// === GetComponentsInChildren
	//  IN : bool - Whether or not it should consider a component that is disabled
	//  OUT : vector<type*> - Vector of the UIComponents that were found, in the children, to have matched the desired UIComponent type.
	//
	//  Searched through the components attached to the children of this UIElement, and returns the first component that matches the 
	//  desired type.
	template<typename type>
	vector<type*>& GetComponentsInChildren(bool _getDisabledComponents = false);
	// ================ //

	// === Accessors === //

	// === IsActive
	//  IN : void
	//  OUT : bool - Whether or not this UIElement is active.
	//
	//  Returns whether or not this UIElement is currently active.
	bool IsActive() const;
	// === IsActiveInHierarchy
	//  IN : void
	//  OUT : bool - Whether or not this UIElement is currently active.
	//
	//  Returns whether or not this UIElement is currently active, taking into account for whether or no it's parent is active.
	bool IsActiveInHierarchy() const;
	// GetCanvasName
	//  IN : void
	//  OUT : string* - Pointer to the Canvas Name
	//
	//  Returns the Name of the Canvas that this UIElement is attached to. If it's not on any canvas, returns nullptr;
	std::string GetCanvasName() const;
	// === GetName
	//  IN : void
	//  OUT : string - Name of the UIElement.
	// 
	//  Returns the name of the UIElement.
	std::string GetName() const;
	// === GetTag
	//  IN : void
	//  OUT : string - Tag of the UIElement.
	//
	//  Returns the tag of the UIElement.
	std::string GetTag() const;
	// === GetTransform
	//  IN : void
	//  OUT : RectTransform* - Pointer to the RectTransform of this UIElement.
	//
	// Returns the RectTransform that is attached to this UIElement.
	RectTransform* GetTransform();

	UIEventSystem* GetEventSystem();
	// ================= //

	// === Mutators === //

	// === SetActive
	//  IN : bool - Whether or not the UIElement should be active.
	//  OUT : void
	//
	//  Sets whether or not the UIElement should be active.
	void SetActive(bool _value);
	// === SetName
	//  IN : string - Name for the UIElement to use.
	//  OUT : void
	//
	//  Sets the Name of the UIElement.
	void SetName(std::string _name);
	// === SetTag
	//  IN : string - Tag for the UIElement to use.
	//  OUT : void
	//
	//  Sets the Tag of the UIElement.
	void SetTag(std::string _tag);
	// ================ //
};

// ===== Templated Interface ===== //

// === AddComponent
//  IN : void
//  OUT : type* - Pointer to the Object Type that was specified and created in the function.
//
//  Returns a Pointer to the Object that was created and attached to this component. Handles Initialization of the Component, as
//  well as the Post-Initialization, if the UIElement itself has already completed it's Post-Initialization.
template<typename type>
type* UIElement::AddComponent()
{
	// === Make sure it's a UIComponent that is trying to be added
	if (!std::is_base_of<UIComponent, type>::value)
		return nullptr;

	UIComponent* newComponent = new type(this);
	m_vComponents.push_back(newComponent);

	newComponent->Init();

	if (m_fcFlags.GetFlag(UE_POSTINIT_CALLED_FLAG)) {
		newComponent->PostInit();
	}

	return (type*)newComponent;
}

// === GetComponent
//  IN : bool - Whether or not it should consider a component that is disabled
//  OUT : type* - Pointer to the first found component of the desired type.
//
//  Searched through the components that are attached to this UIElement and returns the first component that matches the desired
//  type.
template<typename type>
type* UIElement::GetComponent(bool _getDisabledComponents)
{
	type* component = nullptr;

	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		if (_getDisabledComponents || m_vComponents[i]->IsEnabled()) {
			component = dynamic_cast<type*>(m_vComponents[i]);

			if (component != nullptr) {
				return component;
			}
		}
	}

	return component;
}

// === GetComponents
//  IN : bool - Whether or not it should consider a component that is disabled
//  OUT : vector<type*> - Vector of the UIComponents that were found to have matched the desired UIComponent type.
//
//  Searched through the components that are attached to this UIElement and returns the first component that matches the desired
//  type.
template<typename type>
vector<type*>& UIElement::GetComponents(bool _getDisabledComponents)
{
	vector<type*> foundComponents = vector<type*>();
	type* component = nullptr;

	size_t count = m_vComponents.size();
	for (size_t i = 0; i < count; ++i) {
		component = dynamic_cast<type*>(m_vComponents[i]);

		if (component != nullptr) {
			foundComponents.push_back(component);
		}
	}

	return foundComponents;
}

// === GetComponentInChildren
//  IN : bool - Whether or not it should consider a component that is disabled
//  OUT : type* - Pointer to the first found component attached to a child of this UIElement of the desired type. 
//
//  Searched through the components that are attached to this UIElement and returns the first component that matches the desired
//  type.
template<typename type>
type* UIElement::GetComponentInChildren(bool _getDisabledComponents)
{
	type* component = nullptr;

	// === Search through its children
	size_t count = m_pRectTransform->m_vChildren.size();
	for (size_t i = 0; i < count; ++i) {
		component = dynamic_cast<type*>(m_pRectTransform->m_vChildren[i]->GetUIElement()->GetComponent<type>(_getDisabledComponents));

		if (component != nullptr) {
			return component;
		}
	}

	// === Still not found, search through its children children
	for (size_t i = 0; i < count; ++i) {
		component = dynamic_cast<type*>(m_pRectTransform->m_vChildren[i]->GetUIElement()->GetComponentInChildren<type>(_getDisabledComponents));

		if (component != nullptr) {
			return component;
		}
	}

	return component;
}

// === GetComponentsInChildren
//  IN : bool - Whether or not it should consider a component that is disabled
//  OUT : vector<type*> - Vector of the UIComponents that were found, in the children, to have matched the desired UIComponent type.
//
//  Searched through the components attached to the children of this UIElement, and returns the first component that matches the 
//  desired type.
template<typename type>
vector<type*>& UIElement::GetComponentsInChildren(bool _getDisabledComponents)
{
	vector<type*> foundComponents = vector<type*>();
	vector<type*> newFoundComponents = vector<type*>();

	size_t count = m_pRectTransform->m_vChildren.size();

	for (size_t i = 0; i < count; ++i) {
		// === Search through its children
		newFoundComponents = m_pRectTransform->m_vChildren[i]->GetUIElement()->GetComponents(_getDisabledComponents);

		size_t foundCount = newFoundComponents.size();
		for (size_t x = 0; x < foundCount; ++x) {
			foundComponents.push_back(newFoundComponents[x]);
		}

		// === Search through its children children
		newFoundComponents = m_pRectTransform->m_vChildren[i]->GetUIElement()->GetComponentsInChildren(_getDisabledComponents);

		foundCount = newFoundComponents.size();
		for (size_t x = 0; x < foundCount; ++x) {
			foundComponents.push_back(newFoundComponents[x]);
		}
	}

	return foundComponents;
}
// =============================== //