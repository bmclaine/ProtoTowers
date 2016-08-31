///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Deals a specified amount of damage to the specified entity.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Action\ActionBehavior.h"

class GameObject;
class Entity;
class AIEntity;
struct DamageInfo;

class DealDamageBehavior : public ActionBehavior
{
private:
	Entity     *m_pEntity;
	DamageInfo *m_sInfo;

public:
	DealDamageBehavior(Entity* _entity, DamageInfo* _info, std::string& _name = std::string());
	~DealDamageBehavior();

	void Destroy() override;
	Status Update() override;
};