#pragma once
#include "UIComponents\UIComponent.h"

#include "Color.h"
#include <queue>

class InfernoTimer;
class UIRenderer;
class UIText;

class ToolTipManager : public UIComponent
{
private:
	// === Enumerations
	enum class ToolTipFlags { TT_ShowToolTips_Flag = 0, TT_CurrentlyDisplaying_Flag = 1, TT_TOTAL_FLAGS = 2 };
	enum class ToolTipStates { TT_Default_State = 0, TT_FlyIn_State = 1, TT_FadeOut_State = 2 };

	// === Structures
	struct ToolTip {
		std::string Message;
		Color Color;
		float DisplayTime;
	};

	// === Variables
	std::queue<ToolTip> m_QueuedTips;
	InfernoTimer* m_pDisplayTimer;
	UIElement* m_pVisualsElement;
	UIRenderer* m_pMessageBkgd;
	UIText* m_pMessageLbl;
	UIText* m_pMessageText;
	ToolTip m_stCurrentTip;
	Vector2 m_v2StartingPosition;
	Vector2 m_v2EndingPosition;
	Flags<char> m_fcToolTipFlags;
	ToolTipStates m_eCurrentState;

	// === Private Interface === //
	void DisplayNextTip();
	void QueueTip(ToolTip _tip);
	void RegisterForToolTipEvents();
	void UnregisterFromToolTipEvents();
	void UpdateFlyInState();
	void UpdateFadeOutState();
	// ========================= //

	// === Tool Tip Events === //
	void ToolTip_BaseGuardTowers(void* _pData);
	void ToolTip_BaseSpawners(void* _pData);
	void ToolTip_ControlPointSpawners(void* _pData);
	void ToolTip_TowerUpgrades(void* _pData);
	void ToolTip_BossHealth(void* _pData);
	void ToolTip_SpawnerUpgrade(void* _pData);
	void ToolTip_BuffSpawner(void* _pData);
	// ======================= //

	// === Events === //
	void OnShowToolTips(void* _pData);
	void OnTipDone();
	void OnFlyInDone();
	// ============== //

public:
	// === Constructor / Destructor === //
	ToolTipManager(UIElement* _owner);
	virtual ~ToolTipManager();
	// ================================ //

	// === Interface === //
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //
};

