#pragma once
#include "Components\IComponent.h"

class Light;
struct Material;

class TowerUpgradeEffect : public IComponent
{
private:
	// === Enumerations 
	enum class EffectStates { ES_Growing_State = 0, ES_Shrink_State = 1 };

	// === Variables
	Light* m_pLight;
	Material* m_pMaterial;
	float m_fDisplacement;
	EffectStates m_eState;
	bool m_bLightIntensityIncrease;

	// === Private Interface === //
	void UpdateGrowing();
	void UpdateLight();
	void UpdateShrinking();
	// ========================= //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	TowerUpgradeEffect& operator=(const TowerUpgradeEffect& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	TowerUpgradeEffect(GameObject* _gameObject, Transform* _transform);
	virtual ~TowerUpgradeEffect();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //
};

