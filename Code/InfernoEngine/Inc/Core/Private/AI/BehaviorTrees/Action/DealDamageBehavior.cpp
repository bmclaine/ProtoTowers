///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Deals a specified amount of damage to the specified entity.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Action\DealDamageBehavior.h"
#include "../../../../ProtoTowers/Components/Entity.h"
#include "GameObject.h"
#include "../../../../ProtoTowers/Core/iDamagable.h"
/// Types of entities(possible targets)
#include "../../../../ProtoTowers/Components/AIEntity.h"

DealDamageBehavior::DealDamageBehavior(Entity* _entity, DamageInfo* _info, std::string& _name)
	: ActionBehavior(_name)
	, m_pEntity(_entity)
	, m_sInfo(_info)
{ }

DealDamageBehavior::~DealDamageBehavior()
{ }

void DealDamageBehavior::Destroy()
{
	if (m_sInfo != nullptr)
	{
		delete m_sInfo;
		m_sInfo = nullptr;
	}
}

Status DealDamageBehavior::Update()
{
	AIEntity* aiEntity = m_pEntity->gameObject->GetComponent<AIEntity>();
	if (aiEntity->GetEnemyToTrack() != nullptr)
	{

		aiEntity->GetEnemyToTrack()->TakeDamage(*m_sInfo);
		return Status::BT_SUCCESS;
	}
	return Status::BT_FAILURE;
}

