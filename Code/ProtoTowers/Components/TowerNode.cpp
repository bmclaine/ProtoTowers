#include "stdafx.h"
#include "TowerNode.h"

#include "../Core/PlayerController.h"
#include "BaseTower.h"
#include "EventSystem.h"
#include "InputManager.h"
#include "TowerBuildMenu.h"
#include "UIElement.h"

// === Defines
#define BUILD_TIME 4.5f

// === Static Variables
TowerBuildMenu* TowerNode::s_pBuildMenu = nullptr;
unsigned int TowerNode::s_uiPlayerAutoTurretLevel = 0;
unsigned int TowerNode::s_uiPlayerLazerTurretLevel = 0;
unsigned int TowerNode::s_uiPlayerFreezeTowerLevel = 0;
unsigned int TowerNode::s_uiPlayerMortarTowerLevel = 0;
unsigned int TowerNode::s_uiPlayerPoisonTurretLevel = 0;

// ===== Constructor / Destructor ===== //
TowerNode::TowerNode(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_uiEnemyTowerLevel = 0;
	m_bAvailable = true;
	m_bBuilding = false;
	m_eStartingTower = TowerTypes::TT_UNKNOWN;

	m_pOwnedTower = nullptr;
	m_pBuildPrompt = nullptr;
	m_pBuildIcon = nullptr;
	m_pBuildTimer = new InfernoTimer();
	m_pBuildTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&TowerNode::OnBuildTowerDone, this));
	m_pInteractingPlayer = nullptr;
	m_pPromptTexture = new Texture();
	m_pPromptTexture->SetImage(Inferno::GetImage(L"BuildPrompt_Key.dds"));
}

TowerNode::~TowerNode()
{
	SAFE_DELETE(m_pBuildTimer);
	delete m_pPromptTexture;
}
// ==================================== //

// ===== Static Interface ===== //
void TowerNode::UpgradeTowerType(TowerTypes _type)
{
	// === Increase the correct Tower Type
	switch (_type) {
	case TowerTypes::TT_AUTOTURRET:
		++s_uiPlayerAutoTurretLevel;
		break;
	case TowerTypes::TT_FREEZETOWER:
		++s_uiPlayerFreezeTowerLevel;
		break;
	case TowerTypes::TT_LAZERTURRET:
		++s_uiPlayerLazerTurretLevel;
		break;
	case TowerTypes::TT_MORTAR:
		++s_uiPlayerMortarTowerLevel;
		break;
	case TowerTypes::TT_POISONTOWER:
		++s_uiPlayerPoisonTurretLevel;
		break;
	default:
		// === Not a Valid Type, just leave
		return;
	}

	// === Send out the Event to notify all TowerNodes of the upgrade
	struct EventData {
		unsigned int Team;
		unsigned int TowerType;
	};
	EventData data = { (unsigned int)Team::BLUE, (unsigned int)_type };
	EventSystem::GetInstance()->SendEventNow("UpgradeTowers", (void*)&data);
}
// ============================ //

// ===== Interface ===== //
void TowerNode::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Team";
	Property* prop = data->GetProperty(propName);
	m_eTeam = (Team)atoi(prop->value.c_str());

	propName = "StartingTower";
	prop = data->GetProperty(propName);
	m_eStartingTower = (TowerTypes)atoi(prop->value.c_str());

	propName = "IsAvailable";
	prop = data->GetProperty(propName);
	m_bAvailable = atoi(prop->value.c_str()) != 0;
}

void TowerNode::OnEnable()
{
	if (s_pBuildMenu == nullptr) {
		UIElement* element = Inferno_UI::GetUIElementByName(std::string("Tower Build Menu"), std::string("ExtendedHUD"));
		if (element != nullptr) {
			s_pBuildMenu = element->GetComponent<TowerBuildMenu>();
		}
	}

	if (m_eStartingTower != TowerTypes::TT_UNKNOWN && m_bAvailable) {
		m_bAvailable = false;
		m_eTowerToBuild = m_eStartingTower;
		OnBuildTowerDone();
	}

	if (m_pBuildPrompt == nullptr) {
		GameObject* foundObject = transform->GetChildByName(std::string("TN Build Prompt"));
		if (foundObject != nullptr) {
			m_pBuildPrompt = foundObject->GetComponent<SpriteRenderer>();
		}
	}

	if (m_pBuildPrompt != nullptr) {
		m_pBuildPrompt->gameObject->SetActive(false, true);
	}

	// === Register for Events
	EventSystem::GetInstance()->RegisterForEvent("UpgradeTowers", this, BindEventFunction(&TowerNode::OnUpgradeTowers, this));

	IComponent::OnEnable();
}

