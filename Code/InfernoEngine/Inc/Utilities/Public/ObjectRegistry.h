// Author : Jack Gillette
//
// Registry System that allows object classes to be registered to an identifier, which can then be used to create a new instance of
// that object class by just giving the ID.
#pragma once

#include <map>

class GameObject;
class IComponent;
class RectTransform;
class Transform;
class UIComponent;
class UIElement;

class ObjectRegistry
{
public:
	// === TypeDefs
	typedef IComponent* (*ComponentConstructor)(GameObject* const, Transform* const);
	typedef UIComponent* (*UIComponentConstructor)(UIElement* const);

private:
	// === TypeDefs
	typedef std::map<unsigned int, ComponentConstructor>::iterator ComponentIter;
	typedef std::map<unsigned int, UIComponentConstructor>::iterator UIComponentIter;

	// === Static Variables
	static std::map<unsigned int, ComponentConstructor> m_RegisteredComponents;
	static std::map<unsigned int, UIComponentConstructor> m_RegisteredUIComponents;

protected:
	// === Protected Interface === //

	// === RegisterComponentType
	//  IN : string - The Identifier of the Component being registered.
	//       ComponentConstructor - The Constructor of the Component that is being registered.
	//  OUT : unsigned int - The hashed value of the identifier
	//
	//  Stores the Constructor of the component that is being registered by a hash value from a string Identifier.
	static unsigned int RegisterComponentType(std::string _id, ComponentConstructor _constructor);
	// === RegisterUIComponentType
	//  IN : string - The Identifier of the UIComponent being registered.
	//       ComponentConstructor - The Constructor of the UIComponent that is being registered.
	//  OUT : unsigned int - The hashed value of the identifier
	//
	//  Stores the Constructor of the UIComponent that is being registered by a hash value from a string Identifier.
	static unsigned int RegisterUIComponentType(std::string _id, UIComponentConstructor _constructor);
	// =========================== //

	// === Templated Protected Interface === //
	template<typename type> 
	friend IComponent* componentConstructor(GameObject* _gameObject, Transform* _transform) {
		return new type(_gameObject, _transform);
	}
	template<typename type>
	friend UIComponent* uiComponentConstructor(UIElement* _uiElement) {
		return new type(_uiElement);
	}
	// ===================================== //

public:
	// === Constructor / Destructor === //
	ObjectRegistry();
	~ObjectRegistry();
	// ================================ //

	// === Interface === //

	// === CreateComponent
	//  IN : string - The hash key of the desired Component to create.
	//  OUT : Component* - Pointer of the Created Component.
	//
	//  Looks up a Component by a given Identifier, creates and returns a pointer to the instance, if it was found.
	IComponent* CreateComponent(std::string& _id, GameObject* _gameObject, Transform* _transform);

	IComponent* CreateComponent(unsigned int _hash, GameObject* _gameObject, Transform* _transform);
	UIComponent* CreateUIComponent(std::string _id, UIElement* _uiElement, RectTransform* _transform);
	UIComponent* CreateUIComponent(unsigned int _hash, UIElement* _uiElement, RectTransform* _transform);
	// ================= //

	// === Templated Interface === //
	template<typename type> 
	static unsigned int RegisterComponentType(std::string _id);
	template<typename type>
	static unsigned int RegisterUIComponentType(std::string _id);
	// =========================== //
};



// ===== Templated Interface ===== //
template<typename type>
unsigned int ObjectRegistry::RegisterComponentType(std::string _id)
{
	return RegisterComponentType(_id, &componentConstructor<type>);
}

template<typename type>
unsigned int ObjectRegistry::RegisterUIComponentType(std::string _id)
{
	return RegisterUIComponentType(_id, &uiComponentConstructor<type>);
}
// =============================== //