///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: BehaviorTree is the class that will be the base of everything.
//              It will have one BaseBehavior child that will act as the Tree's root.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Components\BehaviorTree.h"
#include "Components\NavAgent.h"
#include "../../../../ProtoTowers/Components/AIEntity.h"
#include "GameObject.h"
#include "GlobalIncludes.h"
#include "Components\Transform.h"
#include "Components\Animator.h"

/// Actions
#include "AI\BehaviorTrees\Action\MoveToBehavior.h"
#include "AI\BehaviorTrees\Action\MoveToTargetBehavior.h"
#include "AI\BehaviorTrees\Action\MoveToWaypointBehavior.h"
#include "AI\BehaviorTrees\Action\RequestPathBehavior.h"
#include "AI\BehaviorTrees\Action\DealDamageBehavior.h"
#include "AI\BehaviorTrees\Action\SetDestinationBehavior.h"
#include "AI\BehaviorTrees\Action\FindClosestTargetBehavior.h"
#include "AI\BehaviorTrees\Action\DealDamageToNearbyEnemiesBehavior.h"
#include "AI\BehaviorTrees\Action\SpawnBossShockwaveBehavior.h"
#include "AI\BehaviorTrees\Action\TurnToTarget.h"
#include "AI\BehaviorTrees\Action\ChargeAtTargetBehavior.h"
#include "AI\BehaviorTrees\Action\ReturnStatusBehavior.h"
#include "AI\BehaviorTrees\Action\LookAtBehavior.h"
/// Animations
#include "AI\BehaviorTrees\Animate\RunAnimationBehavior.h"
#include "AI\BehaviorTrees\Animate\AttackAnimationBehavior.h"
#include "AI\BehaviorTrees\Animate\IdleAnimationBehavior.h"
#include "AI\BehaviorTrees\Animate\CastSpellAnimationBehavior.h"
#include "AI\BehaviorTrees\Animate\JumpAnimationBehavior.h"
#include "AI\BehaviorTrees\Animate\SpecialAnimationBehavior.h"
/// Decorators
#include "AI\BehaviorTrees\Decorator\TimerBehavior.h"
#include "AI\BehaviorTrees\Decorator\WaitBehavior.h"
#include "AI\BehaviorTrees\Decorator\DoOnceBehavior.h"
#include "AI\BehaviorTrees\Decorator\RepeatBehavior.h"
#include "AI\BehaviorTrees\Decorator\RepeatForTime.h"
#include "AI\BehaviorTrees\Decorator\InvertBehavior.h"
#include "AI\BehaviorTrees\Decorator\FailureToRunningBehavior.h"
/// Composite
#include "AI\BehaviorTrees\Composite\ParallelBehavior.h"
#include "AI\BehaviorTrees\Composite\SequenceBehavior.h"
#include "AI\BehaviorTrees\Composite\SelectorBehavior.h"
/// Condition
#include "AI\BehaviorTrees\Condition\InRangeBehavior.h"
#include "AI\BehaviorTrees\Condition\IsOvercrowdedBehavior.h"
#include "AI\BehaviorTrees\Condition\IsStunnedBehavior.h"
#include "AI\BehaviorTrees\Condition\IsStationaryBehavior.h"
#include "AI\BehaviorTrees\Condition\IsBossStuckBehavior.h"
#include "AI\BehaviorTrees\Condition\ORBehavior.h"
#include "AI\BehaviorTrees\Condition\HasTargetBehavior.h"
#include "AI\BehaviorTrees\Condition\HasTravelledDistanceBehavior.h"

BehaviorTree::BehaviorTree(GameObject* const _gameObject, Transform* const _transform)
	: IComponent(_gameObject, _transform)
	, m_pChild(nullptr)
	, m_pCutsceneChild(nullptr)
	, m_sName(std::string("BehaviorTree"))
	, m_pAgent(nullptr)
	, m_pEntity(nullptr)
	, m_fRequestTime(0.3f)
	, m_bBuiltTree(false)
	, m_bPlayCutsceneTree(false)
{ }

