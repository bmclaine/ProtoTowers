///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Author: Doug Berg
//
// Description: Child class of Entity. Responsible for the MiniMinion actions.
//
///////////////////////////////////////////////////////////////////////////////////////////////////


// Engine Includes
#include "stdafx.h"
#include "MiniMinion.h"
#include "Assets\Animation.h"
#include "GameObject.h"
#include "Engine.h"
#include "ModuleInterface.h"
#include "InfernoTimer.h"
#include "Resource.h"
#include "Components\SkinnedMeshRenderer.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "EventSystem.h"
#include "MainBase.h"
#include "MinionSpawner.h"
#include "EntityTargetingWeights.h"

#define MELEE_MINION_HEAD_BONE_INDEX 5

MiniMinion::MiniMinion(GameObject* const _gameObject, Transform* const _transform) : AIEntity(_gameObject, _transform)
{
	SetAttackTimer(MINI_MINION_ATTACK_TIME);
	m_pEnemyToTrack = nullptr;
	m_fMaxHealth = MINI_MINION_HEALTH;
	m_fHealth = m_fMaxHealth;
	m_pAttackAnimationTimer->SetTime((unsigned int)600);
	m_pTakingDamageTimer->SetTime((unsigned int)300);
	m_pStunTimer->SetTime((unsigned int)600);
	m_pTakeDamageSoundTimer->SetTime((unsigned int)1000);

	m_pRenderer = nullptr;
	m_pIconRenderer = nullptr;
	m_pIconElement = nullptr;

	m_pIcon = new Texture();
	m_pIcon->SetImage(Inferno::GetImage(L"IconMushroom.dds"));

	SetAnimation(AnimationType::NO_ANIMATION);

	m_fMaxWeight = MAX_WEIGHT_MINI_MINION;
	m_fInfluence = INFLUENCE_MINI_MINION;

	InfernoEngine_AI::RegisterAIEntity(this);
}

MiniMinion::~MiniMinion()
{
	InfernoEngine_AI::UnregisterAIEntity(this);

	if (isEnabled())
		OnDisable();

	delete m_pIcon;
	delete m_pIconElement;
}

void MiniMinion::OnEnable()
{
	AIEntity::OnEnable();

	/// Animation stuff
	m_pAnimator = gameObject->GetComponentInChildren<Animator>();
	SetAnimation(AnimationType::IDLE);
	m_pAnimationController = m_pAnimator->GetAnimationController();

	m_pRenderer = gameObject->GetComponentInChildren<SkinnedMeshRenderer>();

	/// MiniMap Icon stuff
	if (m_pIconElement == nullptr)
	{
		m_pIconElement = new UIElement();
		m_pIconElement->SetName("MeleeMinionIcon");
		m_pIconRenderer = m_pIconElement->AddComponent<UIRenderer>();
		
		RectTransform* rectTransform = m_pIconElement->GetTransform();
		rectTransform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		rectTransform->SetBounds(Vector2(5.0f, 5.0f));

		m_pIconRenderer->SetTexture(m_pIcon);

		m_pIconRenderer->SetLayer(3);


		if (m_eTeam == Team::RED)
			m_pIconRenderer->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		else if (m_eTeam == Team::BLUE)
			m_pIconRenderer->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));

		m_pMapElement.element = m_pIconElement;
		m_pMapElement.transform = transform;
	}

	m_pIconRenderer->SetEnabled(true);
	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_pMapElement);

	IComponent::OnEnable();
}

void MiniMinion::OnDisable(void)
{
	if (m_pIconRenderer)
		m_pIconRenderer->SetEnabled(false);

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_pMapElement);

	IComponent::OnDisable();
}

IComponent& MiniMinion::operator=(IComponent& _rhs)
{
	MiniMinion* rhsMM = dynamic_cast<MiniMinion*>(&_rhs);
	if (rhsMM == nullptr)
	{
		return *this;
	}

	return operator=(*rhsMM);
}

