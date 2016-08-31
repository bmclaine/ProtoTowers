//Author: Zane Lewis
//
//Areas on the level that must be taken for the player to progress

#include "stdafx.h"
#include "ControlPoint.h"
#include "Player.h"
#include "../Components/TowerNode.h"
#include "../Components/BaseTower.h"
#include "../Components/AIEntity.h"
#include "../Components/Barrier.h"
#include "../Components/BarrierButton.h"
#include "AlertMessage.h"
#include "Components\ParticleSystem.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "EventSystem.h"
#include "HealthPickup.h"
#include "AlertMessage.h"
#include "MinionSpawner.h"
#include "../Core/PlayerController.h"
#include "Ability.h"
#include "StunGernadeAbility.h"
#include "MinionBuffAbility.h"
#include "ShieldAbility.h"
#include "WeaponBuffAbility.h"
#include "PlasmaBlaster.h"
#include "UIComponents\UICanvas.h"
#include "AbilityFlyIn.h"


//TODO:: Delete these
#include "Physics\Collider.h"
#include "GameObject.h"
#include "InfernoTime.h"
#include "Components\Transform.h"
#include "Components\SpriteRenderer.h"

int ControlPoint::m_iEnemyTurretLevel = 0;

ControlPoint::ControlPoint(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_bIsFriendly = false;
	m_bIsContested = false;
	m_bIsPlayerPresent = false;
	m_bCanSetWarning = true;
	m_bHasBeenCaptured = false;
	
	m_fCaptureProgress = ENEMY_CAPTURE;
	m_pBarrierButton = nullptr;
	m_pBarrier = nullptr;
	m_iLevel = 0;
	m_iChannel = -2;

	for (int i = 0; i < NUM_PARTICLE; i++)
	{
		m_pParticleSystem[i] = nullptr;
	}
	for (int i = 0; i < 25; i++)
	{
		m_pEnemiesOnPoint[i] = nullptr;
	}
	for (int i = 0; i < NUM_TOWER_NODES; i++)
	{
		m_pTowerNodes[i] = nullptr;
	}

	m_pTopIconRenderer = nullptr;
	m_pBottomIconRenderer = nullptr;
	m_pWarningRenderer = nullptr;
	m_pIconElement = nullptr;
	m_pIconWarningElement = nullptr;

	m_pEnemySpawner = nullptr;
	m_pHealthPickUp = nullptr;
	m_pAlertMessage = nullptr;
	m_pPlayerController = nullptr;
	m_pCaptureParticles = nullptr;

	m_pIconTop = new Texture();
	m_pIconTop->SetImage(Inferno::GetImage(L"IconControlPoint.dds"));
	m_pIconBottom = new Texture();
	m_pIconBottom->SetImage(Inferno::GetImage(L"IconControlPoint.dds"));
	m_pIconWarning = new Texture();
	m_pIconWarning->SetImage(Inferno::GetImage(L"IconPing.dds"));

	m_fMaxPingSize = 30.0f;
	m_fMinPingSize = 20.0f;

	m_fPingTime = 6.0f;

	m_iNumTowerNodes = -1;

}

ControlPoint::~ControlPoint()
{
	if (isEnabled()) {
		OnDisable();
	}

	delete m_pIconTop;
	delete m_pIconBottom;
	delete m_pIconWarning;
	delete m_pIconElement;
	delete m_pIconWarningElement;
}