IComponent& BehaviorTree::operator=(IComponent& _assign)
{
	BehaviorTree* tree = (BehaviorTree*)&_assign;
	if (tree == nullptr)
		return *this;

	return operator=(*tree);
}

IComponent* BehaviorTree::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	BehaviorTree* temp = new BehaviorTree(_gameObject, _transform);

	*temp = *this;

	return temp;
}

BehaviorTree& BehaviorTree::operator = (const BehaviorTree& _assign)
{
	if (this != &_assign)
	{
		m_sName = _assign.m_sName;
		m_fcFlags = _assign.m_fcFlags;
	}
	return *this;
}

BehaviorTree::~BehaviorTree()
{
	Destroy();
}

void BehaviorTree::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{ }

void BehaviorTree::Destroy()
{
	if (m_pChild != nullptr)
		m_pChild->Destroy();

	if (m_pCutsceneChild != nullptr)
		m_pCutsceneChild->Destroy();

	SAFE_DELETE(m_pChild);
	SAFE_DELETE(m_pCutsceneChild);
}

void BehaviorTree::OnEnable()
{
	IComponent::OnEnable();

	m_pAgent = gameObject->GetComponent<NavAgent>();
	m_pEntity = gameObject->GetComponent<AIEntity>();

	ASSERT(m_pAgent != nullptr,  "BehaviorTree::OnEnable() -> A BehaviorTree component was added to a gameObject that has no NavAgent");
	ASSERT(m_pEntity != nullptr, "BehaviorTree::OnEnable() -> A BehaviorTree component was added to a gameObject that has no AIEntity");

	if (m_bBuiltTree == false)
	{
		/// Build the tree according to the minionType
		switch (m_pAgent->GetMinionType())
		{
		case MinionType::MINI:
			CreateMeleeTree();
			break;

		case MinionType::RANGED:
			CreateRangedTree();
			break;

		case MinionType::TANK:
			CreateTankTree();
			break;

		case MinionType::BOSS:
			CreateBossTree();
			break;

		default:
			CreateTutorialTree();
			break;
		}

		CreateCutsceneTree();
		m_bBuiltTree = true;
	}
}

void BehaviorTree::Reset()
{
	m_pChild->Reset();
}

Status BehaviorTree::Tick()
{
	ASSERT(m_pChild != nullptr, "BehaviorTree::Tick() -> Behavior Tree doesn't have a non-cutscene tree.");
	ASSERT(m_pCutsceneChild != nullptr, "BehaviorTree::Tick() -> Behavior Tree doesn't have a cutscene tree.");

	if (m_bPlayCutsceneTree)
		return m_pCutsceneChild->Tick();
	else
		return m_pChild->Tick();
}

void BehaviorTree::CreateTutorialTree()
{
	// High Level Roots
	SequenceBehavior *mainRoot = new SequenceBehavior(std::string("MainRoot"));
	/// Play Idle
	IdleAnimationBehavior *idleAnim = new IdleAnimationBehavior(m_pEntity, 1.0f, std::string("IdleAnimation"));

	m_pChild = mainRoot;
	mainRoot->AddChild(idleAnim);
}

