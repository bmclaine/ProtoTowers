#pragma once
#include "Components\IComponent.h"

class HelpScreenPrefab : public IComponent
{
protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	HelpScreenPrefab& operator=(const HelpScreenPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	HelpScreenPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~HelpScreenPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

