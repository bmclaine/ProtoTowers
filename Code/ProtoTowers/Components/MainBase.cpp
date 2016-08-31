#include "stdafx.h"
#include "MainBase.h"

#include "AlertMessage.h"
#include "EndGame.h"
#include "EventSystem.h"
#include "FadeAway.h"
#include "MinionSpawner.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "TowerNode.h"

// ===== Constructor / Destructor ===== //
MainBase::MainBase(GameObject* _gameObject, Transform* _transform) : BaseTower(_gameObject, _transform)
{
	m_eType = TowerTypes::TT_MAINBASE;
	m_fMaxHealth = m_fHealth = 500.0f;
	m_pFadeAwayEffect = nullptr;
	m_pHealthBar = nullptr;
	m_pAlertMessage = nullptr;

	m_pRenderer = nullptr;
	m_pIconRenderer = nullptr;
	m_pIconElement = nullptr;

	m_pIcon = new Texture();
	m_pIcon->SetImage(Inferno::GetImage(L"IconMainBase.dds"));

	m_pElementUnderAttackPing = nullptr;
	m_pRendererUnderAttackPing = nullptr;
	m_pTransformUnderAttackPing = nullptr;

	m_pIconUnderAttackPing = new Texture();
	m_pIconUnderAttackPing->SetImage(Inferno::GetImage(L"IconPing.dds"));

	m_usTotalSpawners = m_usActiveSpawners = 0;

	m_fMaxPingSize = 30.0f;
	m_fMinPingSize = 20.0f;

	m_fPingTime = 6.0f;

	m_bUnderAttackIsPingable = true;
}

MainBase::~MainBase()
{
	if (isEnabled()) {
		OnDisable();
	}

	// === Clean up the Memory
	delete m_pIconElement;
	delete m_pElementUnderAttackPing;
	delete m_pIcon;
	delete m_pIconUnderAttackPing;
}
// ==================================== //

// ===== Interface ===== //
void MainBase::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	BaseTower::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;
	std::string propName;
	Property* prop;

	// === Determine how many Spawners there are
	propName = "SpawnerCount";
	prop = data->GetProperty(propName);
	if (prop)
		m_usActiveSpawners = m_usTotalSpawners = (unsigned short)atoi(prop->value.c_str());

	m_fDamageResistance = (m_usTotalSpawners > 0) ? 0.0f : 1.0f;

	// === Register for the ActivationStatus Event for the Spawners
	MinionSpawner* spawnerObject;
	unsigned int gameObjectID;
	for (unsigned short i = 0; i < m_usTotalSpawners; ++i) {
		propName = "Spawner_" + std::to_string(i);
		prop = data->GetProperty(propName);
		gameObjectID = (unsigned int)atoi(prop->value.c_str());
		spawnerObject = (MinionSpawner*)objectMap.find(gameObjectID)->second;
		spawnerObject->AddForOnActivationStatus(this, BindOnActivationStatusFunction(&MainBase::OnSpawnerActivationStatus, this));
	}

	// === Determine how many Guard Towers there are
	propName = "GuardNodeCount";
	prop = data->GetProperty(propName);
	int count = atoi(prop->value.c_str());

	// === Calculate the Health Portion of each Tower
	m_fGuardTowerHealthPortion = m_fMaxHealth * 0.7f;	// Towers make up 70% of the Base's Health
	m_fGuardTowerHealthPortion /= count;

	// === Store the Tower with their health portion
	TowerNode* nodeObject;
	for (int i = 0; i < count; ++i) {
		propName = "GuardNode_" + std::to_string(i);
		prop = data->GetProperty(propName);
		gameObjectID = (unsigned int)atoi(prop->value.c_str());
		nodeObject = (TowerNode*)objectMap.find(gameObjectID)->second;
		nodeObject->AddForOnBuildTower(this, BindOnTowerBuiltFunction(&MainBase::OnGuardTowerBuilt, this));
		m_vGuardNodes.push_back(nodeObject);
	}

	propName = "NextSceneName";
	prop = data->GetProperty(propName);
	if (prop)
		m_sNextSceneName = prop->value;
}

