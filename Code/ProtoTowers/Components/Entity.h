// Author : Jack Gillette
//
// Abstract Base Class for anything that has health and can be damaged
#pragma once
#include "Components\IComponent.h"
#include "../Core/iDamagable.h"

#include <functional>
#include <vector>

class GameObject;
class Transform;

// === Defines
#define BindEntityOnReleaseFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }

enum Team { NO_TEAM, BLUE, RED, TEAM_MAX };
enum Cheat { TOGGLE_GODMODE, KILL_ENEMY_WAVE, SPAWN_FRIENDLY_WAVE, GIVE_RESOURCES, CHEAT_MAX };

class Entity : public IComponent, public iDamagable
{
public:
	// === Typedefs
	typedef std::function<void(Entity*)> OnReleaseFunc;

private:
	// === Structures
	struct TrackingObject {
		void* Object;
		OnReleaseFunc Func;
	};

	// === Variables
	std::vector<TrackingObject> m_vObjectsTrackedBy;

protected:
	// === Variables
	float m_fMaxHealth;
	float m_fHealth;
	Team m_eTeam;

	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	Entity& operator=(const Entity& _rhs);
	virtual bool CheatEntered( Cheat _cheat ) const;

	// === Death
	//  IN : void
	//  OUT : void
	//
	//  Handles what happens when the Entity dies. Should be overriden by each entity, but STILL call base Entity's Death() to handle Object Tracking properly.
	virtual void Death();
	// === AlertTrackingObjects
	//  IN : void
	//  OUT : void
	//
	//  Calls the stored function from all Tracking Objects. Should be called when this Entity is no longer a valid target.
	void AlertTrackingObjects();
	// ========================= //

public:
	// === Constructor / Destructor === //
	Entity(GameObject* _owner, Transform* _transform);
	virtual ~Entity();
	// ================================ //

	// === Interface === //

	// === PostInit
	//  IN : map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//  OUT : void
	//
	//  Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void OnDisable() override;
	// === AddForTracking
	//  IN : void* - Pointer to the Tracking Object
	//       OnReleaseFunc - Function to call when this Entity is no longer valid { void Func(void) }
	//  OUT  : void
	//
	//  Adds an Object paired with the function that should be called when this entity is no longer valid.
	void AddForTracking(void* _trackingObject, OnReleaseFunc _func);
	// === RemoveFromObject
	//  IN : void* - Pointer to the Tracking Object
	//  OUT : void
	//
	//  Removes the given object from this Entity's array of Tracked by Objects
	void RemoveFromTracking(void* _trackingObject);
	virtual void TakeDamage(DamageInfo _damageInfo) = 0;
	// ================= //

	// === Accessors === //
	float GetMaxHealth() const;
	float GetHealth() const;
	Team GetTeam() const;
	// ================= //

	// === Mutators === //
	void SetMaxHealth(float _maxHealth);
	void SetHealth(float _health);
	void SetTeam(Team _team);
	// ================ //
};