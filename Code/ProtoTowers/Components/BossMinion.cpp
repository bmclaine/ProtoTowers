///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: The boss minion that inherits from AIEntity. Will be the hardest AIAgent to kill
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// Engine Includes
#include "stdafx.h"
#include "BossMinion.h"
#include "ModuleInterface.h"
#include "MathLib.h"
#include "GameObject.h"
#include "Resource.h"
#include "MainBase.h"
#include "ParticleManager.h"
#include "MinionBuffWave.h"
#include "BossShockwave.h"
#include "MinionSpawner.h"
#include "EntityTargetingWeights.h"

#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "EventSystem.h"

#define BOSS_MINION_HEAD_BONE_INDEX 6

BossMinion::BossMinion(GameObject* const _gameObject, Transform* const _transform) : AIEntity(_gameObject, _transform)
{
	SetAttackTimer(BOSS_MINION_ATTACK_TIME);
	m_pEnemyToTrack = nullptr;
	m_fMaxHealth = BOSS_MINION_HEALTH;
	m_fHealth = m_fMaxHealth;
	m_pAttackAnimationTimer->SetTime((unsigned int)600);
	m_pTakingDamageTimer->SetTime((unsigned int)300);
	m_pStunTimer->SetTime((unsigned int)1000);
	m_pTakeDamageSoundTimer->SetTime((unsigned int)1500);

	m_pRenderer = nullptr;
	m_pIconElement = nullptr;
	m_pIconRenderer = nullptr;

	m_pIcon = new Texture();
	m_pIcon->SetImage(Inferno::GetImage(L"IconBoss.dds"));

	SetAnimation(AnimationType::NO_ANIMATION);

	m_fMaxWeight = MAX_WEIGHT_BOSS_MINION;
	m_fInfluence = INFLUENCE_BOSS_MINION;

	m_vecPrevPosition = Vector3(0.0f, 0.0f, 0.0f);

	InfernoEngine_AI::RegisterAIEntity(this);
}

BossMinion::~BossMinion()
{
	if (isEnabled()) {
		OnDisable();
	}

	delete m_pIcon;
	delete m_pIconElement;

	InfernoEngine_AI::UnregisterAIEntity(this);
}

void BossMinion::OnEnable()
{
	AIEntity::OnEnable();

	m_pAnimator = gameObject->GetComponentInChildren<Animator>();
	SetAnimation(AnimationType::RUNNING);

	m_pAnimationController = m_pAnimator->GetAnimationController();
	m_pRenderer = gameObject->GetComponentInChildren<SkinnedMeshRenderer>();

	/// MiniMap Icon
	if (m_pIconElement == nullptr)
	{
		m_pIconElement = new UIElement();
		m_pIconElement->SetName("BossMinionIcon");
		m_pIconRenderer = m_pIconElement->AddComponent<UIRenderer>();

		RectTransform* rectTransform = m_pIconElement->GetTransform();
		rectTransform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		rectTransform->SetBounds(Vector2(10.0f, 10.0f));

		m_pIconRenderer->SetTexture(m_pIcon);

		m_pIconRenderer->SetLayer(4);

		m_pMapElement.element = m_pIconElement;
		m_pMapElement.transform = transform;
	}

	m_pIconRenderer->SetEnabled(true);
	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_pMapElement);

	m_vecPrevPosition = transform->GetPosition();

	IComponent::OnEnable();
}

void BossMinion::OnDisable(void)
{
	if (m_pIconRenderer)
		m_pIconRenderer->SetEnabled(false);

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_pMapElement);

	IComponent::OnDisable();
}

void BossMinion::Death()
{
	if (m_bIsDying)
		return;

	AIEntity::Death();

	if (m_eTeam == RED)
	{
		for (unsigned int i = 0; i < 10; ++i)
		{
			GameObject* object = Inferno::Instantiate("Resource");
			object->GetComponent<Resource>()->ResetKillTime();
			float randX = (float)(rand() % 2 - 1);
			float randZ = (float)(rand() % 2 - 1);
			object->transform->SetPosition(transform->GetPosition() + Vector3(randX, 2.0f, randZ));
		}
	}

	m_pAnimator->SetSpeed(1.0f);
	SetAnimation(AnimationType::DYING);

	HandleParticles();

	Inferno::PostAudioEventAtObjectPosition(MEGA_TANK_DEATH, gameObject);
	Inferno::Destroy(gameObject, 1.6f);
}

