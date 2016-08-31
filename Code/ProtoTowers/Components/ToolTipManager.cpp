#include "stdafx.h"
#include "ToolTipManager.h"

#include "EventSystem.h"
#include "InfernoUI.h"
#include "ModuleInterface.h"

// === Defines
#define FLY_IN_TIME 1.5f
#define STARTING_SCALE 0.3f
#define ENDING_SCALE 1.0f
#define TOOLTIP_FADEOUT_TIME 2.0f
#define TOOLTIP_BKGD_COLOR Color(0.55f, 0.55f, 0.55f, 0.6f)

// ===== Constructor / Destructor ===== //
ToolTipManager::ToolTipManager(UIElement* _owner) : UIComponent(_owner), m_fcToolTipFlags((unsigned int)ToolTipFlags::TT_TOTAL_FLAGS)
{
	m_pDisplayTimer = new InfernoTimer();
	m_pDisplayTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&ToolTipManager::DisplayNextTip, this));

	m_fcToolTipFlags.SetFlag((unsigned int)ToolTipFlags::TT_ShowToolTips_Flag, (unsigned int)true);
	m_fcToolTipFlags.SetFlag((unsigned int)ToolTipFlags::TT_CurrentlyDisplaying_Flag, (unsigned int)false);

	m_pMessageBkgd = nullptr;
	m_pMessageLbl = nullptr;
	m_pMessageText = nullptr;
	m_pVisualsElement = nullptr;

	m_eCurrentState = ToolTipStates::TT_Default_State;
}

ToolTipManager::~ToolTipManager()
{
	UnregisterFromToolTipEvents();

	delete m_pDisplayTimer;
}
// ==================================== //

// ===== Interface ===== //
void ToolTipManager::OnEnable()
{
	RegisterForToolTipEvents();

	if (m_pMessageBkgd == nullptr) {
		UIElement* bkgdElement = GetTransform()->GetChildByName(std::string("ToolTip Background"), true);
		m_pMessageBkgd = bkgdElement->GetComponent<UIRenderer>();
	}

	if (m_pMessageLbl == nullptr) {
		UIElement* lblElement = GetTransform()->GetChildByName(std::string("ToolTip Label"), true);
		m_pMessageLbl = lblElement->GetComponent<UIText>();
	}

	if (m_pMessageText == nullptr) {
		UIElement* msgElement = GetTransform()->GetChildByName(std::string("ToolTip Message"), true);
		m_pMessageText = msgElement->GetComponent<UIText>();
	}

	if (m_pVisualsElement == nullptr) {
		m_pVisualsElement = GetTransform()->GetChildByName(std::string("ToolTip Visuals"));
	}
	m_pVisualsElement->SetActive(false);

	// === Determine the Starting and Ending Locations for the Visuals
	Vector2 dimensions = Inferno_UI::GetCanvasByName(m_pVisualsElement->GetCanvasName())->GetTargetResolution();
	Vector2 parentPosition = m_pVisualsElement->GetTransform()->GetParent()->GetPosition();
	Vector2 bounds = m_pVisualsElement->GetTransform()->GetChildByName(std::string("ToolTip Background"))->GetTransform()->GetBounds();
	m_v2EndingPosition = Vector2(0.0f, 0.0f);
	m_v2StartingPosition = Vector2(((-dimensions.x * 0.5f) - parentPosition.x) + bounds.x, ((dimensions.y * 0.5f) - parentPosition.y) - bounds.y);
}

void ToolTipManager::OnDisable()
{
	UnregisterFromToolTipEvents();
}

void ToolTipManager::Update()
{
	if (m_eCurrentState == ToolTipStates::TT_FlyIn_State) {
		UpdateFlyInState();
	}
	else if (m_eCurrentState == ToolTipStates::TT_FadeOut_State) {
		UpdateFadeOutState();
	}
}
// ===================== //