void BehaviorTree::CreateMeleeTree()
{
	// High Level Roots
	ParallelBehavior *mainParallel = new ParallelBehavior(Policy::REQUIRE_ALL, Policy::REQUIRE_ALL, std::string("MainParallel"));
	SequenceBehavior *mainRoot = new SequenceBehavior(std::string("MainRoot"));
	/// MoveRoot
	IsStunnedBehavior         *isStunned         = new IsStunnedBehavior(m_pEntity, std::string("IdleIfStunned"));
	SequenceBehavior          *moveRoot          = new SequenceBehavior(std::string("MoveRoot"));
	RunAnimationBehavior      *runAnim           = new RunAnimationBehavior(m_pEntity, 1.0f, std::string("RunAnimation"));
	SetDestinationBehavior    *setDestination    = new SetDestinationBehavior(m_pEntity, std::string("SetDestination"));
	FindClosestTargetBehavior *findClosestTarget = new FindClosestTargetBehavior(m_pEntity, std::string("FindClosestTarget"));
	MoveToBehavior            *moveToDestination = new MoveToBehavior(m_pAgent, std::string("MoveToDestination"));
	/// Request Path
	RequestPathBehavior *requestPath = new RequestPathBehavior(m_pAgent, std::string("RequestNewPath"));
	TimerBehavior       *timer       = new TimerBehavior(m_fRequestTime, true, requestPath, std::string("WaitToRequestPath"));
	/// AttackRoot
	SequenceBehavior        *attackRoot    = new SequenceBehavior(std::string("AttackRoot"));
	InRangeBehavior         *targetInRange = new InRangeBehavior(m_pAgent, std::string("IsTargetInRange"));
	AttackAnimationBehavior *attackAnim    = new AttackAnimationBehavior(m_pEntity, 1.0f, std::string("PlayAttackAnimation"));
	WaitBehavior            *wait          = new WaitBehavior(0.8f, std::string("WaitForAttackingAnimationToComplete"));
	InRangeBehavior         *inRangeDamage = new InRangeBehavior(m_pAgent, std::string("CheckDistanceBeforeDealingDamage"));
	DealDamageBehavior      *dealDamage    = new DealDamageBehavior(m_pEntity, new DamageInfo(MINI_MINION_DMG, m_pEntity, 1.0f, 1.0f, 3.0f), std::string("DamageTheTarget"));
	

	/// -----------------------------------
	m_pChild = mainParallel;
	/// -----------------------------------
	mainParallel->AddChild(timer);
	mainParallel->AddChild(mainRoot);
	/// -----------------------------------
	mainRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("BreakFromTreeIfStationary")));
	mainRoot->AddChild(isStunned);
	mainRoot->AddChild(moveRoot);
	mainRoot->AddChild(attackRoot);
	/// -----------------------------------
	moveRoot->AddChild(findClosestTarget);
	moveRoot->AddChild(setDestination);
	moveRoot->AddChild(runAnim);
	moveRoot->AddChild(moveToDestination);
	/// -----------------------------------
	attackRoot->AddChild(targetInRange);
	attackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IdleBeforeAttackAnim")));
	attackRoot->AddChild(attackAnim);
	attackRoot->AddChild(wait);
	attackRoot->AddChild(inRangeDamage);
	attackRoot->AddChild(dealDamage);
	/// -----------------------------------
}

void BehaviorTree::CreateRangedTree()
{ /* To Be implemented */ }

