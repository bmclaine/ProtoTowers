#pragma once
#include "Components\IComponent.h"

class LoseHUDPrefab : public IComponent
{
private:
	// === Variable
	Texture* m_pLoseImage;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	LoseHUDPrefab& operator=(const LoseHUDPrefab& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	LoseHUDPrefab(GameObject* _gameObject, Transform* _transform);
	virtual ~LoseHUDPrefab();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	// ================= //
};

