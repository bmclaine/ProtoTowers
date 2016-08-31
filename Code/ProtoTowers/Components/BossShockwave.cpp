#include "stdafx.h"
#include "BossShockwave.h"
#include "Assets/Material.h"
#include "Engine.h"


BossShockwave::BossShockwave(GameObject* const _gameObject, Transform* const _transform)
	: IComponent(_gameObject, _transform)
	, m_fRadius(20.0f)
	, m_pMaterial(nullptr)
{ }

BossShockwave::~BossShockwave()
{ }

BossShockwave& BossShockwave::operator=(const BossShockwave& _assign)
{
	return *this;
}

IComponent* BossShockwave::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	BossShockwave* copy = new BossShockwave(gameObject, transform);
	(*copy) = *this;

	return copy;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void BossShockwave::Init()
{ }

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void BossShockwave::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{ }

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void BossShockwave::OnEnable()
{
	const float LIFE_TIME = 0.5f;

	m_itScaleTimer.SetTime(LIFE_TIME);
	Inferno::Destroy(gameObject, LIFE_TIME);

	MeshRenderer* rend = gameObject->GetComponent<MeshRenderer>();

	if (rend != nullptr)
		m_pMaterial = rend->GetMaterial();
}

// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void BossShockwave::OnDisable()
{ }

// IN: void
// OUT: void
//
// Update that is called every frame
void BossShockwave::Update()
{
	float difference = m_itScaleTimer.TimeRemaining() / m_itScaleTimer.GetSetTimeSeconds();
	float scale = m_fRadius * (1.0f - difference);
	transform->SetScale(scale, 3.0f, scale);

	if (m_pMaterial != nullptr)
		m_pMaterial->SetOffsets(Vector2(m_pMaterial->GetOffsets().x + 3.0f * Time::DeltaTime, 0.0f));
}

