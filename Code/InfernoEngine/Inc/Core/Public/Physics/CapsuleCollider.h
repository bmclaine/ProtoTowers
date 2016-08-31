#pragma once

#include "Collider.h"

class GameObject;
class Transform;

class CapsuleCollider : public Collider {

private:
	float m_fRadius;
	float m_fHeight;
	Vector3 m_f3Start;
	Vector3 m_f3End;

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	CapsuleCollider& operator=(const CapsuleCollider& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	virtual ~CapsuleCollider() override;

public:
	CapsuleCollider(GameObject* const gameObject, Transform* const transform);

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

	virtual const AABB GetBoundingBox() const;

	virtual const Box2D GetBox2D();

	virtual int GetColliderType() override;

	float GetRadius() const;

	void SetRadius(float radius);

	float GetHeight() const;

	void SetHeight(float h);

	Vector3 GetStart();

	Vector3 GetEnd();

	Vector3 GetDirection();

};


