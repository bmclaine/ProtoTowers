#include "stdafx.h"
#include "Player.h"

#include "../Core/PlayerController.h"
#include "../InfernoUI.h"
#include "BaseWeapon.h"
#include "Components\SkinnedMeshRenderer.h"
#include "Components\Transform.h"
#include "EventSystem.h"
#include "FadeAway.h"
#include "GameObject.h"
#include "Resource.h"

// === Defines
#define DEATH_FADEOUT_TIME 2.5f
#define DEATH_FADEIN_TIME 1.5f
#define STARTING_RESOURCES 200
#define INVULNERABILITY_TIME 0.75f

// === Static Variables
unsigned int Player::s_uiResources = STARTING_RESOURCES;
Player* Player::s_pPlayer = nullptr;

// ===== Constructor / Destructor ===== //
Player::Player(GameObject* _owner, Transform* _transform) : Entity(_owner, _transform)
{
	m_fMaxHealth = m_fHealth = 150;
	m_pInvulnerabilityTimer.SetTime(INVULNERABILITY_TIME);
	m_pWeapon = nullptr;
	m_pFadeAwayEffect = nullptr;
	m_bGodMode = false;
	m_pSpawnLocation = nullptr;
	m_pSkinnedMeshRenderer = nullptr;
	s_pPlayer = this;
	m_visualDamageTimer.SetTime((unsigned int)255);
	m_bDamageTimerIsInitialized = false;
	m_pHealthBar = nullptr;
	m_pEnergyBar = nullptr;
	m_pEnergyOverheatIndicator = nullptr;
	m_HealthFlashTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&Player::FlashHealthBar, this));
	m_OverheatFlashTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&Player::FlashOverheatImage, this));

	m_bIsInvulnerable = false;

	m_pIconElement = nullptr;
	m_pIconRenderer = nullptr;

	m_pIcon = new Texture();
	m_pIcon->SetImage(Inferno::GetImage(L"IconPlayer.dds"));
}

Player::~Player()
{
	if (isEnabled()) {
		OnDisable();
	}

	delete m_pIcon;
	delete m_pIconElement;
}
// ==================================== //

// ===== Interface ===== //
void Player::Init()
{
	s_uiResources = STARTING_RESOURCES;
}

void Player::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	Entity::PostInit(objectMap, dataMap);

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "PlayerController";
	Property* prop = data->GetProperty(propName);
	int id = atoi(prop->value.c_str());
	if (id > 0){
		PlayerController* controller = (PlayerController*)objectMap.find((unsigned int)id)->second;
		SetPlayerController(controller);
	}

	propName = "SpawnLocation";
	prop = data->GetProperty(propName);
	id = atoi(prop->value.c_str());
	if (id > 0){
		m_pSpawnLocation = (Transform*)objectMap.find((unsigned int)id)->second;
	}

	propName = "BaseWeapon";
	prop = data->GetProperty(propName);
	id = atoi(prop->value.c_str());
	if (id > 0){
		m_pWeapon = (BaseWeapon*)objectMap.find((unsigned int)id)->second;
	}

	propName = "StartingResources";
	prop = data->GetProperty(propName);
	if (prop)
		s_uiResources = (UINT)atoi(prop->value.c_str());
}

