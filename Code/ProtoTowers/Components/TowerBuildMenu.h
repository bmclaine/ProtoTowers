// Author : Jack Gillette
//
// 
#pragma once
#include "UIComponents\UIComponent.h"

class TowerDescription;
class TowerNode;
class UISelectable;
class UIText;

class TowerBuildMenu : public UIComponent
{
private:
	// === Enumerations
	enum class Towers { AutoTurret = 0, FreezeTower = 1, PoisonTurret = 2, Mortar = 3};

	// === Variables
	TowerDescription* m_pTowerDescription;
	TowerNode* m_pOwningNode;
	UISelectable* m_pFirstSelectable;
	UIText* m_pResourcesText;
	UIRenderer* m_pSelectImage;
	UIRenderer* m_pBackImage;
	Texture* m_pSelectBtnTexture;
	Texture* m_pBackBtnTexture;
	Vector2 m_pButtonPositions[4];
	bool m_bRecieveInput;

	// === Private Interface === //
	void CloseMenu();
	// ========================= //

	// === Events === //
	void OnCloseMenu(void* _pData);
	// ============== //

public:
	// === Constructor / Destructor === //
	TowerBuildMenu(UIElement* _owner);
	virtual ~TowerBuildMenu();
	// ================================ //

	// === Interface === //
	virtual void PostInit() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	// ================= //

	// === Button Functions === //
	void OnTurretTabClick();
	void OnBuildingsTabClick();
	void OnAutoTurretBtnClick();
	void OnAutoTurretHover();
	void OnFreezeTowerBtnClick();
	void OnFreezeTurretHover();
	void OnMortarBtnClick();
	void OnMortarTurretHover();
	void OnPoisonTurretClick();
	void OnPoisonTurretHover();
	void OnTowerBtnHoverEnd();
	// ======================== //

	// === Text Functions === //
	void OnResourcesUpdate(unsigned int _newAmount);
	// ====================== //

	// === Input Functions === //
	void OnMenuExit(ACTION_STATUS _status);
	// ======================= //

	// === Mutators === //
	void SetOwningNode(TowerNode* _node);
	void SetFirstSelectable(UISelectable* _selectable);
	// ================ //
};