// ===== Private Interface ===== //
void ToolTipManager::DisplayNextTip()
{
	m_eCurrentState = ToolTipStates::TT_FlyIn_State;

	if (m_QueuedTips.size() > 0) {
		// === Still more messages to Display
		m_stCurrentTip = m_QueuedTips.front();
		m_QueuedTips.pop();

		m_fcToolTipFlags.SetFlag((unsigned int)ToolTipFlags::TT_CurrentlyDisplaying_Flag, (unsigned int)true);
		m_eCurrentState = ToolTipStates::TT_FlyIn_State;
		m_pVisualsElement->GetTransform()->SetPosition(m_v2StartingPosition);
		m_pMessageBkgd->GetTransform()->SetScale(Vector2(STARTING_SCALE, STARTING_SCALE));
		m_pMessageLbl->GetTransform()->SetScale(Vector2(STARTING_SCALE, STARTING_SCALE));
		m_pMessageText->GetTransform()->SetScale(Vector2(STARTING_SCALE, STARTING_SCALE));
		m_pVisualsElement->SetActive(true);

		m_pMessageBkgd->SetColor(TOOLTIP_BKGD_COLOR);
		
		Color color = m_pMessageLbl->GetColor();
		color.Alpha = 1.0f;
		m_pMessageLbl->SetColor(color);

		m_pMessageText->SetText(m_stCurrentTip.Message);
		m_pMessageText->SetColor(m_stCurrentTip.Color);

		m_pDisplayTimer->SetTime(FLY_IN_TIME);
		m_pDisplayTimer->Reset();
		m_pDisplayTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&ToolTipManager::OnFlyInDone, this));
	}
	else {
		// === No more messages to Display
		m_fcToolTipFlags.SetFlag((unsigned int)ToolTipFlags::TT_CurrentlyDisplaying_Flag, (unsigned int)false);
		m_eCurrentState = ToolTipStates::TT_Default_State;
		m_pVisualsElement->SetActive(false);
	}
}

void ToolTipManager::QueueTip(ToolTip _tip)
{
	m_QueuedTips.push(_tip);

	if (!m_fcToolTipFlags.GetFlag((unsigned int)ToolTipFlags::TT_CurrentlyDisplaying_Flag)) {
		DisplayNextTip();
	}
}

void ToolTipManager::RegisterForToolTipEvents()
{
	EventSystem* pEventSystem = EventSystem::GetInstance();

	pEventSystem->RegisterForEvent("ToolTip_GuardTowers", this, BindEventFunction(&ToolTipManager::ToolTip_BaseGuardTowers, this));
	pEventSystem->RegisterForEvent("ToolTip_MainSpawners", this, BindEventFunction(&ToolTipManager::ToolTip_BaseSpawners, this));
	pEventSystem->RegisterForEvent("ToolTip_CPSpawners", this, BindEventFunction(&ToolTipManager::ToolTip_ControlPointSpawners, this));
	pEventSystem->RegisterForEvent("ToolTip_TowerUpgrades", this, BindEventFunction(&ToolTipManager::ToolTip_TowerUpgrades, this));
	pEventSystem->RegisterForEvent("ToolTip_BossHealth", this, BindEventFunction(&ToolTipManager::ToolTip_BossHealth, this));
	pEventSystem->RegisterForEvent("ToolTip_SpawnerUpgrade", this, BindEventFunction(&ToolTipManager::ToolTip_SpawnerUpgrade, this));
	pEventSystem->RegisterForEvent("ToolTip_BuffSpawner", this, BindEventFunction(&ToolTipManager::ToolTip_BuffSpawner, this));
}

