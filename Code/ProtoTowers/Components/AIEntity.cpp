///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Abstract Base class of Entity. All types of AI entities will derive from this.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes


// Engine Includes
#include "stdafx.h"
#include "Player.h"
#include "AIEntity.h"
#include "GameObject.h"
#include "Physics\Collider.h"
#include "Components\Animator.h"
#include "Components\NavAgent.h"
#include "Components\BehaviorTree.h"
#include "EntityTargetingWeights.h"
#include "Assets\AnimationController.h"

// Minions
#include "MiniMinion.h"
#include "TankMinion.h"
#include "BossMinion.h"

AIEntity::AIEntity(GameObject* _owner, Transform* _transform)
	: Entity(_owner, _transform)
{
	m_eState                = State::STATIONARY;
	m_pEnemyToTrack         = nullptr;
	m_pAnimator             = nullptr;
	m_pAnimationController  = nullptr;
	m_pNavAgent             = nullptr;
	m_bIsAttacking          = false;
	m_bIsDying              = false;
	m_bIsRunning            = false;
	m_bIsIdle               = false;
	m_bIsTakingDamage       = false;
	m_bIsCastingSpell       = false;
	m_bIsJumping            = false;
	m_bIsBuffed             = false;
	m_bIsUsingSpecial       = false;
	m_bIsDefending          = false;
	m_pAttackAnimationTimer = new InfernoTimer();
	m_pTakingDamageTimer    = new InfernoTimer();
	m_pStunTimer            = new InfernoTimer();
	m_pTakeDamageSoundTimer = new InfernoTimer();
	m_fDamageMultiplier     = 1.0f;
	m_fCurrentWeight        = 0.0f;
}


AIEntity::~AIEntity()
{
	SAFE_DELETE(m_pAttackAnimationTimer);
	SAFE_DELETE(m_pTakingDamageTimer);
	SAFE_DELETE(m_pStunTimer);
	SAFE_DELETE(m_pTakeDamageSoundTimer);
}


AIEntity& AIEntity::operator = (const AIEntity& _rhs)
{
	if (this != &_rhs)
	{
		m_attackTimer           = _rhs.m_attackTimer;
		m_pEnemyToTrack         = _rhs.m_pEnemyToTrack;
		m_eTeam                 = _rhs.m_eTeam;
		m_bIsAttacking          = _rhs.m_bIsAttacking;
//		m_bIsDying              = _rhs.m_bIsDying;
		m_bIsRunning            = _rhs.m_bIsRunning;
		m_bIsIdle               = _rhs.m_bIsIdle;
		m_bIsTakingDamage       = _rhs.m_bIsTakingDamage;
		m_bIsJumping            = _rhs.m_bIsJumping;
		m_bIsCastingSpell       = _rhs.m_bIsCastingSpell;
		m_bIsDefending          = _rhs.m_bIsDefending;
		m_bIsUsingSpecial       = _rhs.m_bIsUsingSpecial;
		m_pAnimator             = _rhs.m_pAnimator;
		m_pAnimationController  = _rhs.m_pAnimationController;
		m_eState                = _rhs.m_eState;
		m_pNavAgent             = _rhs.m_pNavAgent;

		m_pAttackAnimationTimer->Reset();
		m_pTakingDamageTimer->Reset();
		m_pStunTimer->Reset();
		m_pTakeDamageSoundTimer->Reset();
		
		m_vNearbyEnemies.clear();

		Entity::operator=(_rhs);
	}

	return *this;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void AIEntity::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	Entity::PostInit(objectMap, dataMap);

	m_pAnimator = gameObject->GetComponentInChildren<Animator>();
	m_pAnimationController = m_pAnimator->GetAnimationController();
	m_pNavAgent = gameObject->GetComponent<NavAgent>();
}

void AIEntity::FindClosestTarget()
{
	/// Remove weight from whatever you were tracking
	AIEntity* aiEntity = nullptr;
	if (m_pEnemyToTrack != nullptr)
		aiEntity = m_pEnemyToTrack->gameObject->GetComponent<AIEntity>();
	if (aiEntity != nullptr)
		aiEntity->RemoveInfluenceFromWeight(m_fInfluence);
	m_pEnemyToTrack = nullptr;
}