void BehaviorTree::CreateTankTree()
{
#if 1
	// High Level Roots
	ParallelBehavior *mainParallel = new ParallelBehavior(Policy::REQUIRE_ALL, Policy::REQUIRE_ALL, std::string("MainParallel"));
	SequenceBehavior *mainRoot     = new SequenceBehavior(std::string("MainRoot"));
	/// Request Path
	RequestPathBehavior *requestPath = new RequestPathBehavior(m_pAgent, std::string("RequestNewPath"));
	TimerBehavior       *pathTimer   = new TimerBehavior(m_fRequestTime, true, requestPath, std::string("WaitToRequestPath"));
	/// Move
	IsStunnedBehavior         *isStunned         = new IsStunnedBehavior(m_pEntity, std::string("PlayIdleIfStunned"));
	SequenceBehavior          *moveRoot          = new SequenceBehavior(std::string("MoveRoot"));
	SetDestinationBehavior    *setDestination    = new SetDestinationBehavior(m_pEntity, std::string("SetDestination"));
	RunAnimationBehavior      *runAnim           = new RunAnimationBehavior(m_pEntity, 1.0f, std::string("RunAnimation"));
	FindClosestTargetBehavior *findClosestTarget = new FindClosestTargetBehavior(m_pEntity, std::string("FindClosestTarget"));
	MoveToBehavior            *moveToDestination = new MoveToBehavior(m_pAgent, std::string("MoveToDestination"));
	/// Attack
	SelectorBehavior            *attackRoot             = new SelectorBehavior(std::string("AttackRoot"));
	/// CloseRangeSwing Attack
	SequenceBehavior            *closeRangeAttackRoot   = new SequenceBehavior(std::string("CloseRangeAttackRoot"));
	/// Charge Attack
	SequenceBehavior            *chargeAttackRoot       = new SequenceBehavior(std::string("ChargeAttackRoot"));
	HasTargetBehavior           *hasTarget              = new HasTargetBehavior(m_pEntity, std::string("DoesTheTankHaveAnEnemy"));
	SpecialAnimationBehavior    *specialAnim            = new SpecialAnimationBehavior(m_pEntity, 1.0f, std::string("SpecialAnimation"));
	ParallelBehavior            *lookAtParallel         = new ParallelBehavior(Policy::REQUIRE_ALL, Policy::REQUIRE_ONE, std::string("AttackParallel"));
	TimerBehavior               *playIdleTimer          = new TimerBehavior(1.2f, true, new IdleAnimationBehavior(m_pEntity, 1.0f, std::string("IdleAnim")), std::string("IdleAnimationTimer"));
	ParallelBehavior            *chargeParallel         = new ParallelBehavior(Policy::REQUIRE_ALL, Policy::REQUIRE_ONE, std::string("ChargeParallel"));
	ChargeAtTargetBehavior      *chargeToTarget         = new ChargeAtTargetBehavior(m_pAgent, std::string("ChargeToTarget"));
//	HasTavelledDistanceBehavior *travelledDist          = new HasTavelledDistanceBehavior(m_pEntity, m_pAgent, 25.0f, true, std::string("HasTankTravelledMaxChargeDist"));
	ReturnStatusBehavior        *returnSuccess          = new ReturnStatusBehavior(Status::BT_SUCCESS, std::string("ReturnSuccess"));
	TimerBehavior               *returnSuccessTimer     = new TimerBehavior(1.8f, true, returnSuccess, std::string("ReturnSuccessTimer")); // Charge at the target for one second
	InRangeBehavior             *inRange                = new InRangeBehavior(m_pAgent, std::string("InRangeToAttack"));
	InRangeBehavior             *inRangeWhileCharging   = new InRangeBehavior(m_pAgent, std::string("InRangeWhileCharging"));
	FailureToRunningBehavior    *failureToRunning       = new FailureToRunningBehavior(inRangeWhileCharging, std::string("FailureToRunning"));
	ORBehavior                  *ranForTimeORInRange    = new ORBehavior(returnSuccessTimer, failureToRunning);
	ParallelBehavior            *turnToAfterChargeParallel = new ParallelBehavior(Policy::REQUIRE_ALL, Policy::REQUIRE_ONE, std::string("TurnToAfterChargeParallel"));
	// ---------------------------------------------------------------------------------------------------------------------------
	m_pChild = mainParallel;
	mainParallel->AddChild(pathTimer);
	mainParallel->AddChild(mainRoot);
	// ---------------------------------------------------------------------------------------------------------------------------
	mainRoot->AddChild(isStunned);
	mainRoot->AddChild(moveRoot);
	mainRoot->AddChild(attackRoot);
	// ---------------------------------------------------------------------------------------------------------------------------
	attackRoot->AddChild(closeRangeAttackRoot);
	attackRoot->AddChild(chargeAttackRoot);
	// ---------------------------------------------------------------------------------------------------------------------------
	moveRoot->AddChild(findClosestTarget);
	moveRoot->AddChild(setDestination);
	moveRoot->AddChild(runAnim);
	moveRoot->AddChild(moveToDestination);
	// ---------------------------------------------------------------------------------------------------------------------------
	closeRangeAttackRoot->AddChild(new InRangeBehavior(m_pAgent, std::string("InRange")));
	closeRangeAttackRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("IsStationary")));
	closeRangeAttackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IdleBeforeAttackAnim")));
	closeRangeAttackRoot->AddChild(new AttackAnimationBehavior(m_pEntity, 1.0f, std::string("AttackAnim")));
	closeRangeAttackRoot->AddChild(new WaitBehavior(0.8f, std::string("WaitAfterCloseAttack")));
	closeRangeAttackRoot->AddChild(new InRangeBehavior(m_pAgent, std::string("InRange")));
	closeRangeAttackRoot->AddChild(new DealDamageBehavior(m_pEntity, new DamageInfo(TANK_MINION_DMG, m_pEntity, 1.0f, 1.0f, 1.5f), std::string("DealSwingDamage")));
	// ---------------------------------------------------------------------------------------------------------------------------
	chargeAttackRoot->AddChild(hasTarget);
	chargeAttackRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("IsStationary")));
	chargeAttackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	chargeAttackRoot->AddChild(specialAnim);
	chargeAttackRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("IsStationary")));
	chargeAttackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	chargeAttackRoot->AddChild(playIdleTimer);
	chargeAttackRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("IsStationary")));
	chargeAttackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	chargeAttackRoot->AddChild(lookAtParallel);
	chargeAttackRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("IsStationary")));
	chargeAttackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	chargeAttackRoot->AddChild(new RunAnimationBehavior(m_pEntity, 2.5f, std::string("RunAnim x2.5"))); // Play the animation at double speed
	chargeAttackRoot->AddChild(chargeParallel);
	chargeAttackRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("IsStationary")));
	chargeAttackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	chargeAttackRoot->AddChild(turnToAfterChargeParallel);
	chargeAttackRoot->AddChild(inRange);
	chargeAttackRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("IsStationary")));
	chargeAttackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	chargeAttackRoot->AddChild(new AttackAnimationBehavior(m_pEntity, 1.5f, std::string("Attack x1.5"))); // Play the animation a little faster than usual, to represent being enraged
	chargeAttackRoot->AddChild(new WaitBehavior(0.4f, std::string("WaitAfterAttacking")));
	chargeAttackRoot->AddChild(new IdleAnimationBehavior(m_pEntity, 1.0f, std::string("IdleAnimAfterAttack")));
	chargeAttackRoot->AddChild(new InRangeBehavior(m_pAgent, std::string("InRange")));
	chargeAttackRoot->AddChild(new DealDamageBehavior(m_pEntity, new DamageInfo(TANK_MINION_DMG, m_pEntity, 1.2f, 1.0f, 3.5f), std::string("DealChargeDamage")));
	// ---------------------------------------------------------------------------------------------------------------------------
	lookAtParallel->AddChild(new HasTargetBehavior(m_pEntity, std::string("HasTarget")));
	lookAtParallel->AddChild(new TurnToTarget(m_pEntity, m_pAgent, std::string("TurnToTarget")));
	lookAtParallel->AddChild(new WaitBehavior(0.3f, std::string("WaitToLookAtTarget")));
	//	lookAtParallel->AddChild(new LookAtBehavior(m_pEntity, m_pAgent, std::string("LookAt")));
	// ---------------------------------------------------------------------------------------------------------------------------
	chargeParallel->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunnedWhileCharging")));
	chargeParallel->AddChild(chargeToTarget);
	//	chargeParallel->AddChild(new FindClosestTargetBehavior(m_pEntity, std::string("FindClosestTargetBeforeCharge")));
	chargeParallel->AddChild(new FailureToRunningBehavior(ranForTimeORInRange, std::string("FailureToRunning")));
	// ---------------------------------------------------------------------------------------------------------------------------
	turnToAfterChargeParallel->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	turnToAfterChargeParallel->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	turnToAfterChargeParallel->AddChild(new IdleAnimationBehavior(m_pEntity, 1.0f, std::string("IdleAnim")));
	//	turnToAfterChargeParallel->AddChild(new LookAtBehavior(m_pEntity, m_pAgent, std::string("LookAtTarget")));
	turnToAfterChargeParallel->AddChild(new TurnToTarget(m_pEntity, m_pAgent, std::string("TurnToTarget")));
	turnToAfterChargeParallel->AddChild(new WaitBehavior(0.4f, std::string("WaitToTurn")));
