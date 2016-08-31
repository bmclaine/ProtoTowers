///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: The tank minion inherits from AIEntity.
//              It will target anything close to him, but the player will be its highest priority.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// STL Includes


// Engine Includes
#include "stdafx.h"
#include "TankMinion.h"
#include "ModuleInterface.h"
#include "GameObject.h"
#include "Resource.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "Assets\Texture.h"
#include "EventSystem.h"
#include "MainBase.h"
#include "MinionSpawner.h"
#include "EntityTargetingWeights.h"
#include "Physics\SphereCollider.h"
#include "Physics\CapsuleCollider.h"

#define TANK_MINION_HEAD_BONE_INDEX 12

TankMinion::TankMinion(GameObject* const _gameObject, Transform* const _transform) : AIEntity(_gameObject, _transform)
{
	SetAttackTimer(TANK_MINION_ATTACK_TIME);
	m_pEnemyToTrack = nullptr;
	m_fMaxHealth = TANK_MINION_HEALTH;
	m_fHealth = m_fMaxHealth;
	m_pAttackAnimationTimer->SetTime((unsigned int)600);
	m_pTakingDamageTimer->SetTime((unsigned int)300);
	m_pStunTimer->SetTime((unsigned int)2000);
	m_pTakeDamageSoundTimer->SetTime((unsigned int)1000);

	m_pIconRenderer = nullptr;
	m_pIconElement = nullptr;

	m_pIcon = new Texture();
	m_pIcon->SetImage(Inferno::GetImage(L"IconGolum.dds"));

	SetAnimation(AnimationType::NO_ANIMATION);

	m_fMaxWeight = MAX_WEIGHT_TANK_MINION;
	m_fInfluence = INFLUENCE_TANK_MINION;

	InfernoEngine_AI::RegisterAIEntity(this);
}

TankMinion::~TankMinion()
{
	InfernoEngine_AI::UnregisterAIEntity(this);

	if (isEnabled())
		OnDisable();

	delete m_pIcon;
	delete m_pIconElement;
}

void TankMinion::OnEnable()
{
	AIEntity::OnEnable();

	m_pAnimator = gameObject->GetComponentInChildren<Animator>();
	SetAnimation(AnimationType::RUNNING);
	m_pAnimationController = m_pAnimator->GetAnimationController();
	m_pRenderer = gameObject->GetComponentInChildren<SkinnedMeshRenderer>();

	/// MiniMap Icon stuff
	if (m_pIconElement == nullptr)
	{
		m_pIconElement = new UIElement();
		m_pIconElement->SetName("TankMinionIcon");
		m_pIconRenderer = m_pIconElement->AddComponent<UIRenderer>();

		RectTransform* rectTransform = m_pIconElement->GetTransform();
		rectTransform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		rectTransform->SetBounds(Vector2(7.5f, 7.5f));

		m_pIconRenderer->SetTexture(m_pIcon);

		m_pIconRenderer->SetLayer(3);

		if (m_eTeam == Team::RED)
			m_pIconRenderer->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		else if (m_eTeam == Team::BLUE)
			m_pIconRenderer->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));

		m_pMapElement.element = m_pIconElement;
		m_pMapElement.transform = transform;
	}

	gameObject->GetComponent<SphereCollider>()->SetRadius(10.0f);
	gameObject->GetComponent<CapsuleCollider>()->SetHeight(6.0f);
	gameObject->GetComponent<CapsuleCollider>()->SetCenter(Vector3(0.0f, 4.0f, 0.0f));

	m_pIconRenderer->SetEnabled(true);
	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_pMapElement);

	IComponent::OnEnable();
}

void TankMinion::OnDisable()
{
	if (m_pIconRenderer)
		m_pIconRenderer->SetEnabled(false);

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_pMapElement);
}

void TankMinion::Death()
{
	if (m_bIsDying)
		return;

	AIEntity::Death();

	/// If it's an enemy that just died, spawn a few resources at a slightly random location around this entity
	if (m_eTeam == RED)
	{
		for (unsigned int i = 0; i < 5; ++i)
		{
			GameObject* object = Inferno::Instantiate("Resource");
			object->GetComponent<Resource>()->ResetKillTime();
			int randX = rand() % 2 - 1;
			int randZ = rand() % 2 - 1;
			object->transform->SetPosition(transform->GetPosition() + Vector3((float)randX, 0.5f, (float)randZ));
		}
	}

	m_pAnimator->SetSpeed(1.0f);
	SetAnimation(AnimationType::DYING);

	HandleParticles();

	Inferno::PostAudioEventAtObjectPosition(MINI_MINION_DEATH, gameObject);
	Inferno::Destroy(gameObject, 1.6f);
}

IComponent& TankMinion::operator = (IComponent& _assign)
{
	TankMinion* tank = dynamic_cast<TankMinion*>(&_assign);
	if (tank == nullptr)
	{
		return *this;
	}

	return operator=(*tank);
}

TankMinion& TankMinion::operator = (const TankMinion& _assign)
{
	if (this != &_assign)
	{
		AIEntity::operator=(_assign);
	}

	return *this;
}

IComponent* TankMinion::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	TankMinion* temp = new TankMinion(_gameObject, _transform);
	*temp = *this;

	return temp;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void TankMinion::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	AIEntity::PostInit(objectMap, dataMap);
}

// Update - Nothing special
void TankMinion::Update()
{
	AIEntity::Update();
}

