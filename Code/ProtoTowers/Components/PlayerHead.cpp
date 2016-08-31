//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#include "stdafx.h"
#include "PlayerHead.h"

#include "GameObject.h"
#include "Components\Renderer.h"
#include "Components\SkinnedMeshRenderer.h"
#include "Components\Transform.h"
#include "InfernoTime.h"

#define PLAYER_HEAD_BONE_INDEX 4

PlayerHead::PlayerHead(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_pSkinnedMeshRenderer = nullptr;
	m_bActive = true;

	m_mOffsetMatrix = Matrix4::Scale(Vector3(0.75f, 0.75f, 0.75f)) * Matrix4::RotationZ(0.0f) * Matrix4::RotationX(0.0f) * Matrix4::RotationY(0.0f) * Matrix4::Translate(Vector3(0.0f, 0.0f, 0.0f));
}

PlayerHead::~PlayerHead()
{
	
}

IComponent& PlayerHead::operator=(IComponent& _rhs)
{
	PlayerHead* rhs = dynamic_cast<PlayerHead*>(&_rhs);
	if (rhs != nullptr)
		return *this;

	return operator=(*rhs);
}

IComponent* PlayerHead::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	PlayerHead* newComponent = new PlayerHead(_gameObject, _transform);
	return newComponent;
}

PlayerHead& PlayerHead::operator=(const PlayerHead& _rhs)
{
	if (this == &_rhs)
		return *this;

	m_fSpinTimer = _rhs.m_fSpinTimer;
	m_fcFlags = _rhs.m_fcFlags;

	return *this;
}

void PlayerHead::Init()
{
	m_pSkinnedMeshRenderer = gameObject->transform->GetParent()->gameObject->GetComponentInChildren<SkinnedMeshRenderer>(true);
}

void PlayerHead::PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap)
{
	ObjectData* data = _dataMap.find(GetInstanceID())->second;

	std::string propName = "Active";
	Property* prop = data->GetProperty(propName);
	m_bActive = atoi(prop->value.c_str()) != 0;
}

void PlayerHead::FixedUpdate()
{
	if (m_bActive == true)
	{
		if (m_fSpinTimer.DifferenceBetweenTheStartTimeAndTimePassedForBrian() > 0.0f)
			SpinHead();

		UpdateLocation();
	}
}

void PlayerHead::ExorcistMode(float _time)
{
	m_fSpinTimer.SetTime(_time);
	m_fSpinTimer.Reset();
}

void PlayerHead::ToggleHead(bool _active)
{
	if (m_bActive == _active)
		return;

	if (_active == true)
		Activate();
	else
		Deactivate();
}

void PlayerHead::Activate()
{
	if (m_bActive == true)
		return;

	m_bActive = true;

	Renderer* pRenderer = gameObject->GetComponent<Renderer>(true);
	pRenderer->SetEnabled(true);
}

void PlayerHead::Deactivate()
{
	m_bActive = false;

	Renderer* pRenderer = gameObject->GetComponent<Renderer>(true);
	pRenderer->SetEnabled(false);
}

void PlayerHead::SpinHead()
{
	float rotSpeed = 150.0f * Time::DeltaTime;
	transform->Rotate(Vector3(0.0f, ToRadians(rotSpeed), 0.0f));
}

void PlayerHead::UpdateLocation()
{
	Matrix4 boneWorldMatrix = m_pSkinnedMeshRenderer->GetBoneWorld(PLAYER_HEAD_BONE_INDEX);
	transform->SetLocalMatrix(m_mOffsetMatrix * boneWorldMatrix);
}