#endif

#pragma region OLD_BEHAVIOR
#if 0
	// High Level Roots
	ParallelBehavior *mainParallel = new ParallelBehavior(Policy::REQUIRE_ALL, Policy::REQUIRE_ALL, std::string("MainParallel"));
	SequenceBehavior *mainRoot     = new SequenceBehavior(std::string("MainRoot"));
	/// Move
	IsStunnedBehavior         *isStunned         = new IsStunnedBehavior(m_pEntity, std::string("PlayIdleIfStunned"));
	SequenceBehavior          *moveRoot          = new SequenceBehavior(std::string("MoveRoot"));
	SetDestinationBehavior    *setDestination    = new SetDestinationBehavior(m_pEntity, std::string("SetDestination"));
	RunAnimationBehavior      *runAnim           = new RunAnimationBehavior(m_pEntity, std::string("RunAnimation"));
	FindClosestTargetBehavior *findClosestTarget = new FindClosestTargetBehavior(m_pEntity, std::string("FindClosestTarget"));
	MoveToBehavior            *moveToDestination = new MoveToBehavior(m_pAgent, std::string("MoveToDestination"));
	/// Attack
	SequenceBehavior        *attackRoot    = new SequenceBehavior(std::string("AttackRoot"));
	InRangeBehavior         *targetInRange = new InRangeBehavior(m_pAgent, std::string("IsTargetInRange"));
	AttackAnimationBehavior *attackAnim    = new AttackAnimationBehavior(m_pEntity, std::string("PlayAttackAnimation"));
	WaitBehavior            *wait          = new WaitBehavior(0.8f, std::string("WaitForAttackingAnimationToComplete"));
	InRangeBehavior         *inRangeDamage = new InRangeBehavior(m_pAgent, std::string("CheckDistanceBeforeDealingDamage"));
	DealDamageBehavior      *dealDamage    = new DealDamageBehavior(m_pEntity, new DamageInfo(TANK_MINION_DMG, m_pEntity, 1.0f, 1.0f, 1.0f), std::string("DamageTheTarget"));
	/// Request Path
	RequestPathBehavior *requestPath = new RequestPathBehavior(m_pAgent, std::string("RequestNewPath"));
	TimerBehavior       *timer       = new TimerBehavior(m_fRequestTime, true, requestPath, std::string("WaitToRequestPath"));

	// ---------------------------------------------------------------------------------------------------------------------------
	m_pChild = mainParallel;
	mainParallel->AddChild(timer);
	mainParallel->AddChild(mainRoot);
	/// mainRoot
	mainRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("BreakFromTreeIfStationary")));
	mainRoot->AddChild(isStunned);
	mainRoot->AddChild(moveRoot);
	mainRoot->AddChild(attackRoot);
	/// MoveRoot
	moveRoot->AddChild(findClosestTarget);
	moveRoot->AddChild(setDestination);
	moveRoot->AddChild(runAnim);
	moveRoot->AddChild(moveToDestination);
	/// AttackRoot
	attackRoot->AddChild(targetInRange);
	attackRoot->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IdleBeforeAttackAnim")));
	attackRoot->AddChild(attackAnim);
	attackRoot->AddChild(wait);
	attackRoot->AddChild(inRangeDamage);
	attackRoot->AddChild(dealDamage);
	// ---------------------------------------------------------------------------------------------------------------------------
