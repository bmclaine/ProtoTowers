#include "stdafx.h"
#include "FreezeTower.h"

#include "FreezeEffect.h"
#include "FreezeBlastWave.h"

// === Defines
#define ICEEFFECT_START_SCALE 0.5f
#define ICEEFFECT_END_SCALE 5.75f
#define ICEEFFECT_GROWTH_TIME 1.0f
#define ICEEFFECT_FADEOUT_TIME 1.5f
#define FREEZEWAVE_TIME 8.0f
#define LIGHT_INTENSITY_MIN 1.0f
#define LIGHT_INTENSITY_MAX 2.0f
#define LIGHT_INTENSITY_TIME 1.0f

// ===== Constructor / Destructor ===== //
FreezeTower::FreezeTower(GameObject* _gameObject, Transform* _transform) : BaseTurret(_gameObject, _transform)
{
	m_pIceGroundEffect = nullptr;
	m_pParticleEffects = nullptr;
	m_eCurrentState = FreezeTowerStates::FT_Default_State;
	m_pStateTimer = new InfernoTimer();
	m_pBlastWaveTimer = nullptr;
	m_pLight = nullptr;
}

FreezeTower::~FreezeTower()
{
	delete m_pStateTimer;

	if (m_pBlastWaveTimer != nullptr) {
		delete m_pBlastWaveTimer;
	}
}
// ==================================== //

// ===== Interface ===== //
void FreezeTower::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	BaseTurret::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	// === Get the Ice Ground Sprite Renderer
	std::string propName = "IceGroundEffect";
	Property* prop = data->GetProperty(propName);
	int ID = atoi(prop->value.c_str());
	if (ID > 0) {
		m_pIceGroundEffect = (SpriteRenderer*)objectMap.find((unsigned int)ID)->second;
	}
}

void FreezeTower::OnEnable()
{
	if (m_pParticleEffects == nullptr) {
		// === Get the Particle Effects Object
		m_pParticleEffects = transform->GetChildByName(std::string("FT Particle Effects"));
	}

	if (m_pParticleEffects != nullptr) {
		// === Disable the Particle Effects Object until it's needed
		m_pParticleEffects->SetActive(false, true);
	}

	GameObject* foundObject;
	if (m_pIceGroundEffect == nullptr) {
		foundObject = transform->GetChildByName(std::string("IceGroundEffect"));
		if (foundObject != nullptr) {
			m_pIceGroundEffect = foundObject->GetComponent<SpriteRenderer>();
		}
	}

	if (m_pIceGroundEffect != nullptr) {
		m_pIceGroundEffect->gameObject->SetActive(false, true);
	}

	if (m_pLight == nullptr) {
		foundObject = transform->GetChildByName(std::string("FT Light"));
		if (foundObject != nullptr) {
			m_pLight = foundObject->GetComponent<Light>();
		}
	}

	if (m_pLight != nullptr) {
		m_pLight->SetIntensity(LIGHT_INTENSITY_MIN);
		m_bLightIntensityIncrease = true;
	}

	if (m_uiUpgradeLevel == 2 && m_pBlastWaveTimer == nullptr) {
		m_pBlastWaveTimer = new InfernoTimer();
		m_pBlastWaveTimer->SetTime(FREEZEWAVE_TIME);
	}

	m_bIceEffectActive = false;

	BaseTower::OnEnable();
}

void FreezeTower::OnDisable()
{
	unsigned int count = m_FrozenEnemies.size();
	for (unsigned int i = 0; i < count; ++i) {
		m_FrozenEnemies[i].Effect->OnRemoveEffect();
	}
	m_FrozenEnemies.clear();

	BaseTurret::OnDisable();
}

void FreezeTower::Update()
{
	m_pGunTransform->Rotate(Vector3(0.0f, m_fTurnRate * Time::DeltaTime, 0.0f));

	UpdateLightIntensity();

	if (m_eCurrentState == FreezeTowerStates::FT_EffectFadeOut_State) {
		UpdateEffectFadeOut();
	}
	else if (m_eCurrentState == FreezeTowerStates::FT_EffectGrowth_State) {
		UpdateEffectGrowth();
	}

	if (m_bNoUpdate) {
		return;
	}

	if (m_uiUpgradeLevel == 2) {
		FireFreezeWave();
	}
}
// ===================== //

