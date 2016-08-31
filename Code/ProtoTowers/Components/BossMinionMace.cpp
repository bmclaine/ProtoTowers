//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#include "stdafx.h"
#include "BossMinionMace.h"

#define BOSS_MINION_HAND_BONE 17

BossMinionMace::BossMinionMace(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_pSkinnedMeshRenderer = nullptr;

	m_mOffsetMatrix = Matrix4::Scale(Vector3(1.25f, 1.25f, 1.25f)) * Matrix4::RotationZ(0.0f) * Matrix4::RotationX(ToRadians(90.0f)) * Matrix4::RotationY(0.0f) * Matrix4::Translate(Vector3(0.0f, 0.0f, 10.0f));
}

BossMinionMace::~BossMinionMace()
{

}

IComponent& BossMinionMace::operator=(IComponent& _rhs)
{
	BossMinionMace* rhs = dynamic_cast<BossMinionMace*>(&_rhs);
	if (rhs != nullptr)
		return *this;

	return operator=(*rhs);
}

void BossMinionMace::Init()
{
	m_pSkinnedMeshRenderer = gameObject->transform->GetParent()->gameObject->GetComponentInChildren<SkinnedMeshRenderer>(true);
}

void BossMinionMace::PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap)
{

}

void BossMinionMace::Update()
{
	UpdateLocation();
}

IComponent* BossMinionMace::CreateCopy(GameObject* const _gameobject, Transform* const _transform)
{
	BossMinionMace* newComponent = new BossMinionMace(_gameobject, _transform);
	return newComponent;
}

BossMinionMace& BossMinionMace::operator=(const BossMinionMace& _rhs)
{
	if (this == &_rhs)
		return *this;

	m_pSkinnedMeshRenderer = _rhs.m_pSkinnedMeshRenderer;
	m_mOffsetMatrix = _rhs.m_mOffsetMatrix;
	m_fcFlags = _rhs.m_fcFlags;

	return *this;
}

void BossMinionMace::UpdateLocation()
{
	Matrix4 boneWorldMatrix = m_pSkinnedMeshRenderer->GetBoneWorld(BOSS_MINION_HAND_BONE);
	transform->SetLocalMatrix(m_mOffsetMatrix * boneWorldMatrix);
}