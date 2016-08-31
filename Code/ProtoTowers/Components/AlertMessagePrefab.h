#pragma once
#include "Components\IComponent.h"

class AlertMessagePrefab : public IComponent
{
protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	AlertMessagePrefab& operator=(const AlertMessagePrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	AlertMessagePrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~AlertMessagePrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