IComponent& ControlPoint::operator=(IComponent& _rhs)
{
	ControlPoint* rhsBT = dynamic_cast<ControlPoint*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

ControlPoint& ControlPoint::operator= (const ControlPoint& _rhs)
{
	if (this != &_rhs) {
		m_fCaptureProgress = _rhs.m_fCaptureProgress;
		m_fcFlags = _rhs.m_fcFlags;
		m_bIsPlayerPresent = _rhs.m_bIsPlayerPresent;
		m_bIsFriendly = _rhs.m_bIsFriendly;
		m_bIsCapturable = _rhs.m_bIsCapturable;
		for (int i = 0; i < NUM_TOWER_NODES; i++)
		{
			m_pTowerNodes[i] = _rhs.m_pTowerNodes[i];
		}
		for (size_t i = 0; i < 25; i++)
		{
			m_pEnemiesOnPoint[i] = _rhs.m_pEnemiesOnPoint[i];
		}
		m_iLevel = _rhs.m_iLevel;
		m_iChannel = _rhs.m_iChannel;
		m_iNumTowerNodes = _rhs.m_iNumTowerNodes;
		m_itEnemyRespawnTimer = _rhs.m_itEnemyRespawnTimer;
		m_iEnemyTurretLevel = _rhs.m_iEnemyTurretLevel;
		m_bHasBeenCaptured = _rhs.m_bHasBeenCaptured;
		m_bIsContested = _rhs.m_bIsContested;
		for (int i = 0; i < NUM_ABILITIES; i++)
		{
			m_pAbilities[i] = _rhs.m_pAbilities[i];
		}

	}

	return *this;
}

void ControlPoint::Init()
{

}

void ControlPoint::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	// === Clear out any previous tower nodes
	for (int i = 0; i < NUM_TOWER_NODES; i++)
	{
		m_pTowerNodes[i] = nullptr;
	}

	std::string propName = "TowerNodesCount";
	Property* prop = data->GetProperty(propName);
	int towerNodeCount = atoi(prop->value.c_str());
	m_iNumTowerNodes = towerNodeCount;
	for (int i = 0; i < towerNodeCount; ++i){
		propName = "TowerNode" + std::to_string(i);
		prop = data->GetProperty(propName);
		int ID = atoi(prop->value.c_str());
		TowerNode* towerNode = (TowerNode*)objectMap.find((unsigned int)ID)->second;
		if (towerNode)
			AddTowerNode(towerNode);
	}

	propName = "ParticleCount";
	prop = data->GetProperty(propName);
	int particleCount = atoi(prop->value.c_str());
	for (int i = 0; i < particleCount; ++i){
		propName = "Particle" + std::to_string(i);
		prop = data->GetProperty(propName);
		int ID = atoi(prop->value.c_str());
		GameObject* particleObject = (GameObject*)objectMap.find((unsigned int)ID)->second;
		if (particleObject)
			SetParticleSystem(particleObject);
	}

	propName = "Barrier";
	prop = data->GetProperty(propName);
	int ID = atoi(prop->value.c_str());
	if (ID > 0){
		GameObject* barrierObj = (GameObject*)objectMap.find((unsigned int)ID)->second;
		if (barrierObj)
			SetBarrier(barrierObj);
	}

	propName = "Button";
	prop = data->GetProperty(propName);
	ID = atoi(prop->value.c_str());
	if (ID > 0){
		GameObject* buttonObj = (GameObject*)objectMap.find((unsigned int)ID)->second;
		if (buttonObj)
			SetBarrierButton(buttonObj);
	}

	propName = "Channel";
	prop = data->GetProperty(propName);
	m_iChannel = atoi(prop->value.c_str());

	propName = "EnemySpawner";
	prop = data->GetProperty(propName);
	ID = atoi(prop->value.c_str());
	if (ID > 0){
		GameObject* _enemySpawner = (GameObject*)objectMap.find((unsigned int)ID)->second;
		if (_enemySpawner)
			SetEnemySpawner(_enemySpawner);
	}

	propName = "HealthPickUp";
	prop = data->GetProperty(propName);
	ID = atoi(prop->value.c_str());
	if (ID > 0){
		GameObject* _healthPickUp = (GameObject*)objectMap.find((unsigned int)ID)->second;
		if (_healthPickUp)
			SetHealthPickUp(_healthPickUp);
	}

	propName = "CaptureParticles";
	prop = data->GetProperty(propName);
	if (prop != nullptr)
	{
		ID = atoi(prop->value.c_str());
		if (ID > 0 )
		{
			m_pCaptureParticles = (ParticleSystem*)objectMap.find((unsigned int)ID)->second;
		}
	}

	int currAbility = 0;
	GameObject * player;
	propName = "Player";
	prop = data->GetProperty(propName);
	if (prop != nullptr)
	{
		ID = atoi(prop->value.c_str());
		if (ID > 0)
		{
			player = (GameObject*)objectMap.find((unsigned int)ID)->second;
			m_pAbilities[0] = (Ability*)player->GetComponent<StunGrenadeAbility>();
			m_pAbilities[1] = (Ability*)player->GetComponent<ShieldAbility>();
			m_pAbilities[2] = (Ability*)player->GetComponent<WeaponBuffAbility>();
			m_pAbilities[3] = (Ability*)player->GetComponent<MinionBuffAbility>();
			m_pPlayerController = player->GetComponent<PlayerController>();
			m_pPlayer = player;
		}
	}

}