void ToolTipManager::UnregisterFromToolTipEvents()
{
	EventSystem* pEventSystem = EventSystem::GetInstance();

	pEventSystem->UnregisterFromEvent("ToolTip_GuardTowers", this, BindEventFunction(&ToolTipManager::ToolTip_BaseGuardTowers, this));
	pEventSystem->UnregisterFromEvent("ToolTip_MainSpawners", this, BindEventFunction(&ToolTipManager::ToolTip_BaseSpawners, this));
	pEventSystem->UnregisterFromEvent("ToolTip_CPSpawners", this, BindEventFunction(&ToolTipManager::ToolTip_ControlPointSpawners, this));
	pEventSystem->UnregisterFromEvent("ToolTip_TowerUpgrades", this, BindEventFunction(&ToolTipManager::ToolTip_TowerUpgrades, this));
	pEventSystem->UnregisterFromEvent("ToolTip_BossHealth", this, BindEventFunction(&ToolTipManager::ToolTip_BossHealth, this));
	pEventSystem->UnregisterFromEvent("ToolTip_SpawnerUpgrade", this, BindEventFunction(&ToolTipManager::ToolTip_SpawnerUpgrade, this));
	pEventSystem->UnregisterFromEvent("ToolTip_BuffSpawner", this, BindEventFunction(&ToolTipManager::ToolTip_BuffSpawner, this));
}

void ToolTipManager::UpdateFlyInState()
{
	Vector2 displacement = m_v2EndingPosition - m_v2StartingPosition;
	float ratio = (m_pDisplayTimer->GetSetTimeSeconds() - m_pDisplayTimer->TimeRemaining()) / m_pDisplayTimer->GetSetTimeSeconds();

	m_pVisualsElement->GetTransform()->SetPosition(displacement * ratio + m_v2StartingPosition);

	float newScale = (ENDING_SCALE - STARTING_SCALE) * ratio + STARTING_SCALE;
	m_pMessageBkgd->GetTransform()->SetScale(Vector2(newScale, newScale));
	m_pMessageLbl->GetTransform()->SetScale(Vector2(newScale, newScale));
	m_pMessageLbl->GetTransform()->SetYPosition(-32.0f * newScale);
	m_pMessageText->GetTransform()->SetScale(Vector2(newScale, newScale));
}

void ToolTipManager::UpdateFadeOutState()
{
	Color color;

	// === Fade out the Background
	color = m_pMessageBkgd->GetColor();
	color.Alpha = color.Alpha - ((TOOLTIP_BKGD_COLOR.Alpha / TOOLTIP_FADEOUT_TIME) * Time::DeltaTime);
	m_pMessageBkgd->SetColor(color);

	// === Fade out the Lable
	color = m_pMessageLbl->GetColor();
	color.Alpha = color.Alpha - ((1.0f / TOOLTIP_FADEOUT_TIME) * Time::DeltaTime);
	m_pMessageLbl->SetColor(color);

	// === Fade out the Text
	color = m_pMessageText->GetColor();
	color.Alpha = color.Alpha - ((1.0f / TOOLTIP_FADEOUT_TIME) * Time::DeltaTime);
	m_pMessageText->SetColor(color);
}
// ============================= //

// ===== Tool Tip Events ===== //
void ToolTipManager::ToolTip_BaseGuardTowers(void* _pData)
{
	ToolTip tip;
	tip.Message = "All Guard Towers must be destroyed for the Main Base's Shield to go down.";
	tip.DisplayTime = 6.0f;

	QueueTip(tip);

	EventSystem::GetInstance()->UnregisterFromEvent("ToolTip_GuardTowers", this, BindEventFunction(&ToolTipManager::ToolTip_BaseGuardTowers, this));
}

void ToolTipManager::ToolTip_BaseSpawners(void* _pData)
{
	ToolTip tip;
	tip.Message = "Try destroying a Base's spawners to do more damage to a Base and it's Guard Towers.";
	tip.DisplayTime = 6.0f;

	QueueTip(tip);

	EventSystem::GetInstance()->UnregisterFromEvent("ToolTip_MainSpawners", this, BindEventFunction(&ToolTipManager::ToolTip_BaseSpawners, this));
}