void TankMinion::TakeDamage(DamageInfo _damageInfo)
{
	if (m_bIsDying)
		return;

	m_fHealth -= _damageInfo.IncomingDamage  * _damageInfo.MinionMultiplier;

	/// The minion is dead
	if (m_fHealth <= 0)
	{
		Death();
	}
	/// The minion is still alive
	else
	{
		if (m_pTakeDamageSoundTimer->IsFinished())
		{
			m_pTakeDamageSoundTimer->Reset();
			Inferno::PostAudioEventAtObjectPosition(WWiseIDs::TANK_TAKE_DAMAGE, gameObject);
		}
		if (m_pTakingDamageTimer->IsFinished() && m_pStunTimer->IsFinished())
		{
//			m_pStunTimer->Reset();
			m_pTakingDamageTimer->Reset();
//			SetAnimation(AnimationType::TAKING_DAMAGE);
		}
	}
	HandleParticles();
}


// IN: _collider - The collider that triggered you
//
// OUT: VOID
//
// Description: When a collider triggers you, this will be called
void TankMinion::OnTriggerEnter(Collider* _collider)
{
	Entity* entity = _collider->gameObject->GetComponent<Entity>();
	if (entity == nullptr)
		return;
	if (entity->GetTeam() == NO_TEAM || entity->GetTeam() == m_eTeam)
		return;
	if (entity->isEnabled() == false)
		return;

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i] == entity)
			return;
	}

	m_pEnemyToTrack = entity;
	m_pEnemyToTrack->AddForTracking(this, BindEntityOnReleaseFunction(&TankMinion::StopTrackingEnemy, this));
	m_vNearbyEnemies.push_back(entity);
}

// IN: _collider - The collider that left your trigger
//
// OUT: VOID
//
// Description: When a collider leaves your trigger, this function is called
void TankMinion::OnTriggerExit(Collider* _collider)
{
	Entity* entity = gameObject->GetComponent<Entity>();

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i] == entity)
		{
			std::iter_swap(m_vNearbyEnemies.begin() + i, m_vNearbyEnemies.end() - 1);
			m_vNearbyEnemies.erase(m_vNearbyEnemies.end() - 1);
			break;
		}
	}
}

void TankMinion::FindClosestTarget()
{
	float closest = FLT_MAX;
	MainBase* tower = nullptr;

	AIEntity::FindClosestTarget();

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i]->isEnabled())
		{
			if (m_vNearbyEnemies[i]->gameObject->GetComponent<BaseTower>() != nullptr)
				continue;

			float temp = (m_vNearbyEnemies[i]->transform->GetPosition() - transform->GetPosition()).LengthSq();
			bool isPlayer = m_vNearbyEnemies[i]->gameObject->GetName() == "Player";
			if (temp < closest || isPlayer)
			{
				closest = temp;
				m_pEnemyToTrack = m_vNearbyEnemies[i];
				if (isPlayer)
					return;
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

	/// Add weight to the tracked enemy
	AIEntity* aiEntity = m_pEnemyToTrack->gameObject->GetComponent<AIEntity>();
	if (aiEntity != nullptr)
	{
		if (aiEntity->GetCurrentWeight() < aiEntity->GetMaxWeight())
			aiEntity->AddInfluenceToWeight(m_fInfluence);
	}
}

void TankMinion::HandleParticles() const
{
	Color particleColor = (m_eTeam == Team::RED) ? Color(1, 0, 0, 1) : Color(0, 0, 1, 1);

	if (m_bIsTakingDamage)
	{
#pragma region MAYBE_NOT_USING
#if 0
		GameObject* object = Inferno::GetNewGameObject();
		object->SetName("HitMarker");
		object->transform->SetPosition(gameObject->transform->GetPosition() + Vector3(0, 1.0f, 0));
		ParticleSystem* particle = object->AddComponent<ParticleSystem>();
		particle->SetShape(EMITTER_SHAPE_SPHERE);
		particle->SetStyle(EMITTER_STYLE_INNER);
		particle->SetSphereDimensions(3.0f);
		particle->SetEmissionRate(150);
		particle->SetLifetime(0.2f);
		particle->SetDuration(0.1f);
		particle->SetStartSpeed(3.0f);
		particle->SetStartSize(0.1f);
		particle->SetEndSize(0.1f);
		particle->SetStartColor(particleColor);
		particle->SetEndColor(particleColor);
		particle->Play();
		Inferno::Destroy(object, 0.2f);
#endif
#pragma endregion
	}
	else if (m_bIsDying)
	{
		GameObject* object = Inferno::GetNewGameObject();
		object->SetName("DyingParticles");
		object->transform->SetPosition(gameObject->transform->GetPosition());
		ParticleSystem* particle = object->AddComponent<ParticleSystem>();
		particle->SetShape(EMITTER_SHAPE_CONE);
		particle->SetStyle(EMITTER_STYLE_EDGE);
		particle->SetConeDimensions(0.5f, 1.0f, 2.0f);
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

// IN: VOID
//
// OUT: Matrix4 - The matrix transform of this minions head bone
//
// Description: Returns the transform of this minions head bone.
void TankMinion::GetHeadBone(Matrix4& _matrix)
{
	Matrix4 boneWorldMatrix = m_pRenderer->GetBoneWorld(TANK_MINION_HEAD_BONE_INDEX);
	_matrix = boneWorldMatrix;
}

AIEntityType TankMinion::GetType() const
{
	return AIEntityType::TYPE_TANK;
}

