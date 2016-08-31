//Author: Zane Lewis
//
//Prevents camera from going through files
#pragma once

class IComponent;
class Collider;
class GameObject;

class SpringArm : public IComponent
{
private:
	float m_fMaxDistance;	
	bool m_bIsColliding;
	bool m_bIsTriggered;
	GameObject * goCollidingObject;

protected:
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;

public:
	SpringArm(GameObject* const gameObject, Transform* const transform);

	virtual ~SpringArm()override;

	IComponent& operator=(IComponent&) override;
	SpringArm& operator=(const SpringArm& _rhs);
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void OnCollisionStay(Collider * _col) override;

	virtual void OnCollisionExit(Collider * _col) override;

	virtual void Update() override;


	virtual void OnTriggerExit(Collider * _col) override;
	virtual void OnTriggerEnter(Collider * _col) override;

	void SetCollisionFalse(Entity * _entity);

};