void TowerNode::OnDisable()
{
	s_pBuildMenu = nullptr;

	// === Unregister for Events
	EventSystem::GetInstance()->UnregisterFromEvent("UpgradeTowers", this, BindEventFunction(&TowerNode::OnUpgradeTowers, this));

	// === Clear out any left over OnBuildTower Delegates
	m_vOnBuildTowerFuncs.clear();

	IComponent::OnDisable();
}

void TowerNode::OnDestroy()
{
	// === Reset all the Upgrade Levels
	m_uiEnemyTowerLevel = 0;
	s_uiPlayerAutoTurretLevel = 0;
	s_uiPlayerLazerTurretLevel = 0;
	s_uiPlayerFreezeTowerLevel = 0;
	s_uiPlayerMortarTowerLevel = 0;
	s_uiPlayerPoisonTurretLevel = 0;

	if (m_pOwnedTower != nullptr) {
		m_pOwnedTower->RemoveFromTracking(this);
		m_pOwnedTower = nullptr;
	}
}

void TowerNode::Update()
{
	if (m_pBuildIcon != nullptr) {
		m_pBuildIcon->SetFillAmount((BUILD_TIME - m_pBuildTimer->TimeRemaining()) / BUILD_TIME);
	}
}

void TowerNode::Interact()
{
	if (s_pBuildMenu != nullptr) {
		s_pBuildMenu->GetUIElement()->SetActive(true);
		s_pBuildMenu->SetOwningNode(this);
		m_pBuildPrompt->gameObject->SetActive(false);
	}
}

void TowerNode::CloseMenu(void* _pData)
{
	m_bMenuOpen = false;

	if (m_pInteractingPlayer != nullptr) {
		m_pInteractingPlayer->SetCurrentInteractable(nullptr);
	}

	s_pBuildMenu->GetUIElement()->SetActive(false);
	m_pBuildPrompt->gameObject->SetActive(false);
	EventSystem::GetInstance()->UnregisterFromEvent("CloseTowerMenu", this, BindEventFunction(&TowerNode::CloseMenu, this));
}

void TowerNode::BuildTower(TowerTypes _type)
{
	if (!m_bAvailable)
		return;

	m_bAvailable = false;
	m_bBuilding = true;
	m_bMenuOpen = false;
	m_eTowerToBuild = _type;

	m_pBuildTimer->SetTime(BUILD_TIME);
	m_pBuildTimer->Reset();

	// === Spawn the Icon
	GameObject* buildIcon = Inferno::Instantiate("Build Icon");
	Vector3 pos = transform->GetPosition(); pos.y += 2.5f;
	buildIcon->transform->SetPosition(pos);

	m_pBuildIcon = buildIcon->GetComponent<SpriteRenderer>();
	m_pBuildIcon->SetFillAmount(0.0f);
}

void TowerNode::AddForOnBuildTower(void* _callingObject, OnBuildTowerFunc _func)
{
	size_t count = m_vOnBuildTowerFuncs.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vOnBuildTowerFuncs[i].CallingObject == _callingObject) {
			// === Already Tracking
			ASSERT(false, "Attempted to track a TowerNode's OnTowerBuild by the same Object multiple times.");
			return;
		}
	}

	TowerBuildEvent tbe = { _callingObject, _func };
	m_vOnBuildTowerFuncs.push_back(tbe);
}

void TowerNode::RemoveFromOnBuildTower(void* _callingObject)
{
	size_t count = m_vOnBuildTowerFuncs.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vOnBuildTowerFuncs[i].CallingObject == _callingObject) {
			m_vOnBuildTowerFuncs.erase(m_vOnBuildTowerFuncs.begin() + i);
			return;
		}
	}
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& TowerNode::operator=(IComponent& _rhs)
{
	TowerNode* rhsTN = dynamic_cast<TowerNode*>(&_rhs);
	if (rhsTN == nullptr) {
		return *this;
	}

	return operator=(*rhsTN);
}

TowerNode& TowerNode::operator=(const TowerNode& _rhs)
{
	if (this != &_rhs) {
		m_bAvailable = _rhs.m_bAvailable;
		m_eTeam = _rhs.m_eTeam;
		m_eStartingTower = _rhs.m_eStartingTower;
		m_fcFlags = _rhs.m_fcFlags;
	}

	return *this;
}