void MainBase::OnEnable()
{
	UIElement* element = nullptr;

	if (m_eTeam == Team::BLUE) {
		element = Inferno_UI::GetUIElementByName(std::string("Left Base HealthBar"), std::string("HUD"));
		if (element != nullptr) {
			m_pHealthBar = element->GetComponent<UIRenderer>();
		}
	}
	else {
		element = Inferno_UI::GetUIElementByName(std::string("Right Base HealthBar"), std::string("HUD"));
		if (element != nullptr) {
			m_pHealthBar = element->GetComponent<UIRenderer>();
		}
	}

	if (m_pFadeAwayEffect == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Fade Away Effect"), std::string("ExtendedHUD"));
		if (foundElement != nullptr) {
			m_pFadeAwayEffect = foundElement->GetComponent<FadeAway>(true);
		}
	}

	if (m_pAlertMessage == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Alert Message"), std::string("ExtendedHUD"));
		if (foundElement != nullptr) {
			m_pAlertMessage = foundElement->GetComponent<AlertMessage>();
		}
	}

	// MiniMap Icon stuff
	if (m_pIconElement == nullptr)
	{
		m_pIconElement = new UIElement();
		m_pIconElement->SetName("MainBaseIcon");
		m_pIconRenderer = m_pIconElement->AddComponent<UIRenderer>();

		RectTransform* rectTransform = m_pIconElement->GetTransform();
		rectTransform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		rectTransform->SetBounds(Vector2(15.0f, 15.0f));

		float rot = transform->GetRotation().y;
		rectTransform->SetRotation(rot);

		m_pIconRenderer->SetTexture(m_pIcon);

		if (m_eTeam == Team::RED)
			m_pIconRenderer->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		else if (m_eTeam == Team::BLUE)
			m_pIconRenderer->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));

		m_pIconRenderer->SetLayer(2);

		m_pMapElement.element = m_pIconElement;
		m_pMapElement.transform = transform;
	}

	m_pIconRenderer->SetEnabled(true);
	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_pMapElement);

	CreateBaseUnderAttackPing();

	IComponent::OnEnable();
}

void MainBase::OnDisable()
{
	if (m_pIconRenderer)
		m_pIconRenderer->SetEnabled(false);

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_pMapElement);

	m_pFadeAwayEffect = nullptr;
	m_pAlertMessage = nullptr;
	m_pHealthBar = nullptr;

	ClearMiniMapPingEvents();

	BaseTower::OnDisable();
}

void MainBase::Update()
{
	UpdateBaseUnderAttackPing();
}