void Player::OnEnable()
{
	// === Get / Setup the Weapon
	m_pWeapon = gameObject->GetComponentInChildren<BaseWeapon>();
	if (m_pWeapon != nullptr) {
		m_pWeapon->SetEnergyUpdateFunction(std::bind(&Player::UpdateEnergyBar, this, std::placeholders::_1));
	}

	// === Get the Spawn Location
	GameObject* SpawnLoc = Inferno::GetGameObjectByName(std::string("Player Spawn Location"));
	if (SpawnLoc != nullptr) {
		m_pSpawnLocation = SpawnLoc->transform;
	}

	// === Get the Meshes of the Player
	m_pSkinnedMeshRenderer = gameObject->GetComponentInChildren<SkinnedMeshRenderer>();
	GameObject* foundObject = transform->GetChildByName(std::string("PlayerHead(Mesh)"));
	if (foundObject != nullptr) {
		m_vExtraMeshes.push_back(foundObject->GetComponent<MeshRenderer>());
	}
	foundObject = transform->GetChildByName(std::string("PlayerRifle(Mesh)"));
	if (foundObject != nullptr) {
		m_vExtraMeshes.push_back(foundObject->GetComponent<MeshRenderer>());
	}

	// === Get the Fade Away Effect
	if (m_pFadeAwayEffect == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Fade Away Effect"), std::string("ExtendedHUD"));
		if (foundElement != nullptr) {
			m_pFadeAwayEffect = foundElement->GetComponent<FadeAway>(true);
		}
	}

	// === Get the HUD's Health Bar
	if (m_pHealthBar == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Player Health Bar"), std::string("HUD"));
		if (foundElement != nullptr) {
			m_pHealthBar = foundElement->GetComponent<UIRenderer>();
		}
	}

	// === Get the HUD's Energy Bar
	if (m_pEnergyBar == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Player Energy Bar"), std::string("HUD"));
		if (foundElement != nullptr) {
			m_pEnergyBar = foundElement->GetComponent<UIRenderer>();
			m_pEnergyOverheatIndicator = foundElement->GetComponentInChildren<UIRenderer>();
		}
	}

	// MiniMap Icon stuff
	if (m_pIconElement == nullptr)
	{
		m_pIconElement = new UIElement();
		m_pIconElement->SetName("PlayerIcon");
		m_pIconRenderer = m_pIconElement->AddComponent<UIRenderer>();

		RectTransform* rectTransform = m_pIconElement->GetTransform();
		rectTransform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		rectTransform->SetBounds(Vector2(10.0f, 10.0f));

		// float rot = transform->GetRotation().y;
		// rectTransform->SetRotation(ToRadians(rot));

		m_pIconRenderer->SetTexture(m_pIcon);

		// if (m_eTeam == Team::RED)
		// 	m_pIconRenderer->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		// else if (m_eTeam == Team::BLUE)
		// 	m_pIconRenderer->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));

		m_pIconRenderer->SetLayer(4);

		m_pMapElement.element = m_pIconElement;
		m_pMapElement.transform = transform;
	}

	m_pIconRenderer->SetEnabled(true);
	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_pMapElement);

	IComponent::OnEnable();
}

void Player::OnDisable()
{
	if (m_pIconRenderer)
		m_pIconRenderer->SetEnabled(false);

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_pMapElement);

	m_HealthFlashTimer.SetTime(0.0f);
	m_HealthFlashTimer.Reset();
	m_OverheatFlashTimer.SetTime(0.0f);
	m_OverheatFlashTimer.Reset();

	IComponent::OnDisable();
}

void Player::OnDestroy()
{
	m_pWeapon = nullptr;
	m_pFadeAwayEffect = nullptr;
	m_pSpawnLocation = nullptr;
	m_pSkinnedMeshRenderer = nullptr;
	m_pHealthBar = nullptr;
	m_pEnergyBar = nullptr;
	m_pEnergyOverheatIndicator = nullptr;
}

void Player::Update()
{
	if (CheatEntered(Cheat::TOGGLE_GODMODE)) {
		m_bGodMode = !m_bGodMode;

		UpdateHealthBar();
	}

	if (CheatEntered(Cheat::GIVE_RESOURCES)) {
		int numResourcesToSpawn = 5;
		for (int i = 0; i < numResourcesToSpawn; ++i)
		{
			Resource* temp = Inferno::Instantiate("Resource")->GetComponent<Resource>();
			Vector3 spawnPos = transform->GetPosition();
			spawnPos.y += 5;
			spawnPos.x += rand() % 5 - 2;
			spawnPos.z += rand() % 5 - 2;
			temp->transform->SetPosition(spawnPos);
		}
	}

	// Flash the player after taking damage
	FlashPlayer();
}

