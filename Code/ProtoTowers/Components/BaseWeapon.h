//Author: Zane Lewis
//
//Standard weapon
#pragma once
#define NUM_PROJECTILES 30
#define RIFLE_BONE_INDEX 29
#define FIRE_RATE .25f
#define RECHARGE_DELAY .75f
#define RECHARGE_RATE .075f
#define OVERHEAT_DELAY .25f
#define OVERHEAT_LIMIT 10
#define MAX_LEVEL 5
#define BASE_DAMAGE 20
#define MAX_KILLCOUNT 5

#define SECONDARY_FIRE_RATE 0.125f
#define SECONDARY_BASE_DAMAGE 20

#include "Assets\Material.h"
#include <functional>

class IComponent;
class GameObject;
class InfernoTimer;

// === TypeDefs
typedef std::function<void(float)> EnergyUpdateFunction;

class BaseWeapon : public IComponent
{
protected:
	InfernoTimer m_RechargeDelay;
	InfernoTimer m_FireRate;
	InfernoTimer m_SecondFireRate;
	InfernoTimer m_RechargeRate;
	EnergyUpdateFunction  m_EnergyUpdateFunc;
	int m_iNumRoundsLeft;
	int m_iMaxNumRounds;
	int m_Level;
	float m_currentFireRate;
	float m_currentDamage;
	float m_fFireRateScalar;
	float m_fSecFireRateScalar;
	float m_fShotCostScalar;
	bool m_bIsOverheated;
	bool m_bBuffed;
	int m_currentKillCount;
	Transform * m_pRotationTransfrom;

	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;
	IComponent& operator=(IComponent& _rhs) override;
	BaseWeapon& operator=(const BaseWeapon& _rhs);
public:

	BaseWeapon(GameObject* const gameObject, Transform* const transform);
	virtual ~BaseWeapon() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void Update() override;

	//DESCRIPTION: Fires a projectile if there are any available
	//
	//IN: void
	//
	//OUT: bool: returns true if a projectile is fired
	virtual bool FireProjectile(Entity* _owningEntity);

	virtual bool FireSecondProjectile(Entity* _owningEntity);

	//DESCRIPTION: sets projectiles to fireable 
	//
	//IN: void
	//
	//OUT: void
	virtual void Recharge(void);

	virtual void LevelUp();

	virtual void LevelDown();

	virtual void ResetLevel();

	int GetKillCount() const;

	void SetKillCount(int killCount);

	//DESCRIPTION: Gets the ratio of projectiles left to max projectiles
	//
	//IN: void
	//
	//OUT: void
	virtual float GetEnergyLevel() const;
	float GetFireRateScalar() const;
	float GetShotCostScalar() const;

	virtual void SetEnergyUpdateFunction(EnergyUpdateFunction _func);
	void SetFireRateScalar(float scalar);
	void SetSecFireRateScalar(float scalar);
	void SetShotCostScalar(float scalar);

	void SetRotationTransform(Transform * _transform);

	void SetMaxAmmo(int _maxAmmo);

	void SetLevelUpBar();
	void SetLevelIndicator();

	void ToggleWeaponBuff(bool buffed);
};

