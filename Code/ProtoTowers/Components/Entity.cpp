#include "stdafx.h"
#include "Entity.h"
#include "InputManager.h"
#include "Engine.h"

// ===== Constructor / Destructor ===== //
Entity::Entity(GameObject* _owner, Transform* _transform) : IComponent(_owner, _transform)
{
	m_eTeam = NO_TEAM;
}

Entity::~Entity()
{
	
}
// ==================================== //

// ===== Interface ===== //

// === PostInit
//  IN : map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//  OUT : void
//
//  Initializes object variables
void Entity::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "MaxHealth";
	Property* prop = data->GetProperty(propName);
	m_fMaxHealth = (float)atof(prop->value.c_str());

	propName = "Health";
	prop = data->GetProperty(propName);
	m_fHealth = (float)atof(prop->value.c_str());

	propName = "Team";
	prop = data->GetProperty(propName);
	m_eTeam = (Team)atoi(prop->value.c_str());
}

void Entity::OnDisable()
{
	AlertTrackingObjects();

	IComponent::OnDisable();
}

bool Entity::CheatEntered(Cheat _cheat) const
{
	ASSERT(_cheat >= 0 && _cheat < CHEAT_MAX, "The cheat you're attempting to check for is invalid.");

	bool result = false;

	switch (_cheat)
	{
	case TOGGLE_GODMODE:
		if (InputManager::IsKeyDown(Key::Control) && InputManager::IsKeyPressed(Key::NumPad0) || InputManager::IsButtonPressed(0, Button::DPad_Up))
		{
			result = true;
		}
		break;

	case KILL_ENEMY_WAVE:
		if (InputManager::IsKeyDown(Key::Control) && InputManager::IsKeyPressed(Key::NumPad1) || InputManager::IsButtonPressed(0, Button::DPad_Left))
		{
			result = true;
		}
		break;

	case SPAWN_FRIENDLY_WAVE:
		if (InputManager::IsKeyDown(Key::Control) && InputManager::IsKeyPressed(Key::NumPad2) || InputManager::IsButtonPressed(0, Button::DPad_Right))
		{
			result = true;
		}
		break;

	case GIVE_RESOURCES:
		if (InputManager::IsKeyDown(Key::Control) && InputManager::IsKeyPressed(Key::NumPad3) || InputManager::IsButtonPressed(0, Button::DPad_Down))
		{
			result = true;
		}
		break;
	}
	return result;
}

// === AddForTracking
//  IN : void* - Pointer to the Tracking Object
//       OnReleaseFunc - Function to call when this Entity is no longer valid { void Func(void) }
//  OUT  : void
//
//  Adds an Object paired with the function that should be called when this entity is no longer valid.
void Entity::AddForTracking(void* _trackingObject, OnReleaseFunc _func)
{
	TrackingObject tObj = { _trackingObject, _func };
	m_vObjectsTrackedBy.push_back(tObj);
}

// === RemoveFromObject
//  IN : void* - Pointer to the Tracking Object
//  OUT : void
//
//  Removes the given object from this Entity's array of Tracked by Objects
void Entity::RemoveFromTracking(void* _trackingObject)
{
	size_t count = m_vObjectsTrackedBy.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vObjectsTrackedBy[i].Object == _trackingObject) {
			if (count == 1) {
				m_vObjectsTrackedBy.erase(m_vObjectsTrackedBy.begin());
				break;
			}
			else {
				std::iter_swap(m_vObjectsTrackedBy.begin() + i, m_vObjectsTrackedBy.end() - 1);
				m_vObjectsTrackedBy.erase(m_vObjectsTrackedBy.end() - 1);
				break;
			}
		}
	}
}
// ===================== //

// ===== Private Interface ===== //

// === Death
//  IN : void
//  OUT : void
//
//  Handles what happens when the Entity dies. Should be overriden by each entity, but STILL call base Entity's Death() to handle Object Tracking properly.
void Entity::Death()
{
	AlertTrackingObjects();
}

// === AlertTrackingObjects
//  IN : void
//  OUT : void
//
//  Calls the stored function from all Tracking Objects. Should be called when this Entity is no longer a valid target.
void Entity::AlertTrackingObjects()
{
	size_t count = m_vObjectsTrackedBy.size();
	for (size_t i = 0; i < count; ++i) {
		m_vObjectsTrackedBy[i].Func(this);
	}

	m_vObjectsTrackedBy.clear();
}
// ============================= //

// === Operators === //
IComponent& Entity::operator=(IComponent& _rhs)
{
	Entity* rhsEntity = dynamic_cast<Entity*>(&_rhs);
	if (rhsEntity == nullptr) {
		return *this;
	}

	return operator=(*rhsEntity);
}

Entity& Entity::operator=(const Entity& _rhs)
{
	if (this != &_rhs) {
		m_fHealth = _rhs.m_fHealth;
		m_fMaxHealth = _rhs.m_fMaxHealth;
		m_fcFlags = _rhs.m_fcFlags;
		m_eTeam = _rhs.m_eTeam;
//		m_vObjectsTrackedBy = _rhs.m_vObjectsTrackedBy;
	}

	return *this;
}
// ================= //

// ===== Accessors ===== //
float Entity::GetMaxHealth() const
{
	return m_fMaxHealth;
}

float Entity::GetHealth() const
{
	return m_fHealth;
}

Team Entity::GetTeam() const
{
	return m_eTeam;
}
// ===================== //

// ===== Mutators ===== //
void Entity::SetMaxHealth(float _maxHealth)
{
	m_fMaxHealth = _maxHealth;
}

void Entity::SetHealth(float _health)
{
	m_fHealth = _health;
}

void Entity::SetTeam(Team _team)
{
	ASSERT(_team >= NO_TEAM || _team < TEAM_MAX, "Team assigned is invalid");
	m_eTeam = _team;
}
// ==================== //