#endif
#pragma endregion
}

void BehaviorTree::CreateBossTree()
{
	// High Level Roots
	ParallelBehavior *mainParallel    = new ParallelBehavior(Policy::REQUIRE_ALL, Policy::REQUIRE_ONE, std::string("MainParallel"));
	SequenceBehavior *mainRoot        = new SequenceBehavior(std::string("MainRoot"));
	/// Move
	SequenceBehavior *moveRoot     = new SequenceBehavior(std::string("MoveRoot"));
	/// Attack
	SelectorBehavior *attackRoot  = new SelectorBehavior(std::string("AttackRoot"));
	SequenceBehavior *swingAttack = new SequenceBehavior(std::string("SwingAttack"));
	SequenceBehavior *stompAttack = new SequenceBehavior(std::string("StompAttack"));
	/// Request Path
	RequestPathBehavior *requestPath = new RequestPathBehavior(m_pAgent, std::string("RequestNewPath"));
	TimerBehavior       *timer       = new TimerBehavior(m_fRequestTime, true, requestPath, std::string("WaitToRequestPath"));

	// ---------------------------------------------------------------------------------------------------------------------------
	/// MoveRoot
	IsStunnedBehavior         *isStunned         = new IsStunnedBehavior(m_pEntity, std::string("CheckIfStunned"));
	SetDestinationBehavior    *setDestination    = new SetDestinationBehavior(m_pEntity, std::string("SetDestination"));
	RunAnimationBehavior      *runAnim           = new RunAnimationBehavior(m_pEntity, 1.0f, std::string("RunAnimation"));
	FindClosestTargetBehavior *findClosestTarget = new FindClosestTargetBehavior(m_pEntity, std::string("FindClosestTarget"));
	MoveToBehavior            *moveToDestination = new MoveToBehavior(m_pAgent, std::string("MoveToDestination"));
	// AttackRoot
	/// Is Stuck
	IsBossStuckBehavior     *isStuck = new IsBossStuckBehavior((BossMinion*)m_pEntity, 0.5f, 50.0f, std::string("CheckIfBossIsStuck"));
	/// SwingAttack
	InRangeBehavior         *targetInRange         = new InRangeBehavior(m_pAgent, std::string("IsTargetInRange"));
	SequenceBehavior        *swingSequence         = new SequenceBehavior(std::string("SwingParallel"));
	AttackAnimationBehavior *swingAttackAnim       = new AttackAnimationBehavior(m_pEntity, 1.0f, std::string("SwingAttackAnimation"));
	InRangeBehavior         *inRangeDamage         = new InRangeBehavior(m_pAgent, std::string("CheckDistanceBeforeDealingDamage"));
	DealDamageBehavior      *dealDamage            = new DealDamageBehavior(m_pEntity, new DamageInfo(BOSS_MINION_DMG, m_pEntity, 1.0f, 1.2f, 1.0f), std::string("DamageTheTarget"));
	TimerBehavior           *waitToDealSwingDamage = new TimerBehavior(1.0f, true, dealDamage, std::string("WaitToDealDamage"));
	/// StompAttack
	const int OVERCROWDED_THRESHOLD = 4;
	IsOverCrowdedBehavior             *isOvercrowded       = new IsOverCrowdedBehavior((BossMinion*)m_pEntity, OVERCROWDED_THRESHOLD, std::string("CheckIfOvercrowded"));
	CastSpellAnimationBehavior        *castSpellAnim       = new CastSpellAnimationBehavior(m_pEntity, 1.0f, std::string("CastSpellAnimation"));
	SequenceBehavior                  *stompSequence       = new SequenceBehavior(std::string("StompParallel"));
	JumpAnimationBehavior             *stompAttackAnim     = new JumpAnimationBehavior(m_pEntity, 1.0f, std::string("StompAttackAnimation"));
	SpawnBossShockwaveBehavior        *spawnShockwave      = new SpawnBossShockwaveBehavior(m_pEntity, std::string("SpawnTheBossShockwave"));
	TimerBehavior                     *spawnShockwaveTimer = new TimerBehavior(0.9f, true, spawnShockwave, std::string("SpawnShockwaveTimer"));
	DealDamageToNearbyEnemiesBehavior *damageNearbyEnemies = new DealDamageToNearbyEnemiesBehavior(m_pEntity, std::string("DealDamageToNearbyEnemies"));
	WaitBehavior                      *waitToMove          = new WaitBehavior(0.3f, std::string("PauseAfterStompAttack"));
	/// OR Condition
	ORBehavior *orCondition = new ORBehavior(isOvercrowded, isStuck, std::string("IsBossStuckOrOvercrowded"));
	// ----------------------------------------------------------------------------------------------------------------------------

	/// -----------------------------------
	m_pChild = mainParallel;
	mainParallel->AddChild(timer);
	mainParallel->AddChild(mainRoot);
	/// -----------------------------------
	mainRoot->AddChild(new IsStationaryBehavior(m_pEntity, std::string("BreakFromTreeIfStationary")));
	mainRoot->AddChild(isStunned);
	mainRoot->AddChild(moveRoot);
	mainRoot->AddChild(attackRoot);
	/// -----------------------------------
	moveRoot->AddChild(findClosestTarget);
	moveRoot->AddChild(setDestination);
	moveRoot->AddChild(runAnim);
	moveRoot->AddChild(moveToDestination);
	/// -----------------------------------
	attackRoot->AddChild(stompAttack);
	attackRoot->AddChild(swingAttack);
	/// -----------------------------------
	stompAttack->AddChild(orCondition);
	stompAttack->AddChild(new IsStunnedBehavior(m_pEntity, std::string("PlayIdleIfStunned")));
	stompAttack->AddChild(castSpellAnim);
	stompAttack->AddChild(new IsStunnedBehavior(m_pEntity, std::string("PlayIdleIfStunned")));
	stompAttack->AddChild(stompSequence);
	stompAttack->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IdleAfterCastAnim")));
	stompAttack->AddChild(waitToMove);
	/// -----------------------------------
	stompSequence->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	stompSequence->AddChild(stompAttackAnim);
	stompSequence->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	stompSequence->AddChild(spawnShockwaveTimer);
	stompSequence->AddChild(new IsStunnedBehavior(m_pEntity, std::string("IsStunned")));
	stompSequence->AddChild(damageNearbyEnemies);
	/// -----------------------------------
	swingAttack->AddChild(targetInRange);
	swingAttack->AddChild(swingSequence);
	/// -----------------------------------
	swingSequence->AddChild(swingAttackAnim);
	swingSequence->AddChild(waitToDealSwingDamage);
}