void Player::Shoot(int shotType)
{
	if (m_pWeapon != nullptr) {
		if (shotType == 0)
			m_pWeapon->FireProjectile(this);
		else
			m_pWeapon->FireSecondProjectile(this);
	}
}

void Player::TakeDamage(DamageInfo _damageInfo) {
	if (m_pInvulnerabilityTimer.IsFinished() && m_bGodMode == false && m_bIsInvulnerable == false && m_fHealth > 0.0f) {
		m_pInvulnerabilityTimer.Reset();
		m_visualDamageTimer.Reset();
		Inferno::PostAudioEventAtObjectPosition(PLAYER_TAKE_DAMAGE, gameObject);
		m_fHealth -= _damageInfo.IncomingDamage * _damageInfo.PlayerMultiplier;
		if (m_fHealth <= 0) {
			m_fHealth = 0.0f;
			Death();
		}

		UpdateHealthBar();
	}
}

void Player::ShootDebugProjectile()
{
	GameObject* debugProjectile = Inferno::Instantiate("DebugProjectile");
	debugProjectile->transform->SetRotation(transform->GetRotation());

	Vector3 pos = transform->GetPosition();
	pos.y += 1.0f;
	debugProjectile->transform->SetPosition(pos);

	debugProjectile->transform->Translate(Vector3(0.0f, 0.0f, -1.5f));
}

void Player::UpdateHealthBar()
{
	if (m_pHealthBar == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Player Health Bar"), std::string("HUD"));
		if (foundElement != nullptr) {
			m_pHealthBar = foundElement->GetComponent<UIRenderer>();
		}
	}

	if (m_pHealthBar != nullptr) {
		if (m_bGodMode) {
			m_pHealthBar->SetColor(Color(1.0f, 215.0f / 255.0f, 0.0f, 1.0f)); // Gold-ish
		}
		else {
			float healthRatio = m_fHealth / m_fMaxHealth;
			m_pHealthBar->SetImageFill(healthRatio);
			if (healthRatio > 0.5f) {
				m_pHealthBar->SetColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
				if (m_HealthFlashTimer.GetSetTimeSeconds() > 0.0f) {
					m_HealthFlashTimer.SetTime(0.0f);
				}
			}
			else if (healthRatio > 0.25f) {
				m_pHealthBar->SetColor(Color(1.0f, 1.0f, 0.0f, 1.0f));
				if (m_HealthFlashTimer.GetSetTimeSeconds() > 0.0f) {
					m_HealthFlashTimer.SetTime(0.0f);
				}
			}
			else {
				if (m_HealthFlashTimer.GetSetTimeSeconds() == 0.0f) {
					m_HealthFlashTimer.SetTime(0.25f);
					m_HealthFlashTimer.Reset();
					m_pHealthBar->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
				}
			}
		}
	}
}

void Player::FlashPlayer()
{
	if (m_bDamageTimerIsInitialized == false)
	{
		if (m_visualDamageTimer.IsFinished())
			m_bDamageTimerIsInitialized = true;
	}
	if (m_bDamageTimerIsInitialized)
	{
		float timeRemaining = m_visualDamageTimer.TimeRemaining() * 1000.0f;
		float time = 255.0f - timeRemaining;
		time /= 255.0f;

		if (m_pSkinnedMeshRenderer)
			m_pSkinnedMeshRenderer->SetColor(Color(1, time, time, 1));
	}
}
// ===================== //

// ===== Static Interface ===== //

// === AddResources
//  IN : unsigned int - Amount to add
//  OUT : void
//
//  Adds the given amount to the Player's Resource count.
void Player::AddResources(unsigned int _amount)
{
	s_uiResources += _amount;
	s_pPlayer->UpdateResources();

	if (s_uiResources > 600) {
		EventSystem::GetInstance()->SendEventNow("ToolTip_TowerUpgrades", nullptr);
	}
}

// === GetResources
//  IN : void
//  OUT : unsigned int - The current Resource count of the Player
//
//  Returns the amount of Resources that the Player currently has.
unsigned int Player::GetResources()
{
	return s_uiResources;
}

