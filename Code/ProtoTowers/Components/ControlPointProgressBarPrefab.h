//Author: Zane Lewis
//
//Prefab for the ControlPointProgressBar
#pragma once
#include "ControlPointProgressBar.h"

class ControlPointProgressBarPrefab : public IComponent
{
private:
	Texture * m_pProgressBarTexture;
protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	ControlPointProgressBarPrefab& operator=(const ControlPointProgressBarPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
public:
	ControlPointProgressBarPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~ControlPointProgressBarPrefab();

	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
};