void AIEntity::Update()
{
	if (CheatEntered(Cheat::KILL_ENEMY_WAVE) && m_eTeam == Team::RED)
	{
		MiniMinion* mini = gameObject->GetComponent<MiniMinion>();
		TankMinion* tank = gameObject->GetComponent<TankMinion>();
		BossMinion* boss = gameObject->GetComponent<BossMinion>();
		if (mini != nullptr)
			mini->Death();
		if (tank != nullptr)
			tank->Death();
		if (boss != nullptr)
			boss->Death();
		// TODO: Add all of the other minion Death() calls
	}

	UpdateAnimationConditions();
}

void AIEntity::Init()
{ }

void AIEntity::OnEnable()
{
	SkinnedMeshRenderer* skin = gameObject->GetComponentInChildren<SkinnedMeshRenderer>();
	m_pAnimator = gameObject->GetComponentInChildren<Animator>();
	m_pAnimationController = m_pAnimator->GetAnimationController();
	m_pNavAgent = gameObject->GetComponent<NavAgent>();

	if (m_eTeam == Team::BLUE)
		skin->SetColor(Color(0.8f, 0.8f, 1.0f, 1.0f));
	else if (m_eTeam == Team::RED)
		skin->SetColor(Color(1.0f, 0.8f, 0.8f, 1.0f));

	m_eState = State::AGGRO;

	IComponent::OnEnable();
}

// IN: VOID
//
// OUT: VOID
//
// Description: Updates all of the necessary items with the animations
void AIEntity::UpdateAnimationConditions()
{
	if (m_pAnimationController != nullptr)
	{
		m_pAnimationController->SetBool( std::string("Idle"),         m_bIsIdle        );
		m_pAnimationController->SetBool( std::string("Running"),      m_bIsRunning     );
		m_pAnimationController->SetBool( std::string("Attacking"),    m_bIsAttacking   );
		m_pAnimationController->SetBool( std::string("TakingDamage"), m_bIsTakingDamage);
		m_pAnimationController->SetBool( std::string("Death"),        m_bIsDying       );
		m_pAnimationController->SetBool( std::string("Jumping"),      m_bIsJumping     );
		m_pAnimationController->SetBool( std::string("Casting"),      m_bIsCastingSpell);
		m_pAnimationController->SetBool( std::string("Special"),      m_bIsUsingSpecial);
		m_pAnimationController->SetBool( std::string("Defending"),    m_bIsDefending   );
	}
	else
	{
//		PRINT("AIEntity doesn't have an animation controller");
	}
}

// IN: VOID
//
// OUT: VOID
//
// Description: Handles the minions particles based on its current state
void AIEntity::HandleParticles() const
{ }

void AIEntity::StopTrackingEnemy(Entity* _entity)
{
	if (m_pEnemyToTrack == _entity)
		m_pEnemyToTrack = nullptr;

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i] == _entity)
		{
			std::iter_swap(m_vNearbyEnemies.begin() + i, m_vNearbyEnemies.end() - 1);
			m_vNearbyEnemies.erase(m_vNearbyEnemies.end() - 1);
			return;
		}
	}
}

// IN: AnimationType - The animation to set for the AIEntity
//
// OUT: VOID
//
// Description: Sets the animation booleans according to the one that you want to animate
void AIEntity::SetAnimation(AnimationType _animation)
{
	ASSERT(_animation >= NO_ANIMATION && _animation < ANIMATION_MAX, "AnimationType being set isn't valid");

	m_bIsAttacking    =
	m_bIsCastingSpell =
	m_bIsDying        =
	m_bIsIdle         =
	m_bIsRunning      =
	m_bIsJumping      =
	m_bIsTakingDamage =
	m_bIsUsingSpecial =
	m_bIsDefending    = false;

	switch (_animation)
	{
	case AnimationType::ATTACKING:
		m_bIsAttacking = true;
		break;

	case AnimationType::CAST_SPELL:
		m_bIsCastingSpell = true;
		break;
		
	case AnimationType::DEFENDING:
		m_bIsDefending = true;
		break;

	case AnimationType::DYING:
		m_bIsDying = true;
		break;

	case AnimationType::IDLE:
		m_bIsIdle = true;
		break;

	case AnimationType::JUMP:
		m_bIsJumping = true;
		break;

	case AnimationType::RUNNING:
		m_bIsRunning = true;
		break;

	case AnimationType::SPECIAL:
		m_bIsUsingSpecial = true;
		break;

	case AnimationType::TAKING_DAMAGE:
		m_bIsTakingDamage = true;
		break;

	case AnimationType::NO_ANIMATION:
	default:
		break;
	}
}

