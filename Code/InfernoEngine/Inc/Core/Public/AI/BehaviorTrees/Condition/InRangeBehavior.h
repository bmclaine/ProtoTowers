///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Checks the distance between two transforms positions.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "AI\BehaviorTrees\Condition\ConditionBehavior.h"
#include "MathLib.h"

class NavAgent;
class Transform;
class Entity;
class BaseTower;
class AIEntity;
class BoxCollider;
class CapsuleCollider;
class Player;

class InRangeBehavior : public ConditionBehavior
{
private:
	NavAgent* m_pAgent;

	/// Private Interface
	Status InRangeOfTower(AIEntity* _entity, BaseTower* _tower);
	Status InRangeOfEntity(AIEntity* _entity);
	Status InRangeOfPlayer(AIEntity* _entity, Player* _player);

public:
	InRangeBehavior(NavAgent* _agent, std::string& _name = std::string());
	~InRangeBehavior();

	void Destroy() override;
	Status Update() override;
};