// === UseResources
//  IN : unsigned int - Amount to use
//  OUT : bool - Whether or not the Player had enough, and the amount was taken from the Player
//
//  Takes the given amount of resources away from the Player, if and only if they have enough. Returns true if the amount was taken out.
bool Player::UseResources(unsigned int _amount)
{
	if (HasEnoughResources(_amount)) {
		s_uiResources -= _amount;
		s_pPlayer->UpdateResources();
		Inferno::PostAudioEvent(RESOURCE_SPEND);
		return true;
	}
	else {
		return false;
	}
}

// === HasEnoughResources
//  IN : unsigned int - Amount to check against
//  OUT : bool - Whether or not the Player has enough Resources
//
//  Checks to see if the Player has enough resources compared to the amount specified, return true if they do, false otherwise.
bool Player::HasEnoughResources(unsigned int _amount)
{
	return (s_uiResources >= _amount);
}

void Player::AddForResourceTracking(void* _callingObject, ResourceUpdateFunc _func)
{
	if (s_pPlayer == nullptr) {
		return;
	}

	ResourceTrackingObject newRTrackingobj = { _callingObject, _func };
	s_pPlayer->m_vResourceTrackingObjects.push_back(newRTrackingobj);
}

void Player::RemoveFromResourceTracking(void* _callingObject)
{
	if (s_pPlayer == nullptr) {
		return;
	}

	size_t count = s_pPlayer->m_vResourceTrackingObjects.size();
	for (size_t i = 0; i < count; ++i) {
		if (s_pPlayer->m_vResourceTrackingObjects[i].Object == _callingObject) {
			if (count == 1) {
				s_pPlayer->m_vResourceTrackingObjects.erase(s_pPlayer->m_vResourceTrackingObjects.begin());
				break;
			}
			else {
				std::iter_swap(s_pPlayer->m_vResourceTrackingObjects.begin() + i, s_pPlayer->m_vResourceTrackingObjects.end() - 1);
				s_pPlayer->m_vResourceTrackingObjects.erase(s_pPlayer->m_vResourceTrackingObjects.end() - 1);
				break;
			}
		}
	}
}
// ============================ //

// === Collision Events === //
void Player::OnCollisionEnter(Collider* _collider)
{
	m_pPlayerController->SetCanSprint(false);
}

void Player::OnCollisionExit(Collider* _collider)
{
	m_pPlayerController->SetCanSprint(true);
}
// ======================== //

// === Protected Interface === //
IComponent& Player::operator=(IComponent& _rhs)
{
	Player* rhsPlayer = dynamic_cast<Player*>(&_rhs);
	if (rhsPlayer == nullptr) {
		return *this;
	}

	return operator=(*rhsPlayer);
}

Player& Player::operator=(const Player& _rhs)
{
	if (this != &_rhs) {
		m_pWeapon = _rhs.m_pWeapon;
		m_pPlayerController = _rhs.m_pPlayerController;
		m_pEnergyBar = _rhs.m_pEnergyBar;
		m_pHealthBar = _rhs.m_pHealthBar;
		m_vResourceTrackingObjects = _rhs.m_vResourceTrackingObjects;
		m_pInvulnerabilityTimer = _rhs.m_pInvulnerabilityTimer;
		m_RespawnTimer = _rhs.m_RespawnTimer;
		m_bGodMode = _rhs.m_bGodMode;
		m_visualDamageTimer = _rhs.m_visualDamageTimer;
		m_pSkinnedMeshRenderer = _rhs.m_pSkinnedMeshRenderer;
		m_bDamageTimerIsInitialized = _rhs.m_bDamageTimerIsInitialized;

		Entity::operator=(_rhs);
	}

	return *this;
}

IComponent* Player::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	Player* newComponenent = new Player(gameObject, transform);
	(*newComponenent) = *this;

	return newComponenent;
}
// =========================== //