void AIEntity::SetAttackTimer(unsigned int _milliseconds)
{
	m_attackTimer.SetTime(_milliseconds);
}

// IN: GameObject* - The enemy to track
//
// OUT: VOID
//
// Description: The AIEntity will actively track this GameObject
void AIEntity::SetEnemyToTrack(Entity* _enemy)
{
	m_pEnemyToTrack = _enemy;
}

// IN: VOID
//
// OUT: GameObject* - The enemy this AIEntity is tracking
//
// Description: Returns the GameObject this AIEntity is tracking
Entity* AIEntity::GetEnemyToTrack() const
{
	return m_pEnemyToTrack;
}

// IN: VOID
//
// OUT: bool - If the AIEntity has a target
//
// Description: Returns true if this AIEntity is actively tracking an enemy
bool AIEntity::IsTrackingEnemy() const
{
	return (m_pEnemyToTrack != nullptr) ? true : false;
}

// IN: VOID
//
// OUT: VOID
//
// Description: Called when this minion has died
void AIEntity::Death()
{
	Entity::Death();

	/// Turn the minion off from being updated
	SetEnabled(false);

	/// Turn the collider off to prevent collisions with entities or projectiles
	std::vector<CapsuleCollider*> colliders = gameObject->GetComponents<CapsuleCollider>(true);
	unsigned int size = (unsigned int)colliders.size();
	for (unsigned int i = 0; i < size; ++i)
		colliders[i]->SetEnabled(false);

	m_vNearbyEnemies.clear();

}

bool AIEntity::IsAttacking() const
{
	return m_bIsAttacking;
}

bool AIEntity::IsTakingDamage() const
{
	return m_bIsTakingDamage;
}

bool AIEntity::IsDying() const
{
	return m_bIsDying;
}

bool AIEntity::IsRunning() const
{
	return m_bIsRunning;
}

// IN: VOID
//
// OUT: State - The current state of the AIEntity
//
// Description: Returns the current state of the AIEntity
State AIEntity::GetState() const
{
	return m_eState;
}

// IN: State - The new state for the AIEntity
//
// OUT: VOID
//
// Description: Sets the state for the AIEntity
void AIEntity::SetState(State _state)
{
	m_eState = _state;
}

void AIEntity::SetIsBuffed(bool isBuffed){
	m_bIsBuffed = isBuffed;
}

// IN: float - The new damage multiplier for the AIEntity
//
// OUT: VOID
//
// Description: Sets the multiplier for this AIEntity's damage
void AIEntity::SetDamageMultiplier(float _mul)
{
	m_fDamageMultiplier = _mul;
}

// IN: VOID
//
// OUT: Matrix4 - The matrix transform of this minions head bone
//
// Description: Returns the transform of this minions head bone.
void AIEntity::GetHeadBone(Matrix4& _matrix)
{ }

AnimationController* AIEntity::GetAnimationController()
{
	return m_pAnimationController;
}

float AIEntity::GetInfluence() const
{
	return m_fInfluence;
}

float AIEntity::GetMaxWeight() const
{
	return m_fMaxWeight;
}

float AIEntity::GetCurrentWeight() const
{
	return m_fCurrentWeight;
}

bool AIEntity::IsBuffed() const{
	return m_bIsBuffed;
}

void AIEntity::AddInfluenceToWeight(float _inf)
{
	m_fCurrentWeight += _inf;
}

void AIEntity::RemoveInfluenceFromWeight(float _inf)
{
	m_fCurrentWeight -= _inf;
}

