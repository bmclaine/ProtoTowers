#include "stdafx.h"
#include "BaseTower.h"

#include "../Core/WWise_IDs.h"
#include "EventSystem.h"
#include "TowerOptionsScript.h"
#include "UIElement.h"

// === Static Variables
TowerOptionsScript* BaseTower::s_pTowerOptions = nullptr;
unsigned int BaseTower::s_uiOptionsRefCounter = 0;

// ===== Constructor / Destructor ===== //
BaseTower::BaseTower(GameObject* _owner, Transform* _transform) : Entity(_owner, _transform)
{
	m_eType = TT_UNKNOWN;
	m_fStartingYRot = 0.0f;
	m_fDamageResistance = 1.0f;
	m_pDamageParticleSystem = nullptr;
	m_pHealthBar = nullptr;
	m_pRenderer = nullptr;
	m_bPlayerInteracting = false;
	m_uiUpgradeLevel = 0;

	m_pRepairTimer = new InfernoTimer();
	m_pRepairTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&BaseTower::OnRepairEnd, this));
}

BaseTower::~BaseTower()
{
	delete m_pRepairTimer;
}
// ==================================== //

// ===== Interface ===== //
void BaseTower::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	Entity::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "TowerType";
	Property* prop = data->GetProperty(propName);
	m_eType = (TowerTypes)atoi(prop->value.c_str());

	propName = "UpgradeLevel";
	prop = data->GetProperty(propName);
	if (prop != nullptr) {
		m_uiUpgradeLevel = (unsigned int)atoi(prop->value.c_str());
	}

	propName = "StartingYRot";
	prop = data->GetProperty(propName);
	m_fStartingYRot = (float)atoi(prop->value.c_str());

	propName = "Renderer";
	prop = data->GetProperty(propName);
	int id = atoi(prop->value.c_str());
	if (id > 0){
		m_pRenderer = (MeshRenderer*)objectMap.find((unsigned int)id)->second;
	}
}

void BaseTower::OnEnable()
{
	m_bPlayerInteracting = false;

	// === Handle the Damage Particle System
	m_pDamageParticleSystem = gameObject->GetComponentInChildren<ParticleSystem>();
	if (m_pDamageParticleSystem != nullptr) {
		m_pDamageParticleSystem->Pause(true);
	}

	// === Get the Tower's Health Bar, if there is one
	GameObject* foundObject = transform->GetChildByName(std::string("Health Bar"));
	if (foundObject != nullptr) {
		m_pHealthBar = foundObject->GetComponent<SpriteRenderer>();
		foundObject->SetActive(false, true);
	}

	// === Get the Tower's Idicator and set to the proper color
	foundObject = transform->GetChildByName(std::string("Indicator_Icon"));
	if (foundObject != nullptr) {
		SpriteRenderer* indicator = foundObject->GetComponent<SpriteRenderer>();
		indicator->SetColor(m_eTeam == Team::RED ? Color(1.0f, 0.0f, 0.0f, 1.0f) : Color(0.0f, 0.0f, 1.0f, 1.0f));
	}

	// === Handle the Tower Options, if this Tower deals with it
	if (m_eTeam == Team::BLUE && m_eType != TowerTypes::TT_MAINBASE) {
		if (s_pTowerOptions == nullptr) {
			UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Tower Options"), std::string("ExtendedHUD"));
			if (foundElement != nullptr) {
				s_pTowerOptions = foundElement->GetComponent<TowerOptionsScript>();
			}
		}

		++s_uiOptionsRefCounter;
	}

	// === IComponent OnEnable
	IComponent::OnEnable();
}

void BaseTower::OnDisable()
{
	// === Handle the Tower Options, if this Tower deals with it
	if (m_eTeam == Team::BLUE && m_eType != TowerTypes::TT_MAINBASE && s_uiOptionsRefCounter != 0) {
		--s_uiOptionsRefCounter;
		if (s_uiOptionsRefCounter == 0) {
			PRINT("BaseTower's TowerOptions Ref being set to nullptr ... \n");
			s_pTowerOptions = nullptr;
		}
	}

	// === Clear out any left over TakeDamage Delegates
	m_vTakeDamageFuncs.clear();

	Entity::OnDisable();
}

void BaseTower::TakeDamage(DamageInfo _info)
{
	if (m_fHealth <= 0.0f) {
		return;
	}

	// === Take the incoming Damage
	m_fHealth -= (_info.IncomingDamage * _info.TowerMultiplier * m_fDamageResistance);

	// === Play the Audio
	Inferno::PostAudioEventAtObjectPosition(DEFAULT_TURRET_TAKE_DAMAGE,gameObject);

	// === Handle displaying the smoke
	UpdateDamageParticles();

	// === Handle the Health Bar
	UpdateHealthBar();

	// === Update the TowerOptions, if it needs to
	if (m_bPlayerInteracting) {
		s_pTowerOptions->UpdatePrices(m_fHealth, m_fMaxHealth);
	}

	if (m_fHealth <= 0.0f) {
		Death();
	}

	size_t count = m_vTakeDamageFuncs.size();
	for (size_t i = 0; i < count; ++i) {
		m_vTakeDamageFuncs[i].Func(this);
	}
}

