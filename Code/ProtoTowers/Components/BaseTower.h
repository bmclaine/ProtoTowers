// Author : Jack Gillette
//
// Base class that all Towers will derive from.
#pragma once
#include "Entity.h"
#include "..\Core\iDamagable.h"

class InfernoTimer;
class MeshRenderer;
class ParticleSystem;
class SpriteRenderer;
class TowerOptionsScript;

// === Defines
#define MAX_DAMAGE_PARTICLES 50
#define BindOnTowerTakeDamage(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }

// === Enumerations
enum TowerTypes { TT_UNKNOWN = 0, TT_SPAWNER, TT_MAINBASE, TT_AUTOTURRET, TT_FREEZETOWER, TT_MORTAR, TT_LAZERTURRET, TT_POISONTOWER, MAX_TOWERTYPES };

class BaseTower : public Entity
{
private:
	// === TypeDefs
	typedef std::function<void(BaseTower*)> OnTakeDamageFunc;

	// === Structures
	struct TakeDamageEvent {
		void* CallingObject;
		OnTakeDamageFunc Func;
	};

	// === Static Variables
	static TowerOptionsScript* s_pTowerOptions;
	static unsigned int s_uiOptionsRefCounter;

	// === Variables
	std::vector<TakeDamageEvent> m_vTakeDamageFuncs;
	InfernoTimer* m_pRepairTimer;

	// === Private Events === //
	void OnRepairEnd();
	// ====================== //

protected:
	// === Variables
	TowerTypes m_eType;
	ParticleSystem* m_pDamageParticleSystem;
	SpriteRenderer* m_pHealthBar;
	MeshRenderer* m_pRenderer;
	float m_fStartingYRot;
	float m_fDamageResistance;
	unsigned int m_uiUpgradeLevel;
	bool m_bPlayerInteracting;

	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	BaseTower& operator=(const BaseTower& _rhs);
	virtual void Death() override;
	virtual void UpdateDamageParticles();
	virtual void UpdateHealthBar();
	// =========================== //

public:
	// === Constructor / Destructor === //
	BaseTower(GameObject* _owner, Transform* _transform);
	virtual ~BaseTower() override;
	// ================================ //

	// === Interface === //

	// === PostInit
	//  IN : map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//  OUT : void
	//
	//  Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void TakeDamage(DamageInfo _info) override;
	virtual void Repair();
	virtual void Sell();
	void OpenTowerOptionsMenu();
	void CloseTowerOptionsMenu();
	void AddForOnTakeDamage(void* _callingObject, OnTakeDamageFunc _func);
	void RemoveFromOnTakeDamage(void* _callingObject);
	// ================= //

	// === Static Interface === //
	static std::string GetNameOfTower(TowerTypes _type);
	static unsigned int GetPriceOfTower(TowerTypes _type);
	static unsigned int GetUpgradePrice(TowerTypes _type);
	// ======================== //

	// === Collision Events === //
	virtual void OnCollisionEnter(Collider* _collider) override;
	virtual void OnCollisionExit(Collider* _collider) override;
	virtual void OnTriggerEnter(Collider* _collider) override;
	// ======================== //

	// === Accessors === //
	TowerTypes GetType() const;
	unsigned int GetUpgradeLevel() const;
	// ================= //

	// === Mutators === //
	void SetTowerType(TowerTypes _type);
	void SetStartingYRotation(float _yRotRadians);
	void SetDamageResistance(float _resistance);
	// ================ //
};