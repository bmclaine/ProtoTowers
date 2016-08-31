///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Spawns a boss minion shockwave at the boss's location.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "../Public/AI/BehaviorTrees/Action/SpawnBossShockwaveBehavior.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "../../Interface/Public/Engine.h"
#include "Components\Transform.h"
#include "GameObject.h"

SpawnBossShockwaveBehavior::SpawnBossShockwaveBehavior(AIEntity* _entity, std::string& _name)
	: ActionBehavior(_name)
	, m_pEntity(_entity)
{ }

SpawnBossShockwaveBehavior::~SpawnBossShockwaveBehavior()
{ }

void SpawnBossShockwaveBehavior::Destroy()
{ }

Status SpawnBossShockwaveBehavior::Update()
{
	GameObject* wave = Inferno::Instantiate("MinionBuffWave");
	wave->transform->SetParent(m_pEntity->transform);
	wave->transform->SetPosition(0.0f, 0.0f, 0.0f);

	/// Play the stomp sound
	Inferno::PostAudioEvent(416769819U);
	
	return Status::BT_SUCCESS;
}