IComponent& BossMinion::operator = (IComponent& _assign)
{
	BossMinion* boss = dynamic_cast<BossMinion*>(&_assign);
	if (boss == nullptr)
		return *this;

	return operator=(*boss);
}

BossMinion& BossMinion::operator= (const BossMinion& _assign)
{
	if (this != &_assign)
		AIEntity::operator = (_assign);

	return *this;
}

IComponent* BossMinion::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	BossMinion* temp = new BossMinion(_gameObject, _transform);
	*temp = *this;

	return temp;
}

void BossMinion::HandleParticles() const
{
	Color particleColor = (m_eTeam == Team::RED) ? Color(1, 0, 0, 1) : Color(0, 0, 1, 1);

	if (m_bIsDying)
	{
		GameObject* object = Inferno::GetNewGameObject();
		object->SetName("DyingParticles");
		object->transform->SetPosition(gameObject->transform->GetPosition());
		ParticleSystem* particle = object->AddComponent<ParticleSystem>();
		particle->SetShape(EMITTER_SHAPE_CONE);
		particle->SetStyle(EMITTER_STYLE_EDGE);
		particle->SetHemisphereDimensions(0.1f);
		particle->SetEmissionRate(150);
		particle->SetDuration(0.75f);
		particle->SetLifetime(0.5f);
		particle->SetStartSpeed(3.0f);
		particle->SetStartSize(0.3f);
		particle->SetEndSize(0.1f);
		particle->SetStartColor(particleColor);
		particle->SetEndColor(Color(1, 1, 1, 1));
		particle->Play();
		Inferno::Destroy(object, 1.0f);
	}
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
/*virtual*/ void BossMinion::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	AIEntity::PostInit(objectMap, dataMap);
}

void BossMinion::Update()
{
	AIEntity::Update();
}

// IN: _collider - The collider that triggered you
//
// OUT: VOID
//
// Description: When a collider triggers you, this will be called
void BossMinion::OnTriggerEnter(Collider* _collider)
{
	Entity* entity = _collider->gameObject->GetComponent<Entity>();
	if (entity == nullptr)
		return;
	if (entity->GetTeam() == NO_TEAM || entity->GetTeam() == m_eTeam)
		return;
	if (entity->isEnabled() == false)
		return;

	/// If the entity is somehow already being tracked, break out
	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i] == entity)
			return;
	}

	m_pEnemyToTrack = entity;
	m_pEnemyToTrack->AddForTracking(this, BindEntityOnReleaseFunction(&BossMinion::StopTrackingEnemy, this));
	m_vNearbyEnemies.push_back(entity);
}

// IN: _collider - The collider that left your trigger
//
// OUT: VOID
//
// Description: When a collider leaves your trigger, this function is called
void BossMinion::OnTriggerExit(Collider* _collider)
{
	Entity* entity = _collider->gameObject->GetComponent<Entity>();
	if (entity == nullptr)
		return;
	if (entity->gameObject->GetTag() == "Player")
		return;
	if (entity->gameObject->GetComponent<AIEntity>() != nullptr)
		return;

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i] == entity)
		{
			std::iter_swap(m_vNearbyEnemies.begin() + i, m_vNearbyEnemies.end() - 1);
			m_vNearbyEnemies.erase(m_vNearbyEnemies.end() - 1);
			return;
		}
	}
}

void BossMinion::TakeDamage(DamageInfo _damageInfo)
{
	if (m_bIsDying)
		return;

	m_fHealth -= _damageInfo.IncomingDamage * _damageInfo.MinionMultiplier;

	if (m_OnTakeDamage)
		m_OnTakeDamage(m_fHealth / m_fMaxHealth);

	// The minion is dead
	if (m_fHealth <= 0)
	{
		Death();
	}
	else // The minion is still alive
	{
		if (m_pTakeDamageSoundTimer->IsFinished())
		{
			m_pTakeDamageSoundTimer->Reset();
			Inferno::PostAudioEventAtObjectPosition(WWiseIDs::MEGA_TANK_TAKE_DAMAGE, gameObject);
		}
		//if (m_pTakingDamageTimer->IsFinished() && m_pStunTimer->IsFinished())
		//{
		//	m_pStunTimer->Reset();
		//	m_pTakingDamageTimer->Reset();
		//}
	}

	HandleParticles();
}