void BaseTower::Repair()
{
	// === Already Repaired?
	if (m_fHealth == m_fMaxHealth) {
		return;
	}

	m_fHealth = m_fMaxHealth;

	UpdateDamageParticles();
	UpdateHealthBar();

	m_pRepairTimer->SetTime(1.0f);
	m_pRepairTimer->Reset();

	ASSERT(s_pTowerOptions != nullptr, "No Tower Options Element was found in this scene.");

	s_pTowerOptions->GetUIElement()->SetActive(false);
	OpenTowerOptionsMenu();

	m_bPlayerInteracting = false;

	// === VFX
	GameObject* repairVFX = Inferno::Instantiate("RepairVFX");
	repairVFX->transform->SetPosition(transform->GetPosition());
	Inferno::Destroy(repairVFX, 1.5f);

	// === Audio
	Inferno::PostAudioEventAtObjectPosition(DEFAULT_TURRET_REPAIR, gameObject);
}

void BaseTower::Sell()
{
	Death();

	ASSERT(s_pTowerOptions != nullptr, "No Tower Options Element was found in this scene.");

	s_pTowerOptions->GetUIElement()->SetActive(false);

	m_bPlayerInteracting = false;

	// === VFX
	GameObject* sellVFX = Inferno::Instantiate("SellVFX");
	sellVFX->transform->SetPosition(transform->GetPosition());
	Inferno::Destroy(sellVFX, 1.5f);

	// === Audio
	Inferno::PostAudioEventAtObjectPosition(DEFAULT_TURRET_SELL, gameObject);
}

void BaseTower::OpenTowerOptionsMenu()
{
	if (s_pTowerOptions == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Tower Options"), std::string("ExtendedHUD"));
		if (foundElement != nullptr) {
			s_pTowerOptions = foundElement->GetComponent<TowerOptionsScript>();
		}
		ASSERT(s_pTowerOptions != nullptr, "No Tower Options Element was found in this scene.");
	}

	s_pTowerOptions->SetFocusedTower(this);
	s_pTowerOptions->GetUIElement()->SetActive(true);

	if (m_eType == TowerTypes::TT_SPAWNER) {
		s_pTowerOptions->SetupForMinionSpawner();
	}

	m_bPlayerInteracting = true;
}

void BaseTower::CloseTowerOptionsMenu()
{
#if (defined(_DEBUG)) 
	ASSERT(s_pTowerOptions != nullptr, "No Tower Options Element was found in this scene.");
#endif
	if (s_pTowerOptions == nullptr) {
		PRINT("Tried to close Tower Options, but it was Null ...");
		return;
	}

	s_pTowerOptions->GetUIElement()->SetActive(false);

	m_bPlayerInteracting = false;
}

void BaseTower::AddForOnTakeDamage(void* _callingObject, OnTakeDamageFunc _func)
{
	size_t count = m_vTakeDamageFuncs.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vTakeDamageFuncs[i].CallingObject == _callingObject) {
			// === Already Tracking
#if (defined(_DEBUG))
			ASSERT(false, "Attemped to track for a BaseTower's OnTakeDamage on the same Object multiple times.");
#endif
			return;
		}
	}

	TakeDamageEvent tde = { _callingObject, _func };
	m_vTakeDamageFuncs.push_back(tde);
}

void BaseTower::RemoveFromOnTakeDamage(void* _callingObject)
{
	size_t count = m_vTakeDamageFuncs.size();
	for (size_t i = 0; i < count; ++i) {
		if (m_vTakeDamageFuncs[i].CallingObject == _callingObject) {
			m_vTakeDamageFuncs.erase(m_vTakeDamageFuncs.begin() + i);
			return;
		}
	}
}
// ===================== //

// ===== Static Interface ===== //
std::string BaseTower::GetNameOfTower(TowerTypes _type)
{
	switch (_type) {
	case TowerTypes::TT_AUTOTURRET:
		return std::string("Auto Turret");
		break;
	case TowerTypes::TT_FREEZETOWER:
		return std::string("Freeze Tower");
		break;
	case TowerTypes::TT_LAZERTURRET:
		return std::string("Lazer Turret");
		break;
	case TowerTypes::TT_MORTAR:
		return std::string("Mortar");
		break;
	case TowerTypes::TT_POISONTOWER:
		return std::string("Poison Turret");
		break;
	default:
		return std::string("Unknown");
	}
}

unsigned int BaseTower::GetPriceOfTower(TowerTypes _type)
{
	switch (_type) {
	case TowerTypes::TT_AUTOTURRET:
		return 100;
		break;
	case TowerTypes::TT_FREEZETOWER:
		return 75;
		break;
	case TowerTypes::TT_LAZERTURRET:
		return 200;
		break;
	case TowerTypes::TT_MORTAR:
		return 250;
		break;
	case TowerTypes::TT_POISONTOWER:
		return 175;
		break;
	case TowerTypes::TT_SPAWNER:
		return 200;
	default:
		return 0;
	}
}

