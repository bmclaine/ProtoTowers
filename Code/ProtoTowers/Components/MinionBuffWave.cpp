#include "stdafx.h"
#include "MinionBuffWave.h"
#include "Assets\Material.h"

#include "Engine.h"

MinionBuffWave::MinionBuffWave(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_pMaterial = nullptr;
	m_fRadius = 10.0f;
}

MinionBuffWave::~MinionBuffWave()
{
}

MinionBuffWave& MinionBuffWave::operator=(const MinionBuffWave& rhs)
{
	return *this;
}
IComponent* MinionBuffWave::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	MinionBuffWave* mbw = new MinionBuffWave(gameObject, transform);
	(*mbw) = *this;

	return mbw;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void MinionBuffWave::Init()
{
	
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void MinionBuffWave::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void MinionBuffWave::OnEnable()
{
	m_itScaleTimer.SetTime(0.5f);
	Inferno::Destroy(gameObject, 0.5f);

	MeshRenderer* rend = gameObject->GetComponent<MeshRenderer>();

	if (rend)
		m_pMaterial = rend->GetMaterial();
}
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void MinionBuffWave::OnDisable()
{

}

// IN: void
// OUT: void
//
// Update that is called every frame
void MinionBuffWave::Update()
{
	float scale = m_fRadius * (1.0f - m_itScaleTimer.TimeRemaining() / m_itScaleTimer.GetSetTimeSeconds());
	transform->SetScale(scale, 2.0f, scale);

	if (m_pMaterial)
		m_pMaterial->SetOffsets(Vector2(m_pMaterial->GetOffsets().x + 3.0f * Time::DeltaTime, 0.0f));
}