IComponent* TowerNode::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	TowerNode* newTowerNode = new TowerNode(_gameObject, _transform);
	*newTowerNode = *this;

	return newTowerNode;
}
// =============================== //

// ===== Private Interface ===== //
CollisionLayers TowerNode::GetLayerFromTeam() const
{
	if (m_eTeam == Team::BLUE) {
		return CollisionLayers::LAYER_FRIENDLY;
	}
	return CollisionLayers::LAYER_ENEMY;
}

void TowerNode::UpgradeTower()
{
	// === Disable and Destroy the current Tower
	m_pOwnedTower->RemoveFromTracking(this);
	m_pOwnedTower->gameObject->SetActive(false);
	Inferno::Destroy(m_pOwnedTower->gameObject);

	// === Spawn the Upgraded Tower
	m_eTowerToBuild = m_pOwnedTower->GetType();
	OnBuildTowerDone();

	// === Upgrade Effect
	GameObject* effect = Inferno::Instantiate("TowerUpgradeEffect");
	Vector3 pos = transform->GetPosition();
	pos.y += 0.15f;
	effect->transform->SetPosition(pos);
}

std::string TowerNode::GetTowerPrefabName(TowerTypes _type)
{
	std::string towerName;

	// === Get the Base Prefab Name
	switch (_type) {
	case TowerTypes::TT_AUTOTURRET:
		towerName = "AutoTurret";
		break;
	case TowerTypes::TT_FREEZETOWER:
		towerName = "Freeze Tower";
		break;
	case TowerTypes::TT_LAZERTURRET:
		towerName = "LazerTurret";
		break;
	case TowerTypes::TT_MORTAR:
		towerName = "MortarTower";
		break;
	case TowerTypes::TT_POISONTOWER:
		towerName = "PoisonTurret";
		break;
	}

	// === Account for the Upgrades
	char number[33];

	if (m_eTeam == Team::RED) {
		// === Team Red
		if (m_uiEnemyTowerLevel > 0) {
			towerName += "_";
			_itoa_s(m_uiEnemyTowerLevel, number, 33, 10);
			towerName += number;
		}

		return towerName;
	}
	else {
		// === Team Blue
		switch (_type) {
		case TowerTypes::TT_AUTOTURRET:
			if (s_uiPlayerAutoTurretLevel > 0) {
				towerName += "_";
				_itoa_s(s_uiPlayerAutoTurretLevel, number, 33, 10);
				towerName += number;
			}
			break;
		case TowerTypes::TT_FREEZETOWER:
			if (s_uiPlayerFreezeTowerLevel > 0) {
				towerName += "_";
				_itoa_s(s_uiPlayerFreezeTowerLevel, number, 33, 10);
				towerName += number;
			}
			break;
		case TowerTypes::TT_LAZERTURRET:
			if (s_uiPlayerLazerTurretLevel > 0) {
				towerName += "_";
				_itoa_s(s_uiPlayerLazerTurretLevel, number, 33, 10);
				towerName += number;
			}
			break;
		case TowerTypes::TT_MORTAR:
			if (s_uiPlayerMortarTowerLevel > 0) {
				towerName += "_";
				_itoa_s(s_uiPlayerMortarTowerLevel, number, 33, 10);
				towerName += number;
			}
			break;
		case TowerTypes::TT_POISONTOWER:
			if (s_uiPlayerPoisonTurretLevel > 0) {
				towerName += "_";
				_itoa_s(s_uiPlayerPoisonTurretLevel, number, 33, 10);
				towerName += number;
			}
			break;
		}

		return towerName;
	}
}

void TowerNode::OnTowerDestroyed(Entity* _entity)
{
	m_bAvailable = true;
	m_pOwnedTower = nullptr;
}


void TowerNode::OnUpgradeTowers(void* _pData)
{
	Team eTeam = (Team)*(static_cast<unsigned int*>(_pData));
	TowerTypes eType = (TowerTypes)*(static_cast<unsigned int*>(_pData)+1);

	if (m_eTeam != eTeam) {
		return;
	}

	if (eTeam == Team::RED) {
		// === Team Red, upgrade any tower, Type doesn't matter
		if (m_uiEnemyTowerLevel < 2) {
			++m_uiEnemyTowerLevel;
			if (m_pOwnedTower != nullptr) {
				UpgradeTower();
			}
		}
	}
	else {
		// === Team Blue, Type matters
		if (m_pOwnedTower != nullptr && m_pOwnedTower->GetType() == eType) {
			UpgradeTower();
		}
	}
}

