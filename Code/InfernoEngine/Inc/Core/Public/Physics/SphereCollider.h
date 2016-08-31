#pragma once

#include "Collider.h"

class SphereCollider : public Collider{

private:
	float m_fRadius;

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	SphereCollider& operator=(const SphereCollider& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	virtual ~SphereCollider() override;

public:
	SphereCollider(GameObject* const gameObject, Transform* const transform);

	// IN : void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	// IN: void
	// OUT: void
	//
	// Draws Gizmos for the component
	virtual void OnDrawGizmos() override;

	float GetRadius() const;

	void SetRadius(float radius);

	virtual const AABB GetBoundingBox() const;

	virtual const Box2D GetBox2D();

	virtual int GetColliderType();
};