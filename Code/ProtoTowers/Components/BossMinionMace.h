//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

#include "Components\IComponent.h"
#include "InfernoTimer.h"

#include "MathLib.h"

class SkinnedMeshRenderer;

class BossMinionMace : public IComponent
{
private:
	SkinnedMeshRenderer* m_pSkinnedMeshRenderer;

	Matrix4 m_mOffsetMatrix;

	void UpdateLocation();

protected:
	virtual IComponent* CreateCopy(GameObject* const _gameobject, Transform* const _transform) override;

	BossMinionMace& operator=(const BossMinionMace& _rhs);

public:
	BossMinionMace(GameObject* const _gameObject, Transform* const _transform);

	BossMinionMace(const BossMinionMace& _rhs);

	virtual ~BossMinionMace() override;

	IComponent& operator=(IComponent& _rhs) override;

	virtual void Init();

	virtual void PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap);

	void Update() override;
};