MiniMinion& MiniMinion::operator=(const MiniMinion& _rhs)
{
	if (this != &_rhs)
	{
		AIEntity::operator=(_rhs);
	}

	return *this;
}

void MiniMinion::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	AIEntity::PostInit(objectMap, dataMap);
}

IComponent* MiniMinion::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	MiniMinion* temp = new MiniMinion(_gameObject, _transform);
	*temp = *this;

	return temp;
}

void MiniMinion::HandleParticles() const
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

void MiniMinion::FindClosestTarget()
{
	float closest = FLT_MAX;
	MainBase* tower = nullptr;

	AIEntity::FindClosestTarget();

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i]->isEnabled())
		{
			MinionSpawner* spawner = m_vNearbyEnemies[i]->gameObject->GetComponent<MinionSpawner>();
			if (spawner != nullptr)
				if (spawner->IsDeactivated())
					continue;

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

	/// Add weight to the tracked enemy
	AIEntity* aiEntity = m_pEnemyToTrack->gameObject->GetComponent<AIEntity>();
	if (aiEntity != nullptr)
	{
		if (aiEntity->GetCurrentWeight() < aiEntity->GetMaxWeight())
			aiEntity->AddInfluenceToWeight(m_fInfluence);
	}
}

void MiniMinion::Death()
{
	if (m_bIsDying)
		return;

	AIEntity::Death();

	/// If it's an enemy that just died, spawn a few resources at a slightly random location around this entity
	if (m_eTeam == RED)
	{
		GameObject* object = Inferno::Instantiate("Resource");
		object->GetComponent<Resource>()->ResetKillTime();
		object->transform->SetPosition(transform->GetPosition() + Vector3(0.0f, 0.5f, 0.0f));
	}

	m_pAnimator->SetSpeed(1.0f);
	SetAnimation(AnimationType::DYING);

	HandleParticles();

	Inferno::PostAudioEventAtObjectPosition(MINI_MINION_DEATH, gameObject);
	Inferno::Destroy(gameObject, 1.4f);
}

void MiniMinion::Update()
{
	AIEntity::Update();
}

void MiniMinion::TakeDamage(DamageInfo _damageInfo)
{
	if (m_bIsDying)
		return;

	m_fHealth -= _damageInfo.IncomingDamage * _damageInfo.MinionMultiplier;

	// If we don't have a target, aggro towards the thing that just damaged you
	if (m_pEnemyToTrack == nullptr)
	{
		m_pEnemyToTrack = _damageInfo.OwningObject;
		m_eState = State::AGGRO;
	}

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
			Inferno::PostAudioEventAtObjectPosition(WWiseIDs::MINI_MINION_TAKE_DAMAGE, gameObject);
		}
		if (m_pTakingDamageTimer->IsFinished() && m_pStunTimer->IsFinished() && !IsBuffed())
		{
			m_pStunTimer->Reset();
			m_pTakingDamageTimer->Reset();
			SetAnimation(AnimationType::TAKING_DAMAGE);
		}
	}
	HandleParticles();
}

void MiniMinion::OnTriggerEnter(Collider* _collider)
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
	m_pEnemyToTrack->AddForTracking(this, BindEntityOnReleaseFunction(&MiniMinion::StopTrackingEnemy, this));
	m_vNearbyEnemies.push_back(entity);
}

void MiniMinion::OnTriggerExit(Collider* _collider) /*override*/
{
	Entity* entity = gameObject->GetComponent<Entity>();

	unsigned int size = (unsigned int)m_vNearbyEnemies.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		if (m_vNearbyEnemies[i] == entity)
		{
			m_vNearbyEnemies.erase(m_vNearbyEnemies.begin() + i);
			break;
		}
	}
}

void MiniMinion::GetHeadBone(Matrix4& _matrix)
{
	Matrix4 boneWorldMatrix = m_pRenderer->GetBoneWorld(MELEE_MINION_HEAD_BONE_INDEX);
	_matrix = boneWorldMatrix;
}

AIEntityType MiniMinion::GetType() const
{
	return AIEntityType::TYPE_MINI;
}

