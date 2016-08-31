#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"
#include "GameObject.h"

class Player;

class PlayerShieldScript : public IComponent
{
private:
	InfernoTimer m_itEpandTimer;
	Player* m_pPlayer;
	float m_fAliveTime;
	float m_fExpandTime;
	float m_fRadius;
	float m_fOffset;

	virtual ~PlayerShieldScript() override;

	PlayerShieldScript& operator=(const PlayerShieldScript& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	PlayerShieldScript(GameObject* const gameObject, Transform* const transform);

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

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	// IN: void
	// OUT: void
	//
	// Update only called every 1/60th of second
	virtual void FixedUpdate() override;

	// IN: void
	// OUT: void
	//
	// Called on the first frame that collision has been detected
	virtual void OnCollisionEnter(Collider* col) override;

	void SetRadius(float radius);
	void SetExpandTime(float time);
	void SetShieldTime(float time);
	void SetOffset(float offset);
	void SetPlayer(Player* player);

	friend PlayerShieldScript* GameObject::AddComponent();
};

