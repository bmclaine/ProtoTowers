#include "stdafx.h"
#include "TowerUpgradeEffect.h"

#include "Assets\Material.h"

// === Defines
#define GROWTH_SPEED 1.1f
#define DESIRED_OFFSET 1.25f
#define LIGHT_INTENSITY_MIN 1.0f
#define LIGHT_INTENSITY_MAX 2.0f

// ===== Constructor / Destructor ===== //
TowerUpgradeEffect::TowerUpgradeEffect(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pLight = nullptr;
	m_pMaterial = nullptr;
}

TowerUpgradeEffect::~TowerUpgradeEffect()
{

}
// ==================================== //

// ===== Interface ===== //
void TowerUpgradeEffect::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	// === Nothing to load from file
}

void TowerUpgradeEffect::OnEnable()
{
	// === Get the Light
	if (m_pLight == nullptr) {
		m_pLight = gameObject->GetComponent<Light>();
	}

	// === Get the Mesh
	m_pMaterial = gameObject->GetComponent<MeshRenderer>()->GetMaterial();

	m_fDisplacement = 0.0f;

	m_eState = EffectStates::ES_Growing_State;

	Vector3 scale = transform->GetScale();
	scale.y = 0.1f;
	transform->SetScale(scale);

	IComponent::OnEnable();
}

void TowerUpgradeEffect::OnDisable()
{
	

	IComponent::OnDisable();
}

void TowerUpgradeEffect::Update()
{
#pragma region SAFE_WAY
#if 0
	float movement = GROWTH_SPEED * Time::DeltaTime;
	
	transform->SetYPosition(transform->GetPosition().y + movement);

	m_fDisplacement += movement;
	if (m_fDisplacement > DESIRED_OFFSET) {
		Inferno::Destroy(this->gameObject);
	}

	// === Scrolling UV's
	m_pMaterial->SetOffsets(Vector2(0.0f, m_pMaterial->GetOffsets().y + (0.1f * Time::DeltaTime)));
#endif
#pragma endregion

	UpdateLight();

#pragma region OLD_EFFECT_STUFF
#if 1
	switch (m_eState) {
	case EffectStates::ES_Growing_State:
		UpdateGrowing();
		break;
	case EffectStates::ES_Shrink_State:
		UpdateShrinking();
		break;
	}
#endif 
#pragma endregion
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& TowerUpgradeEffect::operator=(IComponent& _rhs)
{
	TowerUpgradeEffect* rhsTUE = dynamic_cast<TowerUpgradeEffect*>(&_rhs);
	if (rhsTUE == nullptr) {
		return *this;
	}
	
	return operator=(*rhsTUE);
}

TowerUpgradeEffect& TowerUpgradeEffect::operator=(const TowerUpgradeEffect& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
	}

	return *this;
}

IComponent* TowerUpgradeEffect::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	TowerUpgradeEffect* newTUE = new TowerUpgradeEffect(_gameObject, _transform);
	*newTUE = *this;

	return newTUE;
}
// =============================== //

// === Private Interface === //
void TowerUpgradeEffect::UpdateGrowing()
{
	float movement = GROWTH_SPEED * Time::DeltaTime;
	Vector3 newScale = transform->GetScale();
	newScale.y += movement;

	transform->SetYPosition(transform->GetPosition().y + movement);
	transform->SetScale(newScale);

	m_pMaterial->SetTiling(Vector2(1.0f, newScale.y));

	if ((m_fDisplacement += movement) > DESIRED_OFFSET) {
		m_eState = EffectStates::ES_Shrink_State;
	}
}

void TowerUpgradeEffect::UpdateLight()
{
	if (m_pLight == nullptr) {
		return;
	}

	float newIntensity;
	if (m_bLightIntensityIncrease) {
		newIntensity = m_pLight->GetIntensity() + (LIGHT_INTENSITY_MAX - LIGHT_INTENSITY_MIN) * Time::DeltaTime;
		m_pLight->SetIntensity(newIntensity);

		if (newIntensity > LIGHT_INTENSITY_MAX) {
			m_pLight->SetIntensity(LIGHT_INTENSITY_MAX);
			m_bLightIntensityIncrease = false;
		}
	}
	else {
		newIntensity = m_pLight->GetIntensity() - (LIGHT_INTENSITY_MAX - LIGHT_INTENSITY_MIN) * Time::DeltaTime;
		m_pLight->SetIntensity(newIntensity);

		if (newIntensity < LIGHT_INTENSITY_MIN) {
			m_pLight->SetIntensity(LIGHT_INTENSITY_MIN);
			m_bLightIntensityIncrease = true;
		}
	}
}

void TowerUpgradeEffect::UpdateShrinking()
{
	float movement = GROWTH_SPEED * Time::DeltaTime;
	Vector3 newScale = transform->GetScale();
	newScale.y -= movement;

	if (newScale.y <= 0.1f) {
		Inferno::Destroy(this->gameObject);
		return;
	}

	transform->SetYPosition(transform->GetPosition().y + movement);
	transform->SetScale(newScale);

	m_pMaterial->SetOffsets(Vector2(0.0f, m_pMaterial->GetOffsets().y + (0.1f * Time::DeltaTime)));
	m_pMaterial->SetTiling(Vector2(1.0f, newScale.y));
}
// ========================= //