void BossMinion::FindClosestTarget()
{
	float closest = FLT_MAX;
	MainBase* tower = nullptr;
	m_pEnemyToTrack = nullptr;

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		MinionSpawner* spawner = m_vNearbyEnemies[i]->gameObject->GetComponent<MinionSpawner>();
		if (spawner != nullptr)
			if (spawner->IsDeactivated())
				continue;

		if (m_vNearbyEnemies[i]->gameObject->GetComponent<Player>() != nullptr)
			continue;
		if (m_vNearbyEnemies[i]->gameObject->GetComponent<AIEntity>() != nullptr)
			continue;
		if (m_vNearbyEnemies[i]->isEnabled())
		{
			float temp = (m_vNearbyEnemies[i]->transform->GetPosition() - transform->GetPosition()).MagnitudeSq();
			if (temp < closest)
			{
				closest = temp;
				m_pEnemyToTrack = m_vNearbyEnemies[i];
			}
		}
	}

	/// If we didn't find a target at all
	if (m_pEnemyToTrack == nullptr)
		return;

	/// If the closest target is a MainBase, if it has guard towers, target the closest guard tower.
	tower = m_pEnemyToTrack->gameObject->GetComponent<MainBase>();
	if (tower != nullptr)
	{
		if (tower->GetType() == TowerTypes::TT_MAINBASE)
		{
			Entity* closestGuardTower = tower->GetClosestGuardTower(transform->GetPosition());
			if (closestGuardTower != nullptr)
				m_pEnemyToTrack = closestGuardTower;
		}
	}
}

// IN: VOID
//
// OUT: Matrix4 - The matrix transform of this minions head bone
//
// Description: Returns the transform of this minions head bone.
void BossMinion::GetHeadBone(Matrix4& _matrix)
{
	Matrix4 boneWorldMatrix = m_pRenderer->GetBoneWorld(BOSS_MINION_HEAD_BONE_INDEX);
	_matrix =  boneWorldMatrix;
}

void BossMinion::SetOnTakeDamageFunction(OnTakeDamageFunc _func)
{
	m_OnTakeDamage = _func;
}

unsigned int BossMinion::GetNumEnemiesWithinRange() const
{
	unsigned int count = 0;
	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i]->gameObject->GetComponent<AIEntity>() != nullptr)
		{
			float distance = (m_vNearbyEnemies[i]->transform->GetPosition() - transform->GetPosition()).LengthSq();
			if (distance < BOSS_MINION_ATTACK_DIST)
				++count;
		}
	}
	return count;
}

void BossMinion::DamageNearbyEnemies()
{
	for (unsigned int i = 0; i < (unsigned int)m_vNearbyEnemies.size(); ++i)
	{
		Collider* collider = m_vNearbyEnemies[i]->gameObject->GetComponent<Collider>();
		float radiusOfBothColliders = gameObject->GetComponent<NavAgent>()->GetAgentRadius() + collider->GetCenter().x;
		radiusOfBothColliders *= 2.0f; /// Scaling to match the LengthSq() call being made below
		float distanceBetweenCenterPoints = (collider->GetCenter() - transform->GetPosition()).LengthSq();
		float leeway = 400.0f;

		if (distanceBetweenCenterPoints - leeway < radiusOfBothColliders)
		{
			DamageInfo info(BOSS_MINION_DMG, this, 1.0f, 20.0f, 20.0f);
			m_vNearbyEnemies[i]->TakeDamage(info);
		}
	}
}

AIEntityType BossMinion::GetType() const
{
	return AIEntityType::TYPE_BOSS;
}

float BossMinion::GetDistanceTraveled()
{
	// Get the distance since this was last called
	float distance = (m_vecPrevPosition - transform->GetPosition()).LengthSq();
	// Set the previous position to the current position
	m_vecPrevPosition = transform->GetPosition();
	
	return distance;
}