void ControlPoint::OnEnable()
{
	if (m_pAlertMessage == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Alert Message"), std::string("ExtendedHUD"));
		ASSERT(foundElement != nullptr, "No Alert Message found. It's possible it's being added after this Control Point is enabling.");
		if (foundElement != nullptr) {
			m_pAlertMessage = foundElement->GetComponent<AlertMessage>();
		}
	}
	EventSystem::GetInstance()->RegisterForEvent("UpgradeControlPoint", this, BindEventFunction(&ControlPoint::BarrierDisableEvent, this));

	// === Get the Alert Messager
	if (m_pAlertMessage == nullptr) {
		UIElement* foundEle = Inferno_UI::GetUIElementByName(std::string("Alert Message"), std::string("ExtendedHUD"));
		if (foundEle != nullptr) {
			m_pAlertMessage = foundEle->GetComponent<AlertMessage>();
		}
	}

	// MiniMap Icon stuff
	if (m_pIconElement == nullptr)
	{
		m_pIconElement = new UIElement();
		m_pIconElement->SetName("ControlPointIcon");

		RectTransform* rectTransform = m_pIconElement->GetTransform();
		rectTransform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		rectTransform->SetBounds(Vector2(15.0f, 15.0f));

		// Bottom Icon, never changes.
		m_pBottomIconRenderer = m_pIconElement->AddComponent<UIRenderer>();
		m_pBottomIconRenderer->SetTexture(m_pIconBottom);
		m_pBottomIconRenderer->SetLayer(2);
		m_pBottomIconRenderer->SetEnabled(true);

		// Top Icon, radial fill, changes color based on team
		m_pTopIconRenderer = m_pIconElement->AddComponent<UIRenderer>();
		m_pTopIconRenderer->SetEffect(UIEffects::UI_FILL_RADIAL_CLOCKWISE);
		m_pTopIconRenderer->SetRadialOrigin(RadialOrigins::ORIGIN_TOP);
		m_pTopIconRenderer->SetTexture(m_pIconTop);
		m_pTopIconRenderer->SetLayer(3);
		m_pTopIconRenderer->SetEnabled(true);

		if (m_bIsFriendly == false)
			m_pTopIconRenderer->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		else
			m_pTopIconRenderer->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));

		m_sMapElement.element = m_pIconElement;
		m_sMapElement.transform = transform;
	}

	if (m_pBottomIconRenderer)
		m_pBottomIconRenderer->SetEnabled(true);

	if (m_pTopIconRenderer)
		m_pTopIconRenderer->SetEnabled(true);

	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_sMapElement);

	if (m_pIconWarningElement == nullptr)
	{
		m_pIconWarningElement = new UIElement();
		m_pIconWarningElement->SetName("ControlPointWarningIcon");

		m_pWarningRect = m_pIconWarningElement->GetTransform();
		m_pWarningRect->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		m_pWarningRect->SetBounds(Vector2(m_fMaxPingSize, m_fMaxPingSize));

		// Warning Icon
		m_pWarningRenderer = m_pIconWarningElement->AddComponent<UIRenderer>();
		m_pWarningRenderer->SetTexture(m_pIconWarning);
		m_pWarningRenderer->SetLayer(4);
		m_pWarningRenderer->SetEnabled(false);

		m_pWarningRenderer->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));

		m_sMapWarningElement.element = m_pIconWarningElement;
		m_sMapWarningElement.transform = transform;
	}

	if (m_pWarningRenderer)
		m_pWarningRenderer->SetEnabled(false);

	if (m_pCaptureParticles != nullptr)
	{
		m_pCaptureParticles->Pause(true);
		m_pCaptureParticles->SetLooping(false);
	}

	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_sMapWarningElement);

	m_itEnemyRespawnTimer.Reset();
	m_itEnemyRespawnTimer.SetTime(RESPAWN_TIME);
	m_itEnemyRespawnTimer.PauseTimer(true);

	m_iEnemyTurretLevel = 0;

	SpriteRenderer* spRend = transform->GetChildByName(std::string("ControlPointFloatingIcon"))->GetComponent<SpriteRenderer>();
	spRend->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));

	m_iLevel = 0;

	IComponent::OnEnable();
}