void MainBase::TakeDamage(DamageInfo _damageInfo)
{
	// === Is the Base able to Take Damage?
	if (m_vGuardTowers.size() > 0) {
		return;
	}

	// === Smoke Particles
	ParticleSystem* particle = gameObject->GetComponentInChildren<ParticleSystem>();
	if (particle != nullptr) {
		if (m_fHealth < m_fMaxHealth * 0.25f) { // 25% of the Base's max health
			// Red -> Black
			particle->SetStartColor(Color(1, 0, 0, 1));
			particle->SetEndColor(Color(0, 0, 0, 1));
			particle->Play();
		}
		else if (m_fHealth < m_fMaxHealth * 0.50f) { // 50% of the Base's max health
			// Orange
			particle->SetStartColor(Color(255, 165 / 255, 0, 1));
			particle->Play();
		}
		else if (m_fHealth < m_fMaxHealth * 0.75f) { // 25% of the base's max health
			particle->Play();
		}
	}

	// === Alert Message
	if (m_eTeam == Team::BLUE && m_pAlertMessage != nullptr) {
		m_pAlertMessage->SetFlashColors(Color(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_pAlertMessage->DisplayMessage(std::string("Main Base Under Attack!"), 1.5f, true, false, true);
		Inferno::PostAudioEvent(CONTROL_POINT_LOST);

		StartBaseUnderAttackPing();
	}

	BaseTower::TakeDamage(_damageInfo);

	UpdateHealthBar();
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& MainBase::operator=(IComponent& _rhs)
{
	MainBase* rhsMainBase = dynamic_cast<MainBase*>(&_rhs);
	if (rhsMainBase == nullptr) {
		return *this;
	}

	return operator=(*rhsMainBase);
}

MainBase& MainBase::operator=(const MainBase& _rhs)
{
	if (this != &_rhs) {
		BaseTower::operator=(_rhs);
	}

	return *this;
}

IComponent* MainBase::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	MainBase* newMainBase = new MainBase(_gameObject, _transform);
	*newMainBase = *this;

	return newMainBase;
}

void MainBase::UpdateHealthBar()
{
	if (m_pHealthBar != nullptr) {
		m_pHealthBar->SetImageFill(m_fHealth / m_fMaxHealth);
	}
	else {
		// === Didn't find the Health Bar before, try again
		UIElement* element = nullptr;
		if (m_eTeam == Team::BLUE) {
			element = Inferno_UI::GetUIElementByName(std::string("Left Base HealthBar"), std::string("HUD"));
			if (element != nullptr) {
				m_pHealthBar = element->GetComponent<UIRenderer>();
			}
		}
		else {
			element = Inferno_UI::GetUIElementByName(std::string("Right Base HealthBar"), std::string("HUD"));
			if (element != nullptr) {
				m_pHealthBar = element->GetComponent<UIRenderer>();
			}
		}
	}
}
// =============================== //

// ===== Private Interface ===== //
void MainBase::Death()
{
	GameObject* endGameObj = Inferno::Instantiate();
	EndGame* eg = endGameObj->AddComponent<EndGame>();
	eg->SetDelayTime(4.0f);
	eg->SetSceneToLoad(m_sNextSceneName);
	if (m_eTeam == Team::BLUE) {
		eg->SetWinningTeam(Team::RED);
	}
	else {
		if (m_sNextSceneName == "")
			eg->SetWinningTeam(Team::BLUE);
		else
			eg->SetWinningTeam(Team::NO_TEAM);
	}

 	if (m_pFadeAwayEffect != nullptr) {
		m_pFadeAwayEffect->SetEnabled(true);
		m_pFadeAwayEffect->SetFadeColor(0.0f, 0.0f, 0.0f, 1.0f);
		m_pFadeAwayEffect->SetFadeTime(4.0f);
	}

	size_t count = m_vGuardNodes.size();
	for (size_t i = 0; i < count; ++i) {
		m_vGuardNodes[i]->RemoveFromOnBuildTower(this);
	}

	BaseTower::Death();
}

// === CreateSpawnerUnderAttackPing
//  IN : void    
//  OUT : void
//
//  Creates and Registers Base Under Attack MiniMap Ping Event
void MainBase::CreateBaseUnderAttackPing()
{
	// Set Up Spawner Under Attack MiniMap Ping
	if (m_pElementUnderAttackPing == nullptr)
	{
		// Create Element
		m_pElementUnderAttackPing = new UIElement();
		m_pElementUnderAttackPing->SetName("SpawnerUnderAttackPingIcon");

		// Set Element Transform Data
		m_pTransformUnderAttackPing = m_pElementUnderAttackPing->GetTransform();
		m_pTransformUnderAttackPing->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		m_pTransformUnderAttackPing->SetBounds(Vector2(m_fMaxPingSize, m_fMaxPingSize));

		// Attach Renderer Component And Set Its Data
		m_pRendererUnderAttackPing = m_pElementUnderAttackPing->AddComponent<UIRenderer>();
		m_pRendererUnderAttackPing->SetTexture(m_pIconUnderAttackPing);
		m_pRendererUnderAttackPing->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		m_pRendererUnderAttackPing->SetLayer(4);
		m_pRendererUnderAttackPing->SetEnabled(false); // default to false, true when active

		// Set Up MiniMap Element
		m_sMapElementUnderAttackPing.element = m_pElementUnderAttackPing;
		m_sMapElementUnderAttackPing.transform = this->transform;
	}

	// If The Icon Boss Spawn Renderer Is Already Set, Make Sure It Isn't Enabled
	if (m_pRendererUnderAttackPing)
		m_pRendererUnderAttackPing->SetEnabled(false);

	// Register Boss Spawn Event With MiniMap
	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_sMapElementUnderAttackPing);
}

// === ClearMiniMapPingEvents
//  IN : void   
//  OUT : void
//
//  Clears MiniMap Ping Renderers
void MainBase::ClearMiniMapPingEvents()
{
	if (m_pRendererUnderAttackPing)
		m_pRendererUnderAttackPing->SetEnabled(false);

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_sMapElementUnderAttackPing);
}