void ToolTipManager::ToolTip_ControlPointSpawners(void* _pData)
{
	ToolTip tip;
	tip.Message = "All towers within a Control Point must be destroyed for the Spawner's shield to go down.";
	tip.DisplayTime = 6.0f;

	QueueTip(tip);

	EventSystem::GetInstance()->UnregisterFromEvent("ToolTip_CPSpawners", this, BindEventFunction(&ToolTipManager::ToolTip_ControlPointSpawners, this));
}

void ToolTipManager::ToolTip_TowerUpgrades(void* _pData)
{
	ToolTip tip;
	tip.Message = "You can upgrade a fully repaired tower to increase it's stats!";
	tip.DisplayTime = 6.0f;

	QueueTip(tip);

	EventSystem::GetInstance()->UnregisterFromEvent("ToolTip_TowerUpgrades", this, BindEventFunction(&ToolTipManager::ToolTip_TowerUpgrades, this));
}

void ToolTipManager::ToolTip_BossHealth(void* _pData)
{
	ToolTip tip;
	tip.Message = "The Boss's max health will increase each time it spawns, so the more it spawns, the more trouble it will be!";
	tip.DisplayTime = 6.0f;

	QueueTip(tip);

	EventSystem::GetInstance()->UnregisterFromEvent("ToolTip_BossHealth", this, BindEventFunction(&ToolTipManager::ToolTip_BossHealth, this));
}

void ToolTipManager::ToolTip_SpawnerUpgrade(void* _pData)
{
	ToolTip tip;
	tip.Message = "Need more troops? Upgrade your base's spawners to get larger waves and different types of minions!";
	tip.DisplayTime = 6.0f;

	QueueTip(tip);

	EventSystem::GetInstance()->UnregisterFromEvent("ToolTip_SpawnerUpgrade", this, BindEventFunction(&ToolTipManager::ToolTip_SpawnerUpgrade, this));
}

void ToolTipManager::ToolTip_BuffSpawner(void* _pData)
{
	ToolTip tip;
	tip.Message = "These Spawners will activate once the Icon is filled, spawning buffed enemies that are not a joke!";
	tip.DisplayTime = 6.0f;

	QueueTip(tip);

	EventSystem::GetInstance()->UnregisterFromEvent("ToolTip_BuffSpawner", this, BindEventFunction(&ToolTipManager::ToolTip_BuffSpawner, this));
}
// =========================== //

// ===== Events ===== //
void ToolTipManager::OnShowToolTips(void* _pData)
{
	bool value = *(static_cast<bool*>(_pData));

	// === Is it already at the same setting?
	if (m_fcToolTipFlags.GetFlag((unsigned int)ToolTipFlags::TT_ShowToolTips_Flag) == value) {
		return;
	}

	m_fcToolTipFlags.SetFlag((unsigned int)ToolTipFlags::TT_ShowToolTips_Flag, (unsigned int)value);

	value ? RegisterForToolTipEvents() : UnregisterFromToolTipEvents();
}

void ToolTipManager::OnTipDone()
{
	m_pDisplayTimer->SetTime(TOOLTIP_FADEOUT_TIME);
	m_pDisplayTimer->Reset();
	m_pDisplayTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&ToolTipManager::DisplayNextTip, this));

	m_eCurrentState = ToolTipStates::TT_FadeOut_State;
}

void ToolTipManager::OnFlyInDone()
{
	m_eCurrentState = ToolTipStates::TT_Default_State;

	m_pVisualsElement->GetTransform()->SetPosition(m_v2EndingPosition);
	m_pVisualsElement->GetTransform()->SetScale(Vector2(ENDING_SCALE, ENDING_SCALE));
	m_pMessageLbl->GetTransform()->SetYPosition(-32.0f);

	m_pDisplayTimer->SetTime(m_stCurrentTip.DisplayTime);
	m_pDisplayTimer->Reset();
	m_pDisplayTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&ToolTipManager::OnTipDone, this));
}
// ================== //