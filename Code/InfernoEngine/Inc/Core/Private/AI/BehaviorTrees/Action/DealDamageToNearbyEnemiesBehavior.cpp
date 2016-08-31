///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: ActionBehavior derives from BaseBehavior.
//              Is the base class for all action based behaviors.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Action/DealDamageToNearbyEnemiesBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "../../../../ProtoTowers/Components/BossMinion.h"
#include "../../../../ProtoTowers/Components/BossShockwave.h"
#include "Components\Transform.h"

#include "../../Interface/Public/Engine.h"
#include "GameObject.h"

DealDamageToNearbyEnemiesBehavior::DealDamageToNearbyEnemiesBehavior(AIEntity* _entity, std::string& _name)
	: ActionBehavior(_name)
	, m_pEntity(_entity)
{ }

DealDamageToNearbyEnemiesBehavior::~DealDamageToNearbyEnemiesBehavior()
{ }

void DealDamageToNearbyEnemiesBehavior::Destroy()
{ }

Status DealDamageToNearbyEnemiesBehavior::Update()
{
	BossMinion* boss = m_pEntity->gameObject->GetComponent<BossMinion>();
	boss->DamageNearbyEnemies();

	return Status::BT_SUCCESS;
}