// === Protected Interface === //
void FreezeTower::OnEntityRelease(Entity* _releasedEntity)
{
	unsigned int index = 0;
	if (ContainsEnemy(_releasedEntity->gameObject, &index)) {
		m_FrozenEnemies[index].Effect->OnRemoveEffect();
		m_FrozenEnemies.removeAt(index);
		
		if (m_FrozenEnemies.size() == 0) {
			if (m_pParticleEffects != nullptr) {
				m_pParticleEffects->SetActive(false);
			}

			// === Ground Effect
			m_bIceEffectActive = false;
			m_eCurrentState = FreezeTowerStates::FT_EffectFadeOut_State;

			m_pStateTimer->SetTime(ICEEFFECT_FADEOUT_TIME);
			m_pStateTimer->Reset();
			m_pStateTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&FreezeTower::OnEffectFadeOutDone, this));
		}
	}
}
// =========================== //

// === Private Interface === //
IComponent& FreezeTower::operator=(IComponent& _rhs)
{
	FreezeTower* rhsFT = dynamic_cast<FreezeTower*>(&_rhs);
	if (rhsFT == nullptr) {
		return *this;
	}

	return operator=(*rhsFT);
}

FreezeTower& FreezeTower::operator=(const FreezeTower& _rhs)
{
	if (this != &_rhs) {
		m_eCurrentState = _rhs.m_eCurrentState;

		BaseTurret::operator=(_rhs);
	}

	return *this;
}

IComponent* FreezeTower::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	FreezeTower* newFreezeTower = new FreezeTower(_gameObject, _transform);
	*newFreezeTower = *this;

	return newFreezeTower;
}

bool FreezeTower::ContainsEnemy(GameObject* _gameObject, unsigned int* _foundIndex)
{
	unsigned int count = m_FrozenEnemies.size();
	for (unsigned int i = 0; i < count; ++i) {
		if (m_FrozenEnemies[i].Object == _gameObject) {
			if (_foundIndex != nullptr) {
				*_foundIndex = i;
			}
			return true;
		}
	}

	return false;
}

void FreezeTower::UpdateEffectGrowth()
{
	float ratio = (m_pStateTimer->GetSetTimeSeconds() - m_pStateTimer->TimeRemaining()) / m_pStateTimer->GetSetTimeSeconds();
	float newScale = (ICEEFFECT_END_SCALE - ICEEFFECT_START_SCALE) * ratio;

	m_pIceGroundEffect->transform->SetScale(newScale, newScale, newScale);
}

void FreezeTower::UpdateEffectFadeOut()
{
	float ratio = m_pStateTimer->TimeRemaining() / m_pStateTimer->GetSetTimeSeconds();
	float newAlpha = 1.0f * ratio;

	m_pIceGroundEffect->SetColor(Color(1.0f, 1.0f, 1.0f, newAlpha));
}

void FreezeTower::UpdateLightIntensity()
{
	if (m_pLight == nullptr) {
		return;
	}

	float newIntensity;
	if (m_bLightIntensityIncrease) {
		// === Increase
		newIntensity = m_pLight->GetIntensity() + ((LIGHT_INTENSITY_MAX - LIGHT_INTENSITY_MIN) * (LIGHT_INTENSITY_TIME * Time::DeltaTime));
		m_pLight->SetIntensity(newIntensity);

		if (newIntensity > LIGHT_INTENSITY_MAX) {
			m_pLight->SetIntensity(LIGHT_INTENSITY_MAX);
			m_bLightIntensityIncrease = false;
		}
	}
	else {
		// === Decrease
		newIntensity = m_pLight->GetIntensity() - ((LIGHT_INTENSITY_MAX - LIGHT_INTENSITY_MIN) * (LIGHT_INTENSITY_TIME * Time::DeltaTime));
		m_pLight->SetIntensity(newIntensity);

		if (newIntensity < LIGHT_INTENSITY_MIN) {
			m_pLight->SetIntensity(LIGHT_INTENSITY_MIN);
			m_bLightIntensityIncrease = true;
		}
	}
}

