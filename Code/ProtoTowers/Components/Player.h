#pragma once
#include "..\Core\iDamagable.h"
#include "..\Core\iInput.h"
#include "Entity.h"

#include "InfernoTimer.h"

#include <functional>
#include <vector>

class PlayerController;
class BaseWeapon;
struct Color;
class SkinnedMeshRenderer;
class Transform;
class FadeAway;
class GameObject;
class UIRenderer;
class UIElement;
class InfernoUI;
class Texture;

// === Defines 
#define BindForResourceTracking(Func, Class) { std::bind(Func, Class, std::placeholders::_1) }

class Player : public Entity, iInput
{
public:
	// === TypeDefs
	typedef std::function<void(unsigned int)> ResourceUpdateFunc;

private:
	struct MapElement
	{
		Transform* transform;
		UIElement* element;
	};

	UIElement  *m_pIconElement;
	UIRenderer *m_pIconRenderer;
	Texture    *m_pIcon;
	MapElement  m_pMapElement;

	// === Structures
	struct ResourceTrackingObject {
		void* Object;
		ResourceUpdateFunc Func;
	};

	// === Static Variables
	static Player* s_pPlayer;
	static unsigned int s_uiResources;

	// === Variables
	BaseWeapon* m_pWeapon;
	FadeAway* m_pFadeAwayEffect;
	PlayerController* m_pPlayerController;
	Transform* m_pSpawnLocation;
	UIRenderer* m_pHealthBar;
	UIRenderer* m_pEnergyBar;
	UIRenderer* m_pEnergyOverheatIndicator;
	std::vector<ResourceTrackingObject> m_vResourceTrackingObjects;
	InfernoTimer m_pInvulnerabilityTimer;
	InfernoTimer m_RespawnTimer;
	InfernoTimer m_HealthFlashTimer;
	InfernoTimer m_OverheatFlashTimer;
	bool m_bGodMode;
	InfernoTimer m_visualDamageTimer;
	bool m_bDamageTimerIsInitialized;
	bool m_bIsInvulnerable;
	SkinnedMeshRenderer* m_pSkinnedMeshRenderer;
	std::vector<MeshRenderer*> m_vExtraMeshes;
	
	// === Private Interface === //
	void Death();
	void UpdateEnergyBar(float _energyLevel);
	void UpdateResources();
	void Respawn();
	void FlashHealthBar();
	void FlashOverheatImage();
	// ========================= //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	Player& operator=(const Player& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	Player(GameObject* _owner, Transform* _transform);
	~Player();
	// ================================ //

	// === Interface === //
	virtual void Init() override;
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroy() override;
	virtual void Update() override;

	virtual void Shoot(int shotType) override;
	virtual void TakeDamage(DamageInfo _damageInfo) override;

	void ShootDebugProjectile();
	void UpdateHealthBar();
	void FlashPlayer();
	void TeleportToSpawnLocation();
	void Recall();
	// ================= //

	// === Static Interface === //
	
	// === AddResources
	//  IN : unsigned int - Amount to add
	//  OUT : void
	//
	//  Adds the given amount to the Player's Resource count.
	static void AddResources(unsigned int _amount);
	// === GetResources
	//  IN : void
	//  OUT : unsigned int - The current Resource count of the Player
	//
	//  Returns the amount of Resources that the Player currently has.
	static unsigned int GetResources();
	// === UseResources
	//  IN : unsigned int - Amount to use
	//  OUT : bool - Whether or not the Player had enough, and the amount was taken from the Player
	//
	//  Takes the given amount of resources away from the Player, if and only if they have enough. Returns true if the amount was taken out.
	static bool UseResources(unsigned int _amount);
	// === HasEnoughResources
	//  IN : unsigned int - Amount to check against
	//  OUT : bool - Whether or not the Player has enough Resources
	//
	//  Checks to see if the Player has enough resources compared to the amount specified, return true if they do, false otherwise.
	static bool HasEnoughResources(unsigned int _amount);
	static void AddForResourceTracking(void* _callingObject, ResourceUpdateFunc _func);
	static void RemoveFromResourceTracking(void* _callingObject);
	// ======================== //

	// === Collision Events === //
	virtual void OnCollisionEnter(Collider* _collider) override;
	virtual void OnCollisionExit(Collider* _collider) override;
	// ======================== //

	// === Accessors === //
	bool GetIsInvurnerable();
	// ================= //

	// === Mutators === //
	void SetHealthBar(UIRenderer* _healthBar);
	void SetEnergyBar(UIRenderer* _energyBar);
	void SetPlayerController(PlayerController* _pController);
	void SetWeapon(BaseWeapon* _weapon);
	void SetInvurnerable(bool _invurnerable);
	void SetMeshColor(Color& _color, bool _extraMeshesIncluded = false);
	// ================ //

	BaseWeapon* GetWeapon() const;
};

