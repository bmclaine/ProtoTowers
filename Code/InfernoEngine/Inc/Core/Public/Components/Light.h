#pragma once

#include "IComponent.h"
#include "MathLib.h"

#include "GameObject.h"

class Light : public IComponent
{
private:
	Vector4 m_f4ColorAndIntensity;
	float m_fRadius;

	virtual ~Light() override;

	virtual IComponent& operator=(IComponent&) override;
	Light& operator=(const Light& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	Light(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable() override;
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
	virtual void OnDisable() override;

	void SetColor(Vector3 color);
	void SetColor(Vector4 color);
	void SetIntensity(float intensity);
	void SetRadius(float radius);

	Vector3 GetColor() const;
	float GetIntensity() const;
	float GetRadius() const;

	friend Light* GameObject::AddComponent();
};