// === UpdateBaseUnderAttackPing
//  IN : void
//  OUT : void
//
//  Updates Spawner Under Attack Ping If Active
void MainBase::UpdateBaseUnderAttackPing()
{
	if (m_itUnderAttackPingTimer.IsFinished() == true)
		return;

	float lerpRatio = m_itUnderAttackPingTimer.TimeRemaining() / m_itUnderAttackPingTimer.GetSetTimeSeconds();
	float lerpValue = (m_fMinPingSize * (1.0f - lerpRatio) + (m_fMaxPingSize * lerpRatio));

	m_pTransformUnderAttackPing->SetBounds(Vector2(lerpValue, lerpValue));
}

// === StartBaseUnderAttackPing
//  IN : void 
//  OUT : void
//
//  Starts The Base Under Attack MiniMap Ping Event
void MainBase::StartBaseUnderAttackPing()
{
	if (m_bUnderAttackIsPingable == true) {
		m_bUnderAttackIsPingable = false;
		m_pRendererUnderAttackPing->SetEnabled(true);
		m_itUnderAttackPingTimer.SetTime(m_fPingTime);
		m_itUnderAttackPingTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&MainBase::EndBaseUnderAttackPing, this));

		bool pData;
		EventSystem::GetInstance()->SendEventNow("WarningEvent", &pData);
	}
}

// === EndBaseUnderAttackPing
//  IN : void
//  OUT : void
//
//  Ends The Base Under Attack MiniMap Ping Event
void MainBase::EndBaseUnderAttackPing()
{
	m_bUnderAttackIsPingable = true;
	m_pRendererUnderAttackPing->SetEnabled(false);
	m_pTransformUnderAttackPing->SetBounds(Vector2(m_fMaxPingSize, m_fMaxPingSize));
}

void MainBase::OnGuardTowerBuilt(BaseTower* _tower)
{
	GuardTower newGuard;
	newGuard.Tower = _tower;
	newGuard.Tower->AddForTracking(this, BindEntityOnReleaseFunction(&MainBase::OnGuardTowerDestroyed, this));
	newGuard.Tower->AddForOnTakeDamage(this, BindOnTowerTakeDamage(&MainBase::OnGuardTowerDamaged, this));
	newGuard.Tower->SetDamageResistance(m_fDamageResistance);
	newGuard.HealthPortion = m_fGuardTowerHealthPortion;

	m_vGuardTowers.push_back(newGuard);

	UpdateBaseHealth();
}

void MainBase::OnGuardTowerDamaged(BaseTower* _tower)
{
	size_t count = m_vGuardTowers.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vGuardTowers[i].Tower == _tower) {
			float hRatio = _tower->GetHealth() / _tower->GetMaxHealth();
			m_vGuardTowers[i].HealthPortion = m_fGuardTowerHealthPortion * hRatio;
			UpdateBaseHealth();
			return;
		}
	}
}

