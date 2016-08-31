// Author : Jack Gillette
//
// Main Base that handles Winning or Losing when it gets destroyed depending on it's team.
#pragma once
#include "BaseTower.h"
#include "InfernoTimer.h"

#include <vector>

class AlertMessage;
class FadeAway;
class RectTransform;
class Texture;
class TowerNode;
class UIElement;
class UIRenderer;

class MainBase : public BaseTower
{
private:
	// === Structures
	struct MapElement {
		Transform* transform;
		UIElement* element;
	};
	struct GuardTower {
		BaseTower* Tower;
		float HealthPortion;
	};

	// === Variables
	std::vector<GuardTower> m_vGuardTowers;
	std::vector<TowerNode*> m_vGuardNodes;
	std::string m_sNextSceneName;
	AlertMessage* m_pAlertMessage;
	FadeAway* m_pFadeAwayEffect;
	RectTransform* m_pTransformUnderAttackPing;
	Texture* m_pIcon;
	Texture* m_pIconUnderAttackPing;
	UIElement* m_pIconElement;
	UIElement* m_pElementUnderAttackPing;
	UIRenderer* m_pIconRenderer;
	UIRenderer* m_pHealthBar;
	UIRenderer* m_pRendererUnderAttackPing;
	InfernoTimer m_itUnderAttackPingTimer;
	MapElement m_pMapElement;
	MapElement m_sMapElementUnderAttackPing;
	unsigned short m_usTotalSpawners;
	unsigned short m_usActiveSpawners;
	float m_fGuardTowerHealthPortion;
	float m_fMaxPingSize;
	float m_fMinPingSize;
	float m_fPingTime;
	bool m_bUnderAttackIsPingable;

	// === Private Interface === //
	virtual void Death() override;
	// === CreateSpawnerUnderAttackPing
	//  IN : void 
	//  OUT : void
	//
	//  Creates and Registers Base Under Attack MiniMap Ping Event
	void CreateBaseUnderAttackPing();
	// === ClearMiniMapPingEvents
	//  IN : void   
	//  OUT : void
	//
	//  Clears MiniMap Ping Renderers
	void ClearMiniMapPingEvents();
	// === UpdateBaseUnderAttackPing
	//  IN : void 
	//  OUT : void
	//
	//  Updates Spawner Under Attack Ping If Active
	void UpdateBaseUnderAttackPing();
	// === StartBaseUnderAttackPing
	//  IN : void    
	//  OUT : void
	//
	//  Starts The Base Under Attack MiniMap Ping Event
	void StartBaseUnderAttackPing();
	// === EndBaseUnderAttackPing
	//  IN : void    
	//  OUT : void
	//
	//  Ends The Base Under Attack MiniMap Ping Event
	void EndBaseUnderAttackPing();
	void OnGuardTowerBuilt(BaseTower* _tower);
	void OnGuardTowerDamaged(BaseTower* _tower);
	void OnGuardTowerDestroyed(Entity* _entity);
	void OnSpawnerActivationStatus(bool _active);
	void UpdateBaseHealth();
	// ========================= //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	MainBase& operator=(const MainBase& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;

	virtual void UpdateHealthBar() override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	MainBase(GameObject* _gameObject, Transform* _transform);
	virtual ~MainBase();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	virtual void TakeDamage(DamageInfo _damageInfo) override;
	// ================= //

	// === Collision Interface === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	// =========================== //

	// === Accessors === //

	// === GetClosestGuardTower
	//  IN : Vector3 - Position of the Entity / Object requesting the closest guard tower
	//  OUT : Entity* - Closest Guard Tower
	//
	//  Finds the closest Guard Tower to the requesting object, and returns a pointer to it. Returns nullptr if there are no guard towers left.
	Entity* GetClosestGuardTower(Vector3 _position);
	// ================= //
};

