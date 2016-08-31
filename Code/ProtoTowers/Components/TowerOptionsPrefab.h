#pragma once
#include "Components\IComponent.h"

class TowerOptionsPrefab : public IComponent
{
protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	TowerOptionsPrefab& operator=(const TowerOptionsPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	TowerOptionsPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~TowerOptionsPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

