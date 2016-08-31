#include "../Public/ObjectRegistry.h"

#include "AssertHelper.h"

#include <unordered_map>

std::map<unsigned int, ObjectRegistry::ComponentConstructor> ObjectRegistry::m_RegisteredComponents;
std::map<unsigned int, ObjectRegistry::UIComponentConstructor> ObjectRegistry::m_RegisteredUIComponents;

// === Constructor / Destructor === //
ObjectRegistry::ObjectRegistry()
{

}

ObjectRegistry::~ObjectRegistry()
{
	m_RegisteredComponents.clear();
	m_RegisteredUIComponents.clear();
}
// ================================ //

// === Interface === //

// === CreateComponent
//  IN : string - The hash key of the desired Component to create.
//  OUT : Component* - Pointer of the Created Component.
//
//  Looks up a Component by a given Identifier, creates and returns a pointer to the instance, if it was found.
IComponent* ObjectRegistry::CreateComponent(std::string& _id, GameObject* _gameObject, Transform* _transform)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_id);

	ComponentIter iter = m_RegisteredComponents.find(hash);
	if (!ASSERT(iter != m_RegisteredComponents.end(), "No component registered with the given ID")) {
		return nullptr;
	}

	return iter->second(_gameObject, _transform);
}

// === CreateComponent
//  IN : unsigned int - The hash key of the desired Component to create.
//  OUT : Component* - Pointer of the Created Component.
//
//  Looks up a Component by a given Hash Key, creates and returns a pointer to the instance, if it was found.
IComponent* ObjectRegistry::CreateComponent(unsigned int _hash, GameObject* _gameObject, Transform* _transform)
{
	ComponentIter iter = m_RegisteredComponents.find(_hash);
	if (!ASSERT(iter != m_RegisteredComponents.end(), "No component registered with the given ID")) {
		return nullptr;
	}

	return iter->second(_gameObject, _transform);
}

// === CreateUIComponent
//  IN : string - The hash key of the desired UIComponent to create.
//  OUT : UIComponent* - Pointer of the Created UIComponent.
//
//  Looks up a UIComponent by a given Identifier, creates and returns a pointer to the instance, if it was found.
UIComponent* ObjectRegistry::CreateUIComponent(std::string _id, UIElement* _uiElement, RectTransform* _transform)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_id);

	UIComponentIter iter = m_RegisteredUIComponents.find(hash);
	if (!ASSERT(iter != m_RegisteredUIComponents.end(), "No component registered with the given ID")) {
		return nullptr;
	}

	return iter->second(_uiElement);
}

// === CreateUIComponent
//  IN : unsigned int - The hash key of the desired UIComponent to create.
//  OUT : UIComponent* - Pointer of the Created UIComponent.
//
//  Looks up a UIComponent by a given Hash Key, creates and returns a pointer to the instance, if it was found.
UIComponent* ObjectRegistry::CreateUIComponent(unsigned int _hash, UIElement* _uiElement, RectTransform* _transform)
{
	UIComponentIter iter = m_RegisteredUIComponents.find(_hash);
	if (!ASSERT(iter != m_RegisteredUIComponents.end(), "No component registered with the given ID")) {
		return nullptr;
	}

	return iter->second(_uiElement);
}
// ================= //

// === Protected Interface === //

// === RegisterComponentType
//  IN : string - The Identifier of the Component being registered.
//       ComponentConstructor - The Constructor of the Component that is being registered.
//  OUT : unsigned int - The hashed value of the identifier
//
//  Stores the Constructor of the component that is being registered by a hash value from a string Identifier.
unsigned int ObjectRegistry::RegisterComponentType(std::string _id, ComponentConstructor _constructor)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_id);

	ComponentIter iter = m_RegisteredComponents.find(hash);
	if (!ASSERT(iter == m_RegisteredComponents.end(), "Duplicate identifiers being used to register a Component")) {
		return -1;
	}

	m_RegisteredComponents.insert(std::make_pair(hash, _constructor));

	return hash;
}

// === RegisterUIComponentType
//  IN : string - The Identifier of the UIComponent being registered.
//       ComponentConstructor - The Constructor of the UIComponent that is being registered.
//  OUT : unsigned int - The hashed value of the identifier
//
//  Stores the Constructor of the UIComponent that is being registered by a hash value from a string Identifier.
unsigned int ObjectRegistry::RegisterUIComponentType(std::string _id, UIComponentConstructor _constructor)
{
	unsigned int hash = (unsigned int)std::hash<std::string>()(_id);

	UIComponentIter iter = m_RegisteredUIComponents.find(hash);
	ASSERT(iter == m_RegisteredUIComponents.end(), "Duplicate identifiers being used to register a UIComponent");

	m_RegisteredUIComponents.insert(std::make_pair(hash, _constructor));

	return hash;
}
// =========================== //