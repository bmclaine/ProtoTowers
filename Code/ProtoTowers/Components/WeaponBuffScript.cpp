#include "stdafx.h"
#include "WeaponBuffScript.h"
#include "BaseWeapon.h"

WeaponBuffScript::WeaponBuffScript(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_pSkinnedRenderer = nullptr;
	m_pParent = nullptr;

	m_mOffsetMatrix = Matrix4::Scale(Vector3(1.0f, 1.0f, 1.0f)) * Matrix4::RotationZ(0.0f) * Matrix4::RotationX(0.0f) * Matrix4::RotationY(0.0f) * Matrix4::Translate(Vector3(0.0f, 0.25f, -5.0f));
}

WeaponBuffScript::~WeaponBuffScript()
{
}

WeaponBuffScript& WeaponBuffScript::operator=(const WeaponBuffScript& rhs)
{
	m_fBuffTime = rhs.m_fBuffTime;
	m_fShotSpeedIncrease = rhs.m_fShotSpeedIncrease;

	return *this;
}
IComponent* WeaponBuffScript::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	WeaponBuffScript* wbs = new WeaponBuffScript(gameObject, transform);
	(*wbs) = *this;

	return wbs;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void WeaponBuffScript::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void WeaponBuffScript::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void WeaponBuffScript::OnEnable()
{
	Inferno::Destroy(gameObject, m_fBuffTime);
	m_pParent = gameObject->transform->GetParent();

	if (m_pParent)
	{
		BaseWeapon* weapon = m_pParent->gameObject->GetComponentInChildren<BaseWeapon>();
		m_pSkinnedRenderer = m_pParent->gameObject->GetComponentInChildren<SkinnedMeshRenderer>();

		if (weapon)
		{
			weapon->SetSecFireRateScalar(m_fShotSpeedIncrease);
			weapon->ToggleWeaponBuff(true);
		}
	}
}
void WeaponBuffScript::OnDisable()
{
	if (m_pParent)
	{
		BaseWeapon* weapon = m_pParent->gameObject->GetComponentInChildren<BaseWeapon>();

		if (weapon)
		{
			weapon->SetSecFireRateScalar(1.0f);
			weapon->ToggleWeaponBuff(false);
			Inferno::PostAudioEvent(ABILITY_WEAPON_BUFF_DEACTIVATE);
		}
	}
}

// IN: void
// OUT: void
//
// Update that is called every frame
void WeaponBuffScript::Update()
{
	if (m_pSkinnedRenderer)
	{
		Matrix4 handBone = m_pSkinnedRenderer->GetBoneWorld(RIFLE_BONE_INDEX);
		transform->SetLocalMatrix(m_mOffsetMatrix * handBone);
	}
}

void WeaponBuffScript::SetBuffTime(float time)
{
	m_fBuffTime = time;
}
void WeaponBuffScript::SetShotSpeedIncrease(float increase)
{
	m_fShotSpeedIncrease = increase;
}