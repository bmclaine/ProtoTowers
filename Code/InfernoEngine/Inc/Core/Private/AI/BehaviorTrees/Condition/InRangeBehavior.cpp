///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Handles condition statements in the behavior tree. Resembles an 'if' statement.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "AI\BehaviorTrees\Condition\InRangeBehavior.h"
#include "Components\NavAgent.h"
#include "Components\Transform.h"
#include "GameObject.h"
#include "Physics\BoxCollider.h"
#include "Physics\CapsuleCollider.h"
#include "../../../../ProtoTowers/Components/Entity.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "../../../../ProtoTowers/Components/BaseTower.h"
#include "../../../../ProtoTowers/Components/Player.h"

InRangeBehavior::InRangeBehavior(NavAgent* _agent, std::string& _name)
	: ConditionBehavior(_name)
	, m_pAgent(_agent)
{ }

InRangeBehavior::~InRangeBehavior()
{ }

void InRangeBehavior::Destroy()
{ }

Status InRangeBehavior::Update()
{
	AIEntity* entity = m_pAgent->gameObject->GetComponent<AIEntity>();

	if (entity->IsTrackingEnemy())
	{
		Player* player = entity->GetEnemyToTrack()->gameObject->GetComponent<Player>();
		BaseTower* tower = entity->GetEnemyToTrack()->gameObject->GetComponent<BaseTower>();
		if (tower != nullptr)
			return InRangeOfTower(entity, tower);
		else if (player != nullptr)
			return InRangeOfPlayer(entity, player);
		else
			return InRangeOfEntity(entity);
	}
	return Status::BT_FAILURE;
}

Status InRangeBehavior::InRangeOfTower(AIEntity* _entity, BaseTower* _tower)
{
	/// Every tower type has a box collider
	BoxCollider* box = _tower->gameObject->GetComponent<BoxCollider>();
	if (box != nullptr)
	{
		float radiusOfBothColliders = m_pAgent->GetAgentRadius() + _tower->gameObject->GetComponent<BoxCollider>()->GetSize().x;
		radiusOfBothColliders *= 2.0f; /// Scaling to match the LengthSq() call being made below
		float distanceBetweenCenterPoints = (box->GetCenter() - _entity->transform->GetPosition()).LengthSq();
		float leeway = 0.0f;
		if (_entity->GetType() == AIEntityType::TYPE_BOSS)
			leeway += 15.0f;
		else if (_entity->GetType() == AIEntityType::TYPE_MINI)
			leeway = 7.0f;
		else
			leeway += 2.0f;

		if (distanceBetweenCenterPoints - leeway < radiusOfBothColliders)
			return Status::BT_SUCCESS;
	}
	/// Spawners only have a capsulecollider
	CapsuleCollider* capsule = _tower->gameObject->GetComponent<CapsuleCollider>();
	if (capsule != nullptr)
	{
		float radiusOfBothColliders = radiusOfBothColliders = m_pAgent->GetAgentRadius() + _tower->gameObject->GetComponent<CapsuleCollider>()->GetRadius();
		radiusOfBothColliders *= 2.0f; /// Scaling to match the LengthSq() call being made below
		float distanceBetweenCenterPoints = (capsule->GetCenter() - _entity->transform->GetPosition()).LengthSq();
		float leeway = 1.5f;

		if (distanceBetweenCenterPoints - leeway < radiusOfBothColliders)
			return Status::BT_SUCCESS;
	}

	return Status::BT_FAILURE;
}

Status InRangeBehavior::InRangeOfEntity(AIEntity* _entity)
{
	float radiusOfBothColliders = m_pAgent->GetAgentRadius() + _entity->GetEnemyToTrack()->gameObject->GetComponent<NavAgent>()->GetAgentRadius();
	radiusOfBothColliders *= 2.0f; /// Scaling to match the LengthSq() call being made below
	float distanceBetweenCenterPoints = (_entity->GetEnemyToTrack()->transform->GetPosition() - _entity->transform->GetPosition()).LengthSq();
	float leeway = 0.0f;
	if (_entity->GetType() == AIEntityType::TYPE_MINI)
		leeway = 1.0f;
	else if (_entity->GetType() == AIEntityType::TYPE_TANK)
		leeway = 2.5f;
	else if (_entity->GetType() == AIEntityType::TYPE_BOSS)
		leeway = 3.5f;

	if (distanceBetweenCenterPoints - leeway < radiusOfBothColliders)
		return Status::BT_SUCCESS;
	return Status::BT_FAILURE;
}

Status InRangeBehavior::InRangeOfPlayer(AIEntity* _entity, Player* _player)
{
	CapsuleCollider* capsule = _player->gameObject->GetComponent<CapsuleCollider>();
	float radiusOfBothColliders = m_pAgent->GetAgentRadius() + capsule->GetRadius();
	radiusOfBothColliders *= 2.0f; /// Scaling to match the LengthSq() call being made below
	float distanceBetweenCenterPoints = (_player->transform->GetPosition() - _entity->transform->GetPosition()).LengthSq();
	float leeway = 0.0f;
	if (_entity->GetType() == AIEntityType::TYPE_MINI)
		leeway = 2.0f;
	else if (_entity->GetType() == AIEntityType::TYPE_TANK)
		leeway = 8.0f;

	if (distanceBetweenCenterPoints - leeway < radiusOfBothColliders)
		return Status::BT_SUCCESS;
	return Status::BT_FAILURE;
}