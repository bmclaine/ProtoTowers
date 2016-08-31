///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Abstract Base class of Entity. All types of AI entities will derive from this.
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once


// STL Includes


// Engine Includes
#include "Entity.h"
#include "InfernoTimer.h"
#include "Components\NavAgent.h"
#include "MathLib.h"

// This is to account for our use of MagnitudeSq()
#define DIST_SQR(dist) (dist * dist)

// -> The players projectile is set to do 20 damage
#define HEALTH_IN_HITS(x) (x * 20.0f)

// Each minions damage values
#define MINI_MINION_HEALTH          HEALTH_IN_HITS(4)
#define MINI_MINION_DMG             5.0f
#define MINI_MINION_ATTACK_TIME     800
#define MINI_MINION_ATTACK_DIST     DIST_SQR(2)

#define RANGED_MINION_HEALTH        HEALTH_IN_HITS(3)
#define RANGED_MINION_DMG           15.0f
#define RANGED_MINION_ATTACK_TIME   1300
#define RANGED_MINION_ATTACK_DIST   DIST_SQR(5)

#define TANK_MINION_HEALTH          HEALTH_IN_HITS(18)
#define TANK_MINION_DMG             20.0f
#define TANK_MINION_ATTACK_TIME     1200
#define TANK_MINION_ATTACK_DIST     DIST_SQR(4)

#define BOSS_MINION_HEALTH          HEALTH_IN_HITS(25)
#define BOSS_MINION_DMG             30.0f
#define BOSS_MINION_ATTACK_TIME     2000
#define BOSS_MINION_ATTACK_DIST     DIST_SQR(6)


// Forward Declarations
class Player;
class GameObject;
class Collider;
class Animator;
class NavAgent;
class BehaviorTree;
class MiniMinion;
class TankMinion;
class BossMinion;
class AnimationController;
class SkinnedMeshRenderer;

enum AnimationType { NO_ANIMATION, ATTACKING, CAST_SPELL, DEFENDING, DYING, IDLE, JUMP, RUNNING, SPECIAL, TAKING_DAMAGE, ANIMATION_MAX };
enum State { STATIONARY, FOLLOW_WAYPOINT, AGGRO, EVADE, STUNNED, STATE_MAX };
enum AIEntityType { TYPE_MINI, TYPE_RANGED, TYPE_TANK, TYPE_BOSS, TYPE_AI_ENTITY_MAX };

class AIEntity : public Entity
{
protected:
	InfernoTimer          m_attackTimer;          // -> The attack timer
	Entity               *m_pEnemyToTrack;        // -> The current enemy to track by this AIEntity
	float				  m_fDamageMultiplier;	  // -> The multiplier for this AIEntity's damage
	std::vector<Entity*>  m_vNearbyEnemies;       // -> A vector of possible targets
	State                 m_eState;               // -> The state for the AIEntity

	InfernoTimer *m_pAttackAnimationTimer;// TEMPORARY
	InfernoTimer *m_pTakingDamageTimer;   // TEMPORARY
	InfernoTimer *m_pStunTimer;           // -> The timer to keep the "Taking Damage" animation from playing so often
	InfernoTimer *m_pTakeDamageSoundTimer;

	bool m_bIsDying;        // -> Animation Bool
	bool m_bIsAttacking;    // -> Animation Bool
	bool m_bIsJumping;      // -> Animation Bool
	bool m_bIsCastingSpell; // -> Animation Bool
	bool m_bIsRunning;      // -> Animation Bool
	bool m_bIsIdle;         // -> Animation Bool
	bool m_bIsTakingDamage; // -> Animation Bool
	bool m_bIsUsingSpecial; // -> Animation Bool
	bool m_bIsDefending; // -> Animation Bool

	Animator             *m_pAnimator;
	AnimationController  *m_pAnimationController;

	NavAgent             *m_pNavAgent; // -> The navAgent of the gameObject

	float m_fInfluence;    // The entities influence on its targets weight
	float m_fMaxWeight;    // The max weight the entity could have
	float m_fCurrentWeight;// The current weight the entity has from other influences

	bool m_bIsBuffed;

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Handles the minions particles based on its current state
	virtual void HandleParticles( void ) const;
	void StopTrackingEnemy( Entity* _entity );

public:
	// Defaults
	AIEntity(GameObject* _owner, Transform* _transform);
	virtual ~AIEntity( void ) = 0;
	AIEntity& operator = ( const AIEntity& _rhs);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void Init() override;
	virtual void OnEnable( void ) override;

	virtual void FindClosestTarget() = 0;
	virtual AIEntityType GetType(void) const = 0;

	float GetInfluence() const;
	float GetMaxWeight() const;
	float GetCurrentWeight() const;
	void AddInfluenceToWeight(float _inf);
	void RemoveInfluenceFromWeight(float _inf);

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Updates all of the necessary items with the animations
	void UpdateAnimationConditions(void);

	virtual void Update( void );

	// IN: AnimationType - The animation to set for the AIEntity
	//
	// OUT: VOID
	//
	// Description: Sets the animation booleans according to the one that you want to animate
	void SetAnimation(AnimationType _animation);

	// IN: unsigned int - Milliseconds to set the timer to
	//
	// OUT: VOID
	//
	// Description: Sets the AIEntities attack timer to the parameter
	void SetAttackTimer(unsigned int _milliseconds);

	// IN: GameObject* - The enemy to track
	//
	// OUT: VOID
	//
	// Description: The AIEntity will actively track this GameObject
	void SetEnemyToTrack(Entity* _enemy);

	// IN: VOID
	//
	// OUT: GameObject* - The enemy this AIEntity is tracking
	//
	// Description: Returns the GameObject this AIEntity is tracking
	Entity* GetEnemyToTrack( void ) const;

	// IN: VOID
	//
	// OUT: bool - If the AIEntity has a target
	//
	// Description: Returns true if this AIEntity is actively tracking an enemy
	bool IsTrackingEnemy( void ) const;

	// IN: VOID
	//
	// OUT: VOID
	//
	// Description: Called when this minion has died
	void Death( void ) override;

	// TEMPORARY
	bool IsAttacking   ( void ) const;
	bool IsTakingDamage( void ) const;
	bool IsDying       ( void ) const;
	bool IsRunning     ( void ) const;

	// IN: VOID
	//
	// OUT: State - The current state of the AIEntity
	//
	// Description: Returns the current state of the AIEntity
	State GetState( void ) const;

	// IN: State - The new state for the AIEntity
	//
	// OUT: VOID
	//
	// Description: Sets the state for the AIEntity
	void SetState(State _state);

	// IN: bool 
	//
	// OUT: VOID
	//
	// Description: Sets whether the minion is buffed or not
	void SetIsBuffed(bool isBuffed);
	
	// IN: float - The new damage multiplier for the AIEntity
	//
	// OUT: VOID
	//
	// Description: Sets the multiplier for this AIEntity's damage
	void SetDamageMultiplier(float _mul);

	// IN: VOID
	//
	// OUT: Matrix4 - The matrix transform of this minions head bone
	//
	// Description: Returns the transform of this minions head bone.
	virtual void GetHeadBone(Matrix4& _matrix);

	bool IsBuffed() const;

	AnimationController* GetAnimationController();
};