void TowerNode::PauseBuildTimer(bool pause)
{
	if (m_pBuildTimer != nullptr)
	{
		m_pBuildTimer->PauseTimer(pause);
	}
}

void TowerNode::OnBuildTowerDone()
{
	// === Particles
	GameObject* particleObject = Inferno::GetNewGameObject();
	particleObject->SetName("BuildTowerParticles");
	particleObject->transform->SetPosition(transform->GetPosition());
	ParticleSystem* particles = particleObject->AddComponent<ParticleSystem>();
	particles->SetShape(EMITTER_SHAPE_CIRCLE);
	particles->SetStyle(EMITTER_STYLE_EDGE);
	particles->SetSphereDimensions(1.5f);
	particles->SetEmissionRate(200);
	particles->SetDuration(0.8f);
	particles->SetLifetime(1.0f);
	particles->SetStartSpeed(0.5f);
	particles->SetStartSize(0.15f);
	particles->SetEndSize(0.15f);
	particles->SetStartColor(Color(1, 1, 0, 1));
	particles->SetEndColor(Color(1, 0, 1, 1));
	particles->Play();
	Inferno::Destroy(particleObject, particles->GetDuration());

	GameObject* particleObjectCone = Inferno::GetNewGameObject();
	particleObjectCone->SetName("BuildTowerParticleShape");
	particleObjectCone->transform->SetPosition(transform->GetPosition());
	particles = particleObjectCone->AddComponent<ParticleSystem>();
	particles->SetShape(EMITTER_SHAPE_CONE);
	particles->SetStyle(EMITTER_STYLE_INNER);
	particles->SetConeDimensions(0.8f, 1.2f, 2.0f);
	particles->SetEmissionRate(200);
	particles->SetDuration(0.8f);
	particles->SetLifetime(1.0f);
	particles->SetStartSpeed(4.0f);
	particles->SetStartSize(0.15f);
	particles->SetEndSize(0.15f);
	particles->SetStartColor(Color(1, 1, 0, 1));
	particles->SetEndColor(Color(1, 0, 1, 1));
	particles->Play();
	Inferno::Destroy(particleObjectCone, particles->GetDuration());

	// === Audio
	Inferno::PostAudioEventAtObjectPosition(BUILD_TOWER, gameObject);

	// === Icon
	if (m_pBuildIcon != nullptr) {
		Inferno::Destroy(m_pBuildIcon->gameObject);
		m_pBuildIcon = nullptr;
	}

	m_pBuildTimer->SetTime(0.0f);
	m_pBuildTimer->Reset();
	m_bBuilding = false;

	// === Tower
	GameObject* tower = Inferno::Instantiate(GetTowerPrefabName(m_eTowerToBuild));
	tower->SetLayer((unsigned int)GetLayerFromTeam());
	tower->transform->SetPosition(0.0f, 0.0f, 0.0f);
	tower->transform->SetParent(transform);

	BaseTower* baseTower = tower->GetComponent<BaseTower>();
	baseTower->SetTeam(m_eTeam);
	baseTower->SetStartingYRotation(0.0f);
	baseTower->AddForTracking(this, BindEntityOnReleaseFunction(&TowerNode::OnTowerDestroyed, this));
	m_pOwnedTower = baseTower;

	size_t count = m_vOnBuildTowerFuncs.size();
	for (size_t i = 0; i < count; ++i) {
		m_vOnBuildTowerFuncs[i].Func(baseTower);
	}

}

void TowerNode::CancelBuiltTower()
{
	if (m_pBuildIcon != nullptr) {
		Inferno::Destroy(m_pBuildIcon->gameObject);
		m_pBuildIcon = nullptr;
	}
	m_pBuildTimer->SetTime(0.0f);
	m_pBuildTimer->Reset();
	m_bBuilding = false;
	m_bAvailable = true;
}
// ============================= //

