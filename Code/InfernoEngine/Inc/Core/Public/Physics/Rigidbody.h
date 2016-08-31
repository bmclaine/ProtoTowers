#pragma once

#include "PhysicsComponent.h"
#include "MathLib.h"

class Rigidbody : public PhysicsComponent {

private:
	bool m_bUseGravity;
	bool m_bSleep;
	Vector3 m_f3Velocity;
	float m_fMass;

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	Rigidbody& operator=(const Rigidbody& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	Rigidbody(GameObject* const gameObject, Transform* const transform);
	virtual ~Rigidbody() override;

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

	// IN: void
	// OUT: void
	//
	// Update only called every 1/60th of second
	virtual void FixedUpdate();

	// IN: Vector3 - the new position of the rigidbody
	// OUT: void
	//
	// Sets the position of the rigidbody to the new position
	void MovePosition(const Vector3& newPosition);

	void SetUseGravity(bool useGravity);
	void SetSleep(bool sleep);
	void SetVelocity(const Vector3& velocity);
	void SetMass(const float mass);

	bool UseGravity() const;
	bool IsSleep() const;
	const Vector3& GetVelocity() const;
	float GetMass() const;

	void OnEnable() override;

	void OnDisable() override;

private:
	
};