// ===== Private Interface ===== //
void Player::Death()
{
	Inferno::PostAudioEventAtObjectPosition(PLAYER_DEATH, gameObject);

	EventSystem* pEventSystem = EventSystem::GetInstance();
	pEventSystem->SendEventNow("CloseTowerMenu", nullptr);
	bool data = false;
	pEventSystem->SendEventNow("ShowCrosshair", (void*)&data);

	m_pPlayerController->CancelRecall();

	m_RespawnTimer.SetTime(DEATH_FADEOUT_TIME);
	m_RespawnTimer.Reset();
	m_RespawnTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&Player::TeleportToSpawnLocation, this));

	m_HealthFlashTimer.SetTime(0.0f);

	m_pPlayerController->SetIgnoreInput(true);
	m_pPlayerController->SetState(PlayerController::PlayerState::DYING);
	this->SetEnabled(false);

	if (m_pFadeAwayEffect == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Fade Away Effect"), std::string("ExtendedHUD"));
		if (foundElement != nullptr) {
			m_pFadeAwayEffect = foundElement->GetComponent<FadeAway>(true);
		}
	}

	if (m_pFadeAwayEffect != nullptr) {
		m_pFadeAwayEffect->SetEnabled(true);
		m_pFadeAwayEffect->SetFadeTime(DEATH_FADEOUT_TIME, DEATH_FADEIN_TIME);
		m_pFadeAwayEffect->SetFadeColor(0.4f, 0.0f, 0.0f, 1.0f);
	}

	Entity::Death();
}

void Player::UpdateEnergyBar(float _energyLevel)
{
	if (m_pEnergyBar == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Player Energy Bar"), std::string("HUD"));
		if (foundElement != nullptr) {
			m_pEnergyBar = foundElement->GetComponent<UIRenderer>();
			m_pEnergyOverheatIndicator = foundElement->GetComponentInChildren<UIRenderer>();
		}
	}

	if (m_pEnergyBar != nullptr) {
		m_pEnergyBar->SetImageFill(_energyLevel);
		if (_energyLevel == 0.0f && m_pEnergyOverheatIndicator->GetColor().Alpha == 0.0f) {
			m_OverheatFlashTimer.SetTime(0.25f);
			m_OverheatFlashTimer.Reset();
			m_pEnergyOverheatIndicator->SetColor(Color(0.0f, 0.0f, 1.0f, 0.6f));
		}
		else if (_energyLevel >= 0.145f && m_pEnergyOverheatIndicator->GetColor().Alpha > 0.0f) {
			m_OverheatFlashTimer.SetTime(0.0f);
			m_pEnergyOverheatIndicator->SetColor(Color(1.0f, 1.0f, 1.0f, 0.0f));
		}
	}
}

void Player::UpdateResources()
{
	size_t count = m_vResourceTrackingObjects.size();
	for (size_t i = 0; i < count; ++i) {
		m_vResourceTrackingObjects[i].Func(s_uiResources);
	}
}

void Player::Respawn()
{
	// === Audio 
	Inferno::PostAudioEvent(PLAYER_RESPAWN);

	// === Reset Health
	m_fHealth = m_fMaxHealth;
	UpdateHealthBar();

	m_pWeapon->LevelDown();

	// === Bring back Input
	m_pPlayerController->SetIgnore(IGNORE_ALL, false);

	// === Respawn Cost
	unsigned int respawnCost = (100 > (unsigned int)(s_uiResources * 0.2f) ? 100 : (unsigned int)(s_uiResources * 0.2f));
	respawnCost = (respawnCost > s_uiResources ? s_uiResources : respawnCost);
	UseResources(respawnCost);

	// === Enable everything back
	m_pSkinnedMeshRenderer->SetEnabled(true);
	for (size_t i = 0; i < m_vExtraMeshes.size(); ++i) {
		m_vExtraMeshes[i]->SetEnabled(true);
	}
	this->SetEnabled(true);

	bool data = true;
	EventSystem::GetInstance()->SendEventNow("ShowCrosshair", (void*)&data);
}

