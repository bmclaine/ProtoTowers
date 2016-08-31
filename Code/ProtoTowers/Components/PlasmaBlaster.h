//Author: Zane Lewis
//
//Player's main weapon
#pragma once

#include "BaseWeapon.h"
#include "MathLib.h"

class Entity;
class SkinnedMeshRenderer;
class Transform;
class UIElement;

class PlasmaBlaster : public BaseWeapon
{
private:
	SkinnedMeshRenderer* m_pSkinnedMeshRenderer;
	Matrix4 m_mOffsetMatrix;
	int m_projectileCount;
	InfernoTimer m_LevelUpTimer;
	InfernoTimer m_LevelDownTimer;

	void UpdateLocation();

	void Reset();

	void ResetSecondary();

	void MuzzleFlash();

	void ToggleLevelUpUI(bool value);
	void ToggleLevelDownUI(bool value);

	void DisableLevelUpUI();
	void EnableLevelUpUI();

	void DisableLevelDownUI();
	void EnableLevelDownUI();

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	PlasmaBlaster& operator=(const PlasmaBlaster& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;

public:
	PlasmaBlaster(GameObject* const gameObject, Transform* const transform);
	virtual ~PlasmaBlaster() override;

	virtual void Init() override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	virtual void FixedUpdate() override;

	//DESCRIPTION: Fires a projectile if there are any available
	//
	//IN: void
	//
	//OUT: bool: returns true if a projectile is fired
	virtual bool FireProjectile(Entity* _owningEntity) override;

	virtual bool FireSecondProjectile(Entity* _owningEntity) override;

	//DESCRIPTION: sets projectiles to fireable 
	//
	//IN: void
	//
	//OUT: void
	virtual void Recharge(void)	override;

	virtual void LevelUp() override;

	virtual void LevelDown() override;

	virtual void ResetLevel() override;

	virtual void OnEnable() override;

	void CreateLevelUpParticles();

	void CreateLevelDownParticles();

	void SetLevelUpText(const char* text);

	void SetLevelDownText(const char* text);
};