// ===== Collision Events ===== //
void TowerNode::OnTriggerEnter(Collider* _collider)
{
	// === Is it a Friendly Node and a Player?
	if (m_eTeam == Team::BLUE && _collider->gameObject->GetTag() == "Player") {
		m_bMenuOpen = true;

		if (m_bAvailable) {
			// === Build Tower Menu
			if (s_pBuildMenu == nullptr) {
				UIElement* element = Inferno_UI::GetUIElementByName(std::string("Tower Build Menu"), std::string("ExtendedHUD"));
				if (element != nullptr) {
					s_pBuildMenu = element->GetComponent<TowerBuildMenu>();
				}
			}
			ASSERT(s_pBuildMenu != nullptr, "No Build Menu was found in the Scene.");

			m_pBuildPrompt->gameObject->SetActive(true);
			if (InputManager::UsingController()) {
				m_pPromptTexture->SetImage(Inferno::GetImage(L"BuildPrompt_Xbox.dds"));
			}
			else {
				m_pPromptTexture->SetImage(Inferno::GetImage(L"BuildPrompt_Key.dds"));
			}
			m_pBuildPrompt->SetImage(m_pPromptTexture->GetImage());

			m_pInteractingPlayer = _collider->gameObject->GetComponent<PlayerController>();
			m_pInteractingPlayer->SetCurrentInteractable(this);

			EventSystem::GetInstance()->RegisterForEvent("CloseTowerMenu", this, BindEventFunction(&TowerNode::CloseMenu, this));
		}
		else if (m_pOwnedTower != nullptr) {
			// === Tower Options
			m_pOwnedTower->OpenTowerOptionsMenu();
		}
	}
}

void TowerNode::OnTriggerStay(Collider* _collider)
{
	if (m_eTeam == Team::BLUE && m_bMenuOpen == false && _collider->gameObject->GetTag() == "Player") {
		if (m_bAvailable) {
			m_bMenuOpen = true;

			// === Build Tower Menu
			if (s_pBuildMenu == nullptr) {
				UIElement* element = Inferno_UI::GetUIElementByName(std::string("Tower Build Menu"), std::string("ExtendedHUD"));
				if (element != nullptr) {
					s_pBuildMenu = element->GetComponent<TowerBuildMenu>();
				}
			}
			ASSERT(s_pBuildMenu != nullptr, "No Build Menu was found in the Scene.");

			m_pBuildPrompt->gameObject->SetActive(true);
			if (InputManager::UsingController()) {
				m_pPromptTexture->SetImage(Inferno::GetImage(L"BuildPrompt_Xbox.dds"));
			}
			else {
				m_pPromptTexture->SetImage(Inferno::GetImage(L"BuildPrompt_Key.dds"));
			}
			m_pBuildPrompt->SetImage(m_pPromptTexture->GetImage());

			m_pInteractingPlayer = _collider->gameObject->GetComponent<PlayerController>();
			m_pInteractingPlayer->SetCurrentInteractable(this);

			EventSystem::GetInstance()->RegisterForEvent("CloseTowerMenu", this, BindEventFunction(&TowerNode::CloseMenu, this));
		}
		else if (m_pOwnedTower != nullptr) {
			// === Tower Options
			m_pOwnedTower->OpenTowerOptionsMenu();
			m_bMenuOpen = true;
		}
	}
}

void TowerNode::OnTriggerExit(Collider* _collider)
{
	if (m_eTeam == Team::BLUE && _collider->gameObject->GetTag() == "Player") {
		if (m_pOwnedTower == nullptr) {
			ASSERT(s_pBuildMenu != nullptr, "No Build Menu, it may have been deleted.");

			CloseMenu();
			m_pInteractingPlayer = nullptr;
		}
		else {
			m_pOwnedTower->CloseTowerOptionsMenu();
			m_bMenuOpen = false;
		}
	}
}
// ============================ //

// ===== Accessors ===== //
Team TowerNode::GetTeam() const
{
	return m_eTeam;
}

bool TowerNode::IsAvailble() const
{
	return m_bAvailable;
}

bool TowerNode::IsBuilding() const
{
	return m_bBuilding;
}
// ===================== //

// ===== Mutators ===== // 
void TowerNode::SetTeam(Team _team)
{
	if (m_eTeam != _team) {
		if (_team == Team::RED) {
			transform->Rotate(Vector3(0.0f, ToRadians(-180.0f), 0.0f));
		}
		else {
			transform->Rotate(Vector3(0.0f, ToRadians(180.0f), 0.0f));
		}
	}

	m_eTeam = _team;
}

void TowerNode::SetStartingTower(TowerTypes _type)
{
	m_eStartingTower = _type;
}
// ==================== //
