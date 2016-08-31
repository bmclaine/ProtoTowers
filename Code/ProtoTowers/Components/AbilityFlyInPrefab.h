#pragma once
#include "Components\IComponent.h"

class AbilityFlyInPrefab : public IComponent
{
private:
	// === Variables
	Texture* m_pAbilityTexture;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	AbilityFlyInPrefab& operator=(const AbilityFlyInPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	AbilityFlyInPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~AbilityFlyInPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