void Player::TeleportToSpawnLocation()
{
	GameObject* SpawnLoc = Inferno::GetGameObjectByName(std::string("Player Spawn Location"));
	if (SpawnLoc != nullptr) {
		m_pSpawnLocation = SpawnLoc->transform;
	}
	if (ASSERT(m_pSpawnLocation != nullptr, "No Spawn Location. Make sure there's a GameObject with the name 'Player Spawn Location' in the Scene")) {
		transform->SetPosition(m_pSpawnLocation->GetPosition());
		transform->SetRotation(m_pSpawnLocation->GetRotation());

		m_RespawnTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&Player::Respawn, this));
		m_RespawnTimer.SetTime(DEATH_FADEIN_TIME);
		m_RespawnTimer.Reset();

		// === Handle the Animation
		m_pPlayerController->SetState(PlayerController::PlayerState::IDLE);

		m_pSkinnedMeshRenderer->SetEnabled(false);
		for (size_t i = 0; i < m_vExtraMeshes.size(); ++i) {
			m_vExtraMeshes[i]->SetEnabled(false);
		}
		this->SetEnabled(false);
		m_pPlayerController->CancelRecall();
	}
}

void Player::Recall(){

	GameObject* SpawnLoc = Inferno::GetGameObjectByName(std::string("Player Spawn Location"));
	if (SpawnLoc != nullptr) {
		m_pSpawnLocation = SpawnLoc->transform;
	}
	if (m_pSpawnLocation) {
		transform->SetPosition(m_pSpawnLocation->GetPosition());
		transform->SetRotation(m_pSpawnLocation->GetRotation());
	}

	m_pPlayerController->SetState(PlayerController::PlayerState::IDLE);

}

void Player::FlashHealthBar()
{
	if (m_pHealthBar == nullptr) {
		return;
	}

	m_HealthFlashTimer.Reset();
	Inferno::PostAudioEvent(PLAYER_LOW_HEALTH);
	if (m_pHealthBar->GetColor().Green == 0.0f) {
		// === Color is Red, switch to flash color
		m_pHealthBar->SetColor(Color(1.0f, 0.55f, 0.4f, 1.0f));
	}
	else {
		// === Color is flash color, switch to Red
		m_pHealthBar->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
}

void Player::FlashOverheatImage()
{
	if (m_pEnergyBar == nullptr) {
		return;
	}

	m_OverheatFlashTimer.Reset();
	if (m_pEnergyOverheatIndicator->GetColor().Green == 0.0f) {
		// === Color is Blue, change to grey
		m_pEnergyOverheatIndicator->SetColor(Color(0.5f, 0.5f, 0.5f, 0.6f));
	}
	else {
		// === Color is Grey, change to blue
		m_pEnergyOverheatIndicator->SetColor(Color(0.0f, 0.0f, 1.0f, 0.6f));
	}
}
// ========================= //

// ===== Mutators ===== //
bool Player::GetIsInvurnerable()
{
	return m_bIsInvulnerable;
}


// ===== Mutators ===== //
void Player::SetHealthBar(UIRenderer* _healthBar)
{
	m_pHealthBar = _healthBar;
}

void Player::SetEnergyBar(UIRenderer* _energyBar)
{
	m_pEnergyBar = _energyBar;
}

void Player::SetPlayerController(PlayerController* _pController)
{
	m_pPlayerController = _pController;
}

BaseWeapon* Player::GetWeapon() const{
	return m_pWeapon;
}


void Player::SetWeapon(BaseWeapon* _weapon)
{
	m_pWeapon = _weapon;
}

void Player::SetInvurnerable(bool _invurnerable)
{
	m_bIsInvulnerable = _invurnerable;
}

void Player::SetMeshColor(Color& _color, bool _extraMeshesIncluded)
{
	m_pSkinnedMeshRenderer->SetColor(_color);

	if (_extraMeshesIncluded) {
		size_t count = m_vExtraMeshes.size();
		for (size_t i = 0; i < count; ++i) {
			m_vExtraMeshes[i]->SetColor(_color);
		}
	}
}
// ==================== //