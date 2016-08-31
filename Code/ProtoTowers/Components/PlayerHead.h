//*********************************************************************//
//	Author:			Brian McLaine
//	Description:	
//*********************************************************************//
#pragma once

//#include <functional>

#include "Components\IComponent.h"
#include "InfernoTimer.h"

class SkinnedMeshRenderer;

class PlayerHead : public IComponent
{
private:
	SkinnedMeshRenderer* m_pSkinnedMeshRenderer;

	Matrix4 m_mOffsetMatrix;

	InfernoTimer m_fSpinTimer;
	bool m_bActive;

	void Activate();
	void Deactivate();

	void SpinHead();

	void UpdateLocation();

protected:
	virtual IComponent* CreateCopy(GameObject* const _gameobject, Transform* const _transform) override;

	PlayerHead& operator=(const PlayerHead& _rhs);

public:
	PlayerHead(GameObject* const _gameObject, Transform* const _transform);

	PlayerHead(const PlayerHead& _rhs);

	virtual ~PlayerHead() override;

	IComponent& operator=(IComponent&) override;

	virtual void Init();

	virtual void PostInit(std::map<unsigned int, Object*>& _objectMap, std::map<unsigned int, ObjectData*>& _dataMap);

	virtual void FixedUpdate() override;

	void ExorcistMode(float _time);

	void ToggleHead(bool _active);
};