void ControlPoint::OnDisable()
{
	if (m_pBottomIconRenderer)
		m_pBottomIconRenderer->SetEnabled(false);

	if (m_pTopIconRenderer)
		m_pTopIconRenderer->SetEnabled(false);

	if (m_pWarningRenderer)
		m_pWarningRenderer->SetEnabled(false);

	m_bIsContested = false;

	m_pAlertMessage = nullptr;
	EventSystem::GetInstance()->UnregisterFromEvent("UpgradeControlPoint", this, BindEventFunction(&ControlPoint::BarrierDisableEvent, this));

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_sMapElement);
	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_sMapWarningElement);
	m_itEnemyRespawnTimer.Reset();
	m_itEnemyRespawnTimer.SetTime(RESPAWN_TIME);
	m_itEnemyRespawnTimer.PauseTimer(true);
	IComponent::OnDisable();
	m_iEnemyTurretLevel = 0;
}

IComponent* ControlPoint::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	ControlPoint* newComponent = new ControlPoint(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

void ControlPoint::Update()
{

	if (m_pParticleSystem != nullptr)
		transform->Rotate(Vector3(0.0f, Time::DeltaTime * 3.1415926f, 0.0f));
	bool _towerRemaining = false;
	for (int i = 0; i < m_iNumTowerNodes; i++)
	{
		if (m_pTowerNodes[i] && !m_pTowerNodes[i]->IsAvailble() && !m_pTowerNodes[i]->IsBuilding())
		{
			_towerRemaining = true;
		}
	}
	if (!_towerRemaining && !m_pEnemySpawner->GetComponent<MinionSpawner>()->GetVulnerable()
		)
	{
		m_pEnemySpawner->GetComponent<MinionSpawner>()->DeactivateShield();
	}
	if (m_pEnemySpawner->IsActive())
	{
		_towerRemaining = true;
	}
	if (!_towerRemaining)
	{
		Progress();

	}
	SpriteRenderer* spRend = transform->GetChildByName(std::string("Progress Bar"))->GetComponent<SpriteRenderer>();
	if (spRend)
	{
		if (m_fCaptureProgress < 0.0f)
		{
			spRend->SetColor(Color(.8f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			spRend->SetColor(Color(0.0f, 0.0f, 0.8f, 1.0f));
		}
		float ratio = abs(m_fCaptureProgress) / FRIENDLY_CAPTURE;
		spRend->SetFillAmount(ratio);
	}
	CleanUpEnemies();
	if (!m_bIsFriendly)
	{
		m_pHealthPickUp->SetActive(false);
	}

	UpdateMiniMapIcon();

	if (m_itWarningPing.IsFinished() == false)
	{
		UpdateMiniMapWarningIcon();
	}

	if (!m_bIsFriendly)
	{
		HandleRespawnTower();

	}
}

void ControlPoint::HandleRespawnTower()
{
	if (!m_bIsPlayerPresent)
	{
		m_itEnemyRespawnTimer.PauseTimer(false);
		for (int i = 0; i < m_iNumTowerNodes; i++)
		{
			m_pTowerNodes[i]->PauseBuildTimer(false);
		}
		if (m_bHasBeenCaptured &&m_itEnemyRespawnTimer.IsFinished())
		{
			RespawnTower();
		}
	}
	else
	{
		for (int i = 0; i < m_iNumTowerNodes; i++)
		{
			m_pTowerNodes[i]->PauseBuildTimer(true);
		}
	}
}

void ControlPoint::OnTriggerEnter(Collider * _col)
{
	if (!_col->IsTrigger())
	{
		if (_col->gameObject->GetComponent<Player>())
		{
			_col->gameObject->GetComponent<Entity>()->AddForTracking(this, BindEntityOnReleaseFunction(&ControlPoint::SetPlayerFalse, this));
			m_bIsPlayerPresent = true;
			EventSystem::GetInstance()->SendEventNow("SetControlPoint", this);
			return;
		}
		AIEntity* entity = _col->gameObject->GetComponent<AIEntity>();
		if (entity && entity->GetTeam() == RED)
		{
			for (int i = 0; i < 25; i++)
			{
				if (m_pEnemiesOnPoint[i] == nullptr)
				{
					m_pEnemiesOnPoint[i] = entity;
					return;
				}
			}
		}
	}
}

void ControlPoint::OnTriggerExit(Collider * _col)
{
	if (!_col->IsTrigger())
	{
		if (_col->gameObject->GetComponent<Player>())
		{
			_col->gameObject->GetComponent<Entity>()->RemoveFromTracking(this);
			EventSystem::GetInstance()->SendEventNow("SetControlPoint", nullptr);
			m_bIsPlayerPresent = false;
		}
		Entity* entity = _col->gameObject->GetComponent<Entity>();
		if (entity && entity->GetTeam() == RED)
		{
			for (int i = 0; i < 25; i++)
			{
				if (m_pEnemiesOnPoint[i] == entity)
				{
					m_pEnemiesOnPoint[i] = nullptr;
				}
			}
		}
	}
}



//DESCRIPTION: Controls the player's capture progress
//
//IN:void
//
//OUT:void
void ControlPoint::Progress()
{
	UpdateProgressBar();
}

//DESCRIPTION: Switches the owner of the control point
//
//IN: void
//
//OUT: void 
void ControlPoint::SwitchOwner()
{
	int newTeam = RED;
	for (int i = 0; i < m_iNumTowerNodes; i++)
	{
		if (m_pTowerNodes[i]->IsBuilding())
		{
			m_pTowerNodes[i]->CancelBuiltTower();
		}
	}
	SpriteRenderer* spRend = transform->GetChildByName(std::string("ControlPointFloatingIcon"))->GetComponent<SpriteRenderer>();
	if (m_pTowerNodes[0]->GetTeam() == RED)
	{
		Inferno::PostAudioEvent(CONTROL_POINT_CAPTURED);
		newTeam = BLUE;
		spRend->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
		m_pHealthPickUp->SetActive(true);
		if (m_pBarrierButton &&m_pBarrier->GetComponent<Barrier>()->GetActive())
		{
			m_pBarrierButton->GetComponent<BarrierButton>()->SetIsAvailable(true);
		}
		for (int i = 0; i < NUM_PARTICLE; i++)
		{
			m_pParticleSystem[i]->GetComponent<ParticleSystem>()->SetStartColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
			m_pParticleSystem[i]->GetComponent<ParticleSystem>()->SetEndColor(Color(0.0f, 0.0f, 1.0f, 1.0f));
		}
		if (!m_bHasBeenCaptured)
		{
			EventSystem::GetInstance()->SendEventNow("ControlPointCaptured", (void *)m_iChannel);
			GainAbility();
			m_bHasBeenCaptured = true;
		}
	}
	else
	{
		m_bIsContested = false;
		spRend->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));

		m_itEnemyRespawnTimer.Reset();
		m_itEnemyRespawnTimer.SetTime(RESPAWN_TIME);

		Inferno::PostAudioEvent(CONTROL_POINT_LOST);

		for (int i = 0; i < NUM_PARTICLE; i++)
		{
			m_pParticleSystem[i]->GetComponent<ParticleSystem>()->SetStartColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_pParticleSystem[i]->GetComponent<ParticleSystem>()->SetEndColor(Color(1.0f, 0.0f, 0.0f, 1.0f));

		}

	}
	for (int i = 0; i < m_iNumTowerNodes; i++)
	{
		if (m_pTowerNodes[i]) m_pTowerNodes[i]->SetTeam((Team)newTeam);
	}

	m_pTopIconRenderer->SetEnabled(true);
	
	SpawnParticles();
}

//DESCRIPTION:Adds a tower node to the array if there is a spot available
//
//IN:TowerNode * : tower node to be added
//
//OUT: bool : if the tower node was successfully added	
bool ControlPoint::AddTowerNode(TowerNode * _towerNode)
{
	for (int i = 0; i < m_iNumTowerNodes; i++)
	{
		if (m_pTowerNodes[i] == nullptr)
		{
			m_pTowerNodes[i] = _towerNode;
			m_pTowerNodes[i]->SetTeam(RED);
			return true;
		}
	}
	return false;
}

//DESCRIPTION: Translates the tower node at the passed in index
//
//IN: Vector3 : position to be set
//int : index of the tower node
//
//OUT: void
void ControlPoint::TranslateTowerNode(Vector3 translation, int _towerNodeIndex)
{
	if (m_pTowerNodes[_towerNodeIndex])
	{
		m_pTowerNodes[_towerNodeIndex]->transform->SetPosition(translation);
	}
}
//int - index of the tower node
//
//OUT: void
void ControlPoint::BuildTowerAtNode(int _type, int _index)
{
	if (m_pTowerNodes[_index])
		m_pTowerNodes[_index]->BuildTower((TowerTypes)_type);
}



//DESCRIPTION: removes dead enemies from tracking
//
//IN: void
//
//OUT: void
void ControlPoint::CleanUpEnemies()
{
	for (int i = 0; i < 25; i++)
	{
		if (m_pEnemiesOnPoint[i] && (m_pEnemiesOnPoint[i]->GetHealth() <= 0 || m_pEnemiesOnPoint[i]->IsDying()))
		{
			m_pEnemiesOnPoint[i] = nullptr;
		}
	}
}

void ControlPoint::SetParticleSystem(GameObject* particleSystem)
{
	for (int i = 0; i < NUM_PARTICLE; i++)
	{
		if (m_pParticleSystem[i] == nullptr)
		{
			m_pParticleSystem[i] = particleSystem;
			return;
		}
	}
}

void ControlPoint::SetBarrier(GameObject* barrier)
{
	m_pBarrier = barrier;
}

void ControlPoint::SetPlayerFalse(Entity * _entity)
{
	m_bIsPlayerPresent = false;
}

void ControlPoint::BuildRandomTowerInRange(int _index, int _type1, int _type2)
{
	if (m_pTowerNodes[_index])
	{
		int random = rand() % 2;
		if (random)
		{
			m_pTowerNodes[_index]->BuildTower((TowerTypes)_type1);
		}
		else
		{
			m_pTowerNodes[_index]->BuildTower((TowerTypes)_type2);
		}

	}
}

void ControlPoint::RespawnTower()
{
	for (int i = 0; i < m_iNumTowerNodes; i++)
	{
		if (m_pTowerNodes[i] && m_pTowerNodes[i]->IsAvailble())
		{
			BuildRandomTowerInRange(i, TT_AUTOTURRET, TT_FREEZETOWER);
			m_itEnemyRespawnTimer.Reset();
			m_itEnemyRespawnTimer.SetTime(RESPAWN_TIME);
			break;
		}
	}
	
}

float ControlPoint::GetCaptureProgress()
{
	return m_fCaptureProgress / FRIENDLY_CAPTURE;
}

void ControlPoint::UpdateProgressBar()
{
	m_bIsContested = false;
	bool enemyPresent = false;
	for (int i = 0; i < 25; i++)
	{
		if (m_pEnemiesOnPoint[i])
		{
			enemyPresent = true;
			break;
		}
	}
	if (m_bIsPlayerPresent && enemyPresent)
	{
		//TODO::Find an audio clip for contested point
		//Inferno::PostAudioEvent();
		
	}
	else if (m_bIsPlayerPresent && m_fCaptureProgress < FRIENDLY_CAPTURE)
	{
		Inferno::PostAudioEvent(CONTROL_POINT_PLAYER_CAPTURING);
		m_fCaptureProgress += Time::DeltaTime;
		if (m_fCaptureProgress >= FRIENDLY_CAPTURE && !m_bIsFriendly)
		{
			m_bCanSetWarning = true;
			m_fCaptureProgress = FRIENDLY_CAPTURE;
			m_bIsFriendly = true;
			SwitchOwner();
		}
	}
	else if (enemyPresent && m_fCaptureProgress > ENEMY_CAPTURE)
	{
		m_bIsContested = true;

		if (m_bCanSetWarning == true)
		{
			m_bCanSetWarning = false;
			m_pWarningRenderer->SetEnabled(true);
			m_itWarningPing.SetTime(m_fPingTime);
			m_itWarningPing.SetOnTimerEndFunction(BindOnTimerEndFunction(&ControlPoint::EndWarning, this));

			bool pData;
			EventSystem::GetInstance()->SendEventNow("WarningEvent", &pData);
		}

		Inferno::PostAudioEvent(CONTROL_POINT_ENEMY_CAPTURING);
		m_fCaptureProgress -= Time::DeltaTime;

		// === Alert that Enemy is Capturing the Control Point
		if (m_pAlertMessage != nullptr && !m_pAlertMessage->IsDisplayingAMessage()) {
			m_pAlertMessage->SetFlashColors(Color(1.0f, 1.0f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f));
			m_pAlertMessage->DisplayMessage(std::string("The Enemy is Capturing the Control Point!"), 1.5f, true);
		}

		if (m_fCaptureProgress <= -2.5f && m_bIsFriendly)
		{
			m_bCanSetWarning = true;
			m_fCaptureProgress = ENEMY_CAPTURE;
			m_bIsFriendly = false;
			SwitchOwner();

			// === Alert that the Enemy has Captured the Control Point
			if (m_pAlertMessage != nullptr) {
				m_pAlertMessage->QueueMessage(std::string("The Enemy has Captured a Control Point!"), 2.5f, Color(0.8f, 0.8f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), true, true);
			}
		}
	}
}

void ControlPoint::UpdateMiniMapIcon()
{
	float fillRatio = GetCaptureProgress();

	if (fillRatio < 0.0f)
	{
		fillRatio = -1.0f * fillRatio;
		m_pTopIconRenderer->SetColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
		m_pTopIconRenderer->SetColor(Color(0.0f, 0.0f, 1.0f, 1.0f));

	m_pTopIconRenderer->SetImageFill(fillRatio);

	if (m_bIsContested == true)
	{
		FlashIcon();
	}
}

void ControlPoint::UpdateMiniMapWarningIcon()
{
	float lerpRatio = m_itWarningPing.DifferenceBetweenTheStartTimeAndTimePassedForBrian() / m_itWarningPing.GetSetTimeSeconds();
	float lerpValue = (m_fMinPingSize * (1.0f - lerpRatio) + (m_fMaxPingSize * lerpRatio));

	m_pWarningRect->SetBounds(Vector2(lerpValue, lerpValue));
}

void ControlPoint::EndWarning()
{
	m_pWarningRenderer->SetEnabled(false);
	m_pWarningRect->SetBounds(Vector2(m_fMaxPingSize, m_fMaxPingSize));
}

void ControlPoint::FlashIcon()
{
	if (m_itFlashTimer.IsFinished())
	{
		if (m_pTopIconRenderer->IsEnabled() == true)
			m_pTopIconRenderer->SetEnabled(false);
		else
			m_pTopIconRenderer->SetEnabled(true);

		m_itFlashTimer.SetTime(0.15f);
	}
}

void ControlPoint::LevelUp(int _level)
{
	if (m_iEnemyTurretLevel < _level)
	{
		m_iEnemyTurretLevel = _level;
		m_pAlertMessage->SetFlashColors(Color(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f));
		std::string message("Control Points Have Reached Level ");
		message = message + std::to_string(m_iLevel + 1);
		EventData data;
		data.n_team = 2;
		data.n_type = 0;
		EventSystem::GetInstance()->SendEventNow("UpgradeTowers", (void *)&data);
		m_pAlertMessage->DisplayMessage(message, 1.5f, true);
		
	}
	//TODO:: level up the towers and spawner
}


void ControlPoint::BarrierDisableEvent(void * _data)
{
	//int _channel = (int)_data;
	int level = (int)(_data);
	LevelUp(level);
	//if (m_iChannel == _channel)
	//{
	//	for (int i = 0; i < NUM_TOWER_NODES; i++)
	//	{
	//		BuildRandomTowerInRange(i, TowerTypes::TT_AUTOTURRET, TowerTypes::TT_FREEZETOWER);
	//	}
	//}
}

void ControlPoint::GainAbility()
{
	Ability* ability = nullptr;
	for (int i = 0; i < NUM_ABILITIES; i++)
	{
		ability = m_pPlayerController->GetAbility(i);
		if (ability == nullptr)
		{
			Inferno::PostAudioEvent(PLAYER_LEVEL_UP_1);
			m_pPlayer->GetComponentInChildren<PlasmaBlaster>()->CreateLevelUpParticles();
			m_pPlayerController->SetAbility(m_pAbilities[i],i);
			UIElement* abilityFlyIn = Inferno_UI::GetUIElementByName(std::string("AbilityFlyIn"), std::string("ExtendedHUD"));
			abilityFlyIn->GetComponent<AbilityFlyIn>()->DisplayAbilityIcon(i);
			break;
		}
	}
}

void ControlPoint::SpawnParticles()
{
	GameObject* particleObject = Inferno::GetNewGameObject();
	particleObject->SetName("ControlPointParticels");
	particleObject->transform->SetPosition(transform->GetPosition());
	ParticleSystem* particles = particleObject->AddComponent<ParticleSystem>();
	particles->SetShape(EMITTER_SHAPE_CONE);
	particles->SetStyle(EMITTER_STYLE_EDGE);
	particles->SetConeDimensions(7.0f, 7.0f, 30.0f);
	particles->SetEmissionRate(300);
	particles->SetDuration(2.0f);
	particles->SetLifetime(1.0f);
	particles->SetStartSpeed(10.0f);
	particles->SetStartSize(0.5f);
	particles->SetEndSize(0.2f);
	if (m_bIsFriendly)
	{
		particles->SetStartColor(Color(0, 0, 255, 1));
		particles->SetEndColor(Color(0, 0, 255, 1));
	}
	else
	{
		particles->SetStartColor(Color(1.0f,0, 0, 1));
		particles->SetEndColor(Color(1.0f,0, 0, 1));
	}
	particles->Play();
	Inferno::Destroy(particleObject, particles->GetDuration());
}

//Accessors and mutators
void ControlPoint::SetBarrierButton(GameObject * barrierButton)
{
	m_pBarrierButton = barrierButton;
}

void ControlPoint::SetChannel(int _channel)
{
	m_iChannel = _channel;
}

void ControlPoint::SetEnemySpawner(GameObject * _enemySpawner)
{
	m_pEnemySpawner = _enemySpawner;
}

void ControlPoint::SetHealthPickUp(GameObject *_healthPickUp)
{
	m_pHealthPickUp = _healthPickUp;
}

float ControlPoint::GetProgressRatio()
{
	float blueLerp = (m_fCaptureProgress + 2.5f) / 5;
	return blueLerp;
}

//end accessors and mutators