void FreezeTower::FireFreezeWave()
{
	if (m_FrozenEnemies.size() > 0 && m_pBlastWaveTimer->IsFinished()) {
		m_pBlastWaveTimer->Reset();

		GameObject* blastWave = Inferno::Instantiate("FreezeBlastWave");
		blastWave->SetLayer((m_eTeam == Team::BLUE ? (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ : (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ));
		FreezeBlastWave* fbw = blastWave->GetComponent<FreezeBlastWave>();
		fbw->SetTeam(m_eTeam);
		Vector3 position = transform->GetPosition();
		position.y += 1.0f;
		blastWave->transform->SetPosition(position);
	}
}
// ========================= //

// ===== Events ===== //
void FreezeTower::OnEffectGrowthDone()
{
	m_pIceGroundEffect->transform->SetScale(ICEEFFECT_END_SCALE, ICEEFFECT_END_SCALE, ICEEFFECT_END_SCALE);

	m_eCurrentState = FreezeTowerStates::FT_Default_State;

	m_pStateTimer->SetTime(0.0f);
	m_pStateTimer->Reset();
}

void FreezeTower::OnEffectFadeOutDone()
{
	m_pIceGroundEffect->gameObject->SetActive(false);

	m_eCurrentState = FreezeTowerStates::FT_Default_State;

	m_pStateTimer->SetTime(0.0f);
	m_pStateTimer->Reset();
}
// ================== //

// ===== Collision Events ===== //
void FreezeTower::OnTriggerEnter(Collider* _collider)
{
	BaseTower::OnTriggerEnter(_collider);

	if (_collider->IsTrigger() || _collider->gameObject == gameObject)
		return;

	Entity* entity = _collider->gameObject->GetComponent<Entity>();

	if (entity != nullptr && entity->GetTeam() == GetEnemyTeam()) {
		if (!ContainsEnemy(_collider->gameObject)) {
			GameObject* freezeEffect = Inferno::Instantiate("FreezeEffect");
			freezeEffect->transform->SetParent(_collider->gameObject->transform);

			FreezeEffect* fEffect = freezeEffect->GetComponent<FreezeEffect>();
			fEffect->SetSpeedMultipier(m_fFireRate);
			m_FrozenEnemies.add({ _collider->gameObject, fEffect });
			entity->AddForTracking(this->gameObject, BindEntityOnReleaseFunction(&FreezeTower::OnEntityRelease, this));

			if (m_pParticleEffects != nullptr) {
				m_pParticleEffects->SetActive(true);
			}

			// === Audio
			Inferno::PostAudioEventAtObjectPosition(FREEZE_TURRET_ATTACK, gameObject);

			// === Ground Effect
			if (!m_bIceEffectActive) {
				m_bIceEffectActive = true;
				m_eCurrentState = FreezeTowerStates::FT_EffectGrowth_State;

				m_pStateTimer->SetTime(ICEEFFECT_GROWTH_TIME);
				m_pStateTimer->Reset();
				m_pStateTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&FreezeTower::OnEffectGrowthDone, this));

				m_pIceGroundEffect->SetColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
				m_pIceGroundEffect->gameObject->SetActive(true);
			}
		}
	}
}

void FreezeTower::OnTriggerExit(Collider* _collider)
{
	if (_collider->IsTrigger() || _collider->gameObject == gameObject)
		return;

	Entity* entity = _collider->gameObject->GetComponent<Entity>();
	unsigned int index = 0;

	if (entity != nullptr && entity->GetTeam() == GetEnemyTeam()) {
		if (ContainsEnemy(_collider->gameObject, &index)) {
			m_FrozenEnemies[index].Effect->OnRemoveEffect();
			m_FrozenEnemies.removeAt(index);
			entity->RemoveFromTracking(this);

			if (m_FrozenEnemies.size() == 0) {
				// === Particles
				if (m_pParticleEffects != nullptr) {
					m_pParticleEffects->SetActive(false);
				}

				// === Ground Effect
				m_bIceEffectActive = false;
				m_eCurrentState = FreezeTowerStates::FT_EffectFadeOut_State;

				m_pStateTimer->SetTime(ICEEFFECT_FADEOUT_TIME);
				m_pStateTimer->Reset();
				m_pStateTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&FreezeTower::OnEffectFadeOutDone, this));
			}
		}
	}
}
// ============================ //

// ===== Mutators ===== //
void FreezeTower::SetFireRate(float _roundsPerSecond)
{
	m_fFireRate = _roundsPerSecond;
}
// ==================== //