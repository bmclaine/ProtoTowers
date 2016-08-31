#pragma once
#include "Components\IComponent.h"

class Texture;

class FrostHUDEffectPrefab : public IComponent
{
private:
	// === Variables
	Texture* m_pFrostImage;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	FrostHUDEffectPrefab& operator=(const FrostHUDEffectPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	FrostHUDEffectPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~FrostHUDEffectPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