void MainBase::OnGuardTowerDestroyed(Entity* _entity)
{
	size_t count = m_vGuardTowers.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vGuardTowers[i].Tower == _entity) {
			m_vGuardTowers.erase(m_vGuardTowers.begin() + i);
			UpdateBaseHealth();
			return;
		}
	}
}

void MainBase::OnSpawnerActivationStatus(bool _active)
{
	_active ? ++m_usActiveSpawners : --m_usActiveSpawners;

	// === Set the Damage Percent on all Guard Towers
	if (m_usActiveSpawners == 0) {
		m_fDamageResistance = 1.0f;
	}
	else {
		m_fDamageResistance = m_usActiveSpawners == m_usTotalSpawners ? 0.0f : 0.5f;
	}

	size_t count = m_vGuardTowers.size();
	for (size_t i = 0; i < count; ++i) {
		m_vGuardTowers[i].Tower->SetDamageResistance(m_fDamageResistance);
	}
}

void MainBase::UpdateBaseHealth()
{
	// === Get the Base Health
	m_fHealth = m_fMaxHealth * 0.3f;

	// === Add on the Health from the Towers
	size_t count = m_vGuardTowers.size();
	for (size_t i = 0; i < count; ++i) {
		m_fHealth += m_vGuardTowers[i].HealthPortion;
	}

	UpdateHealthBar();
}
// ============================= //

// ===== Collision Interface ===== //
void MainBase::OnTriggerEnter(Collider* _collider)
{
	if (_collider->gameObject->GetTag() != "PlayerProj") {
		return;
	}

	if (m_vGuardTowers.size() > 0) {
		// === Tool Tip 
		EventSystem::GetInstance()->SendEventNow("ToolTip_GuardTowers", nullptr);

		GameObject* popUp = Inferno::Instantiate("NoDamageAlert");
		popUp->transform->SetPosition(_collider->transform->GetPosition());
	}
	else {
		if (m_fDamageResistance == 0.5f) {
			// === Tool Tip 
			EventSystem::GetInstance()->SendEventNow("ToolTip_MainSpawners", nullptr);

			GameObject* popUp = Inferno::Instantiate("HalfDamageAlert");
			popUp->transform->SetPosition(_collider->transform->GetPosition());
		}
		else if (m_fDamageResistance == 0.0f) {
			// === Tool Tip 
			EventSystem::GetInstance()->SendEventNow("ToolTip_MainSpawners", nullptr);

			GameObject* popUp = Inferno::Instantiate("NoDamageAlert");
			popUp->transform->SetPosition(_collider->transform->GetPosition());
		}
	}

}
// =============================== //

// ===== Accessors ===== //

// === GetClosestGuardTower
//  IN : Vector3 - Position of the Entity / Object requesting the closest guard tower
//  OUT : Entity* - Closest Guard Tower
//
//  Finds the closest Guard Tower to the requesting object, and returns a pointer to it. Returns nullptr if there are no guard towers left.
Entity* MainBase::GetClosestGuardTower(Vector3 _position)
{
	BaseTower* closestGuard = nullptr;
	float shortestDist = FLT_MAX, currDist = 0.0f;

	size_t count = m_vGuardTowers.size();
	for (size_t i = 0; i < count; ++i) {
		MinionSpawner* spawner = m_vGuardTowers[i].Tower->gameObject->GetComponent<MinionSpawner>();
		if (spawner != nullptr)
			if (spawner->IsDeactivated())
				continue;

		currDist = (_position - m_vGuardTowers[i].Tower->transform->GetPosition()).LengthSq();
		if (currDist < shortestDist) {
			shortestDist = currDist;
			closestGuard = m_vGuardTowers[i].Tower;
		}
	}

	return closestGuard;
}
// ===================== //