// Author : Jack Gillette
//
// Script that handles all the UI stuff for the Tower Options, such as handling switching out the Control Icons
#pragma once
#include "UIComponents\UIComponent.h"

class BaseTower;
class Texture;
class UIRenderer;
class UIText;

class TowerOptionsScript : public UIComponent
{
private:
	// === Variables
	BaseTower* m_pFocusedTower;
	Texture* m_pUpgradeIcon;
	Texture* m_pSellIcon;
	Texture* m_pRepairIcon;
	UIRenderer* m_pUpgradeControlIcon;
	UIRenderer* m_pSellControlIcon;
	UIRenderer* m_pRepairControlIcon;
	UIText* m_pUpgradeText;
	UIText* m_pSellText;
	UIText* m_pRepairText;
	int m_iRepairPrice;
	int m_iSellPrice;
	int m_iUpgradePrice;
	bool m_bUpgrade;
	bool m_bUpgradeAvailable;

	// === Private Interface === //
	void CreateUIElements();
	void UpdateControlIcons(bool _controller);
	void RepairTower();
	void SellTower();
	void UpgradeTower();
	// ========================= //

	// === Input Events === //
	void OnRepairBtn(ACTION_STATUS _status);
	void OnSellBtn(ACTION_STATUS _status);
	// ==================== //

public:
	// === Constructor / Destructor === //
	TowerOptionsScript(UIElement* _owner);
	virtual ~TowerOptionsScript();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	void UpdatePrices(float _health, float _maxHealth);
	void SetupForMinionSpawner(bool _value = true);
	// ================= //

	// === Mutators === //
	void SetFocusedTower(BaseTower* _tower);
	// ================ //
};