unsigned int BaseTower::GetUpgradePrice(TowerTypes _type)
{
	switch (_type) {
	case TowerTypes::TT_AUTOTURRET:
		return 500;
		break;
	case TowerTypes::TT_FREEZETOWER:
		return 450;
		break;
	case TowerTypes::TT_LAZERTURRET:
		return 400;
		break;
	case TowerTypes::TT_MORTAR:
		return 550;
		break;
	case TowerTypes::TT_POISONTOWER:
		return 500;
		break;
	case TowerTypes::TT_SPAWNER:
		return 500;
	default:
		return 0;
	}
}
// ============================ //

// ===== Protected Interface ===== //
IComponent& BaseTower::operator=(IComponent& _rhs)
{
	BaseTower* rhsBT = dynamic_cast<BaseTower*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

BaseTower& BaseTower::operator=(const BaseTower& _rhs)
{
	if (this != &_rhs) {
		m_eType = _rhs.m_eType;
		m_uiUpgradeLevel = _rhs.m_uiUpgradeLevel;
		m_fDamageResistance = _rhs.m_fDamageResistance;

		Entity::operator=(_rhs);
	}

	return *this;
}

void BaseTower::Death()
{
	// === Audio
	Inferno::PostAudioEventAtObjectPosition(DEFAULT_TURRET_DEATH,gameObject);
	
	// === Particle Effect
	GameObject* explosion = Inferno::Instantiate("Explosion Effect");
	Vector3 pos = transform->GetPosition(); pos.y += 1.0f;
	explosion->transform->SetPosition(pos);
	Inferno::Destroy(explosion, 3.0f);

	Inferno::Destroy(this->gameObject);

	Entity::Death();
}

void BaseTower::UpdateDamageParticles()
{
	if (m_pDamageParticleSystem != nullptr) {
		m_pDamageParticleSystem->Pause(false);

		// === Handle Particle Emmision Rate
		float hRatio = ((m_fHealth / m_fMaxHealth) - 1.0f) * -1.0f;
		m_pDamageParticleSystem->SetEmissionRate((unsigned int)(MAX_DAMAGE_PARTICLES * hRatio));

		// === Handle the Particle's Color
		float damageTaken = m_fHealth / 255.0f;
		m_pDamageParticleSystem->SetStartColor(Color(1.0f, damageTaken, damageTaken, 0.45f));
	}
}

void BaseTower::UpdateHealthBar()
{
	if (m_pHealthBar != nullptr) {
		m_pHealthBar->gameObject->SetActive(true);
		float hRatio = m_fHealth / m_fMaxHealth;
		m_pHealthBar->SetFillAmount(hRatio);
		if (hRatio < 0.25f) {
			m_pHealthBar->SetColor(Color(0.8f, 0.0f, 0.0f, 1.0f));
		}
		else if (hRatio < 0.5f) {
			m_pHealthBar->SetColor(Color(0.8f, 0.8f, 0.0f, 1.0f));
		}
		else {
			m_pHealthBar->SetColor(Color(0.0f, 0.8f, 0.0f, 1.0f));
		}
	}
}
// =============================== //

// ===== Private Events ===== //
void BaseTower::OnRepairEnd()
{
	m_pHealthBar->gameObject->SetActive(false);
}
// ========================== //

// ===== Collision Events ===== //
void BaseTower::OnCollisionEnter(Collider* _collider)
{
	if (_collider->IsTrigger()) {
		return;
	}

	// === Does this Tower deal with the Tower Options and is it the Player we are colliding with?
	if (m_eTeam == Team::BLUE && m_eType == TowerTypes::TT_SPAWNER && _collider->gameObject->GetTag() == "Player") {
		OpenTowerOptionsMenu();
	}
}

void BaseTower::OnCollisionExit(Collider* _collider)
{
	if (_collider->IsTrigger()) {
		return;
	}

	if (m_eTeam == Team::BLUE && m_eType == TowerTypes::TT_SPAWNER && _collider->gameObject->GetTag() == "Player") {
		CloseTowerOptionsMenu();
	}
}

void BaseTower::OnTriggerEnter(Collider* _collider)
{
	if (_collider->gameObject->GetTag() != "PlayerProj") {
		return;
	}

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
// ============================ //

// ===== Accessors ===== //
TowerTypes BaseTower::GetType() const
{
	return m_eType;
}

unsigned int BaseTower::GetUpgradeLevel() const
{
	return m_uiUpgradeLevel;
}
// ===================== //

// ===== Mutators ===== //
void BaseTower::SetTowerType(TowerTypes _type)
{
	m_eType = _type;
}

void BaseTower::SetStartingYRotation(float _yRotRadians)
{
	m_fStartingYRot = _yRotRadians;
}

void BaseTower::SetDamageResistance(float _resistance)
{
	m_fDamageResistance = _resistance;
}
// ==================== //