void BehaviorTree::CreateCutsceneTree()
{
	if (m_pAgent->GetMinionType() == MinionType::BOSS)
	{
		SequenceBehavior* root = new SequenceBehavior(std::string("Root"));
		/// Play attack once
		SequenceBehavior           *attackRoot          = new SequenceBehavior(std::string("AttackRoot"));
		DoOnceBehavior             *attackOnce          = new DoOnceBehavior(attackRoot, std::string("AttackOnce"));
		WaitBehavior               *wait                = new WaitBehavior(1.0f, std::string("WaitToCastSpell"));
		CastSpellAnimationBehavior *castSpellAnim       = new CastSpellAnimationBehavior(m_pEntity, 1.0f, std::string("CastSpellAnimation"));
		SequenceBehavior           *stompSequence       = new SequenceBehavior(std::string("StompParallel"));
		JumpAnimationBehavior      *stompAttackAnim     = new JumpAnimationBehavior(m_pEntity, 1.0f, std::string("StompAttackAnimation"));
		SpawnBossShockwaveBehavior *spawnShockwave      = new SpawnBossShockwaveBehavior(m_pEntity, std::string("SpawnTheBossShockwave"));
		TimerBehavior              *spawnShockwaveTimer = new TimerBehavior(0.9f, true, spawnShockwave, std::string("SpawnShockwaveTimer"));
		/// Idle
		IdleAnimationBehavior *idleAnim = new IdleAnimationBehavior(m_pEntity, 1.0f, std::string("IdleAnimation"));
		/// --------------------------------------------------------------------------------------------------
		m_pCutsceneChild = root;
		root->AddChild(attackOnce);
		root->AddChild(idleAnim);
		///
		attackRoot->AddChild(new IdleAnimationBehavior(m_pEntity, 1.0f, std::string("IdleOnCreation")));
		attackRoot->AddChild(wait);
		attackRoot->AddChild(castSpellAnim);
		attackRoot->AddChild(stompSequence);
		///
		stompSequence->AddChild(stompAttackAnim);
		stompSequence->AddChild(spawnShockwaveTimer);
	}
	else
	{
		IdleAnimationBehavior *idleAnim = new IdleAnimationBehavior(m_pEntity, 1.0f, std::string("IdleAnimation"));
		m_pCutsceneChild = idleAnim;
	}
}

void BehaviorTree::SwitchToCutsceneTree()
{
	m_bPlayCutsceneTree = true;
}

void BehaviorTree::SwitchToNormalTree()
{
	m_bPlayCutsceneTree = false;
}

