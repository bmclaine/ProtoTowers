// Author : Jack Gillette
//
// Child of the BaseTurret class that slows down enemies and does a small amount of damage over time.
#pragma once
#include "BaseTurret.h"

#include <list>

class FreezeEffect;
class InfernoTimer;
class Light;
class ParticleSystem;
class SpriteRenderer;

class FreezeTower : public BaseTurret
{
private:
	// === Enumerations
	enum class FreezeTowerStates { FT_Default_State = 0, FT_EffectGrowth_State = 1, FT_EffectFadeOut_State = 2 };

	// === Structures
	struct FrozenEntity {
		GameObject* Object;
		FreezeEffect* Effect;
	};

	// === Variables
	List<FrozenEntity> m_FrozenEnemies;
	GameObject* m_pParticleEffects;
	InfernoTimer* m_pStateTimer;
	InfernoTimer* m_pBlastWaveTimer;
	SpriteRenderer* m_pIceGroundEffect;
	Light* m_pLight;
	FreezeTowerStates m_eCurrentState;
	bool m_bIceEffectActive;
	bool m_bLightIntensityIncrease;

	// === Private Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	FreezeTower& operator=(const FreezeTower& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;

	bool ContainsEnemy(GameObject* _gameObject, unsigned int* _foundIndex = nullptr);
	void UpdateEffectGrowth();
	void UpdateEffectFadeOut();
	void UpdateLightIntensity();
	void FireFreezeWave();
	// ========================= //

	// === Events === //
	void OnEffectGrowthDone();
	void OnEffectFadeOutDone();
	// ============== //

protected:
	// === Protected Interface === //
	virtual void OnEntityRelease(Entity* _releasedEntity) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	FreezeTower(GameObject* _gameObject, Transform* _transform);
	virtual ~FreezeTower();
	// ================================ //
	
	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	virtual void OnTriggerExit(Collider* _collider) override;
	// ======================== //

	// === Mutators === //
	virtual void SetFireRate(float _roundsPerSecond) override;
	// ================ //
};

