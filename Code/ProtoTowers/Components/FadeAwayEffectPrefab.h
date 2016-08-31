#pragma once
#include "Components\IComponent.h"

class FadeAwayEffectPrefab : public IComponent
{
protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	FadeAwayEffectPrefab& operator=(const FadeAwayEffectPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	FadeAwayEffectPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~FadeAwayEffectPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

