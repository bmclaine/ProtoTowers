// Author : Jack Gillette
//
// Handles the spots on the map where towers can be built, and how the player can interact with it.
#pragma once
#include "Components\IComponent.h"
#include "../Core/iInteractable.h"

#include <functional>

class BaseTower;
class InfernoTimer;
class PlayerController;
class SpriteRenderer;
class TowerBuildMenu;
enum Team;
enum TowerTypes;

// === Defines
#define BindOnTowerBuiltFunction(Function, Class) { std::bind(Function, Class, std::placeholders::_1) }

class TowerNode : public IComponent, iInteractable
{
private:
	// === TypeDefs
	typedef std::function<void(BaseTower*)> OnBuildTowerFunc;

	// === Structures 
	struct TowerBuildEvent {
		void* CallingObject;
		OnBuildTowerFunc Func;
	};

	// === Static Variables
	static TowerBuildMenu* s_pBuildMenu;
	static unsigned int s_uiPlayerAutoTurretLevel;
	static unsigned int s_uiPlayerLazerTurretLevel;
	static unsigned int s_uiPlayerFreezeTowerLevel;
	static unsigned int s_uiPlayerMortarTowerLevel;
	static unsigned int s_uiPlayerPoisonTurretLevel;

	// === Variables
	std::vector<TowerBuildEvent> m_vOnBuildTowerFuncs;
	TowerTypes m_eStartingTower;
	BaseTower* m_pOwnedTower;
	InfernoTimer* m_pBuildTimer;
	PlayerController* m_pInteractingPlayer;
	SpriteRenderer* m_pBuildPrompt;
	SpriteRenderer* m_pBuildIcon;
	Texture* m_pPromptTexture;
	Team m_eTeam;
	TowerTypes m_eTowerToBuild;
	unsigned int m_uiEnemyTowerLevel;
	bool m_bAvailable;
	bool m_bBuilding;
	bool m_bMenuOpen;

	// === Private Interface === //
	CollisionLayers GetLayerFromTeam() const;
	void UpgradeTower();
	std::string GetTowerPrefabName(TowerTypes _type);
	void OnTowerDestroyed(Entity* _entity);
	void OnUpgradeTowers(void* _pData);
	void OnBuildTowerDone();
	// ========================= //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	TowerNode& operator=(const TowerNode& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	TowerNode(GameObject* _gameObject, Transform* _transform);
	virtual ~TowerNode();
	// ================================ //

	// === Static Interface === //
	static void UpgradeTowerType(TowerTypes _type);
	// ======================== //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroy() override;

	virtual void Update() override;

	virtual void Interact() override;
	void CloseMenu(void* _pData = nullptr);
	void BuildTower(TowerTypes _type);
	void AddForOnBuildTower(void* _callingObject, OnBuildTowerFunc _func);
	void RemoveFromOnBuildTower(void* _callingObject);
	void PauseBuildTimer(bool pause);
	void CancelBuiltTower();
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider) override;
	virtual void OnTriggerStay(Collider* _collider) override;
	virtual void OnTriggerExit(Collider* _collider) override;
	// ======================== //

	// === Accessors === //
	Team GetTeam() const;
	bool IsAvailble() const;
	bool IsBuilding() const;
	// ================= //

	// === Mutators === // 
	void SetTeam(Team _team);
	void SetStartingTower(TowerTypes _type);
	// ================ //
};

