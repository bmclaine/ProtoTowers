#include "stdafx.h"
#include "BaseProjectile.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "InfernoTimer.h"
#include "Physics\Collider.h"
#include "Physics\SphereCollider.h"
#include "../Core/iDamagable.h"
#include "Entity.h"
#include "Engine.h"
#include "Player.h"
#include "BaseWeapon.h"
#include "AIEntity.h"
#include "AOEDamage.h"
#include "StunEffect.h"

#define KILL_TIME .50f
#define STUN_TIME 1.25f

BaseProjectile::BaseProjectile(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_bIsAlive = false;
	m_bIsFireable = true;
	m_fDamage = 20.0f;
	m_fDistTraveled = 0.0f;
	m_fKillDist = 10.0f;
	m_fVelocity = 30.0f;
	m_bAddEffect = false;
	m_iStunChance = 0;
	Inferno::RegisterObjectToAudioManager(gameObject);
}


BaseProjectile::~BaseProjectile()
{
}
IComponent& BaseProjectile::operator=(IComponent& _rhs)
{
	BaseProjectile* rhsBT = dynamic_cast<BaseProjectile*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

BaseProjectile& BaseProjectile::operator=(const BaseProjectile& _rhs)
{
	if (this != &_rhs) {
		m_bIsAlive = _rhs.m_bIsAlive;
		m_bIsFireable = _rhs.m_bIsFireable;
		m_fcFlags = _rhs.m_fcFlags;
		m_fDamage = _rhs.m_fDamage;
		m_fKillDist = _rhs.m_fKillDist;
		m_iStunChance = _rhs.m_iStunChance;
		m_pMoveDirection = _rhs.m_pMoveDirection;
		m_fDistTraveled = _rhs.m_fDistTraveled;
	}

	return *this;
}



IComponent* BaseProjectile::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	BaseProjectile* newComponent = new BaseProjectile(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void BaseProjectile::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Damage";
	Property* prop = data->GetProperty(propName);
	m_fDamage = (float)atof(prop->value.c_str());

	propName = "KillDistance";
	prop = data->GetProperty(propName);
	m_fKillDist = (float)atof(prop->value.c_str());

	propName = "Velocity";
	prop = data->GetProperty(propName);
	m_fVelocity = (float)atof(prop->value.c_str());
}

void BaseProjectile::Update()
{
	if (m_bIsAlive)
	{

		Kill();
		float _distTraveled = m_fVelocity * Time::DeltaTime;
		m_fDistTraveled += _distTraveled;
		transform->Translate(m_pMoveDirection * _distTraveled);
	}
	else
	{
		gameObject->SetActive(false);
	}
}

void BaseProjectile::OnTriggerEnter(Collider * _col)
{
	if (!_col->IsTrigger() && _col->gameObject->GetTag() != "Player")
	{
		if (m_bIsAlive)
		{
			m_bIsAlive = false;
			m_fDistTraveled = 0.0f;
			iDamagable * damagable = _col->gameObject->GetComponent<iDamagable>();
			Inferno::PostAudioEventAtObjectPosition(PLASMA_BLASTER_PROJECTILE_HIT, gameObject);
			if (damagable != nullptr)
			{
				DamageInfo damageInfo = DamageInfo(m_fDamage, m_pOwningEntity, 1.0f, 0.5f, 1.0f);
				damagable->TakeDamage(damageInfo);

				AIEntity* entity = _col->gameObject->GetComponent<AIEntity>();
				if (entity && entity->GetState() != State::STUNNED && entity->GetType() != AIEntityType::TYPE_BOSS && !entity->IsBuffed()){
					int chance = rand() % 100;
					if (chance < m_iStunChance){
						GameObject* stun = Inferno::Instantiate("StunEffect");
						stun->transform->SetParent(entity->transform);
						StunEffect* effect = stun->GetComponent<StunEffect>();
						effect->SetEffectTime(STUN_TIME);
					}
				}
				if (entity && entity->GetHealth() <= 0.0f){
					if (m_pOwningEntity){
						Player* player = static_cast<Player*>(m_pOwningEntity);
						if (player){
							BaseWeapon* weapon = player->GetWeapon();
							if (weapon){
								weapon->SetKillCount(weapon->GetKillCount() + 1);
							}
						}
					}
				}
			}

			CreateImpactParticles();
		}

		if (m_bAddEffect){
			GameObject* aoeDamageObj = Inferno::Instantiate("AOEDamage");
			aoeDamageObj->transform->SetPosition(transform->GetPosition());
			AOEDamage* aoeDamage = aoeDamageObj->GetComponent<AOEDamage>();
			aoeDamage->Cast();
		}
	}
}

void BaseProjectile::CreateImpactParticles(){
	// Impact partilces
	GameObject* particleObject = Inferno::GetNewGameObject();
	particleObject->transform->SetPosition(transform->GetPosition());
	ParticleSystem* impact = particleObject->AddComponent<ParticleSystem>();
	impact->SetShape(EMITTER_SHAPE_SPHERE);
	impact->SetStyle(EMITTER_STYLE_INNER);
	impact->SetLooping(false);
	impact->SetSphereDimensions(1.0f);
	impact->SetEmissionRate(300);
	impact->SetDuration(0.1f);
	impact->SetLifetime(0.2f);
	impact->SetStartSpeed(5.0f);
	impact->SetStartSize(0.1f);
	impact->SetEndSize(0.05f);
	impact->SetStartColor(Color(0, 1, 0, 1));// Green
	impact->SetEndColor(Color(1, 1, 0, 1));  // Yellow
	impact->Play();
	Inferno::Destroy(particleObject, impact->GetDuration());
}


void BaseProjectile::Kill()
{
	if (m_fDistTraveled >= m_fKillDist)
	{
		m_bIsAlive = false;
		gameObject->SetActive(false);
	}
}

void BaseProjectile::FireProjectile(const Vector3& _position, const Vector3& _direction)
{
	if (m_bIsFireable)
	{
		this->gameObject->SetActive(true);
		Inferno::PostAudioEventAtObjectPosition(PLASMA_BLASTER_ATTACK, gameObject);
		m_fDistTraveled = 0.0f;
		m_bIsAlive = true;
		m_bIsFireable = false;
		m_pMoveDirection = _direction;
		transform->SetPosition(_position);
	}
}

void BaseProjectile::MakeFireable()
{
	m_bIsFireable = true;
}

//DESCRIPTION: Sets the owning entity of this projectile
//
//IN: Entity* - The entity that shot this projectile
//
//OUT: void
void BaseProjectile::SetOwningEntity(Entity* _entity)
{
	m_pOwningEntity = _entity;
}

void BaseProjectile::SetDamage(float damage){
	m_fDamage = damage;
}

void BaseProjectile::SetStunChance(int chance){
	m_iStunChance = chance;
}

//DESCRIPTION: Returns the owning entity of this projectile
//
//IN: void
//
//OUT: Entity* - The owning entity of the projectile
Entity* BaseProjectile::GetOwningEntity() const
{
	return m_pOwningEntity;
}

void BaseProjectile::SetAddEffect(bool value){
	m_bAddEffect = value;
}

bool BaseProjectile::GetAddEffect() const{
	return m_bAddEffect;
}

