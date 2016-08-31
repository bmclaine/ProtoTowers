#pragma once

#include "Collision.h"
#include "MathLib.h"
#include "PhysicsComponent.h"
#include "List.h"
#include <unordered_map>

class Collider : public PhysicsComponent {
	
protected:
	bool m_bIsTrigger;
	Vector3 m_f3Center;
	int m_timeStamp;
	Box2D m_boundingBox;
	std::unordered_map<Collider*, CollisionState> m_Collisions;

	virtual ~Collider() = default;

public:
	Collider(GameObject* const gameObject, Transform* const transform);

	Collider& operator=(const Collider& _rhs);

	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) = 0;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	void OnEnable() override;

	void OnDisable() override;

	void AddCollision(Collider* collider);

	void RemoveCollision(Collider* collider);

	bool Contains(Collider* collider);

	void UpdateCollision(Collider* collider, CollisionState state = COL_EXIT);

	void CollisionEvents();

	bool IsStatic() const;

	int GetTimeStamp() const;
	bool IsTrigger() const;
	Vector3 GetCenter() const;
	virtual const AABB GetBoundingBox() const = 0;
	virtual const Box2D GetBox2D() = 0;

	void SetTimeStamp(const int timeStamp);
	void SetIsTrigger(bool isTrigger);
	void SetCenter(const Vector3& center);	

	virtual int GetColliderType();
};