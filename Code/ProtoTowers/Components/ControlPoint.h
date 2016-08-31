//Author: Zane Lewis
//
//Areas on the level that must be taken for the player to progress
#pragma once
#define ENEMY_CAPTURE -2.50f
#define FRIENDLY_CAPTURE 2.50f
#define NUM_TOWER_NODES 4
#define NUM_PARTICLE 8
#define RESPAWN_TIME 20.0f
#define MAX_CONTROL_POINT_LEVEL 1
#define NUM_ABILITIES 4

class AlertMessage;
class IComponent;
class TowerNode;
class AIEntity;
class BarrierButton;
class UIElement;
class UIRenderer;
class Texture;
class RectTransform;
class PlayerController;
class Ability;

class ControlPoint : public IComponent
{
private:
	struct MapElement
	{
		Transform* transform;
		UIElement* element;
	};

	struct EventData
	{
		unsigned int n_team;
		unsigned int n_type;
	};

	static int m_iEnemyTurretLevel;

	UIElement		*m_pIconElement;
	UIElement		*m_pIconWarningElement;
	RectTransform	*m_pWarningRect;
	UIRenderer		*m_pBottomIconRenderer;
	UIRenderer		*m_pTopIconRenderer;
	UIRenderer		*m_pWarningRenderer;
	Texture			*m_pIconBottom;
	Texture			*m_pIconTop;
	Texture			*m_pIconWarning;
	MapElement		 m_sMapElement;
	MapElement		 m_sMapWarningElement;

	//enemies on the point
	AIEntity * m_pEnemiesOnPoint[25];
	//States if the player is on the point
	bool m_bIsPlayerPresent;
	//Player capture progress
	float m_fCaptureProgress;
	//ping timer
	float m_fPingTime;
	//which side has the point
	bool m_bIsFriendly;
	//if the point is capturable
	bool m_bIsCapturable;
	//if the point is contested
	bool m_bIsContested;
	// if the warning is able to enable
	bool m_bCanSetWarning;
	//	if the Control Point has been captured before
	bool m_bHasBeenCaptured;
	//nodes at the point
	TowerNode* m_pTowerNodes[NUM_TOWER_NODES];
	//Particle System game object
	GameObject * m_pParticleSystem[NUM_PARTICLE];
	//Barrier button
	GameObject * m_pBarrierButton;
	//Barrier
	GameObject * m_pBarrier;
	//Enemy spawner
	GameObject * m_pEnemySpawner;
	//Health Pick up
	GameObject * m_pHealthPickUp;
	//Player
	GameObject * m_pPlayer;
	//Player controller
	PlayerController * m_pPlayerController;
	// Alert Message 
	AlertMessage* m_pAlertMessage;
	//Times when to rebuild an enemy turret
	InfernoTimer  m_itEnemyRespawnTimer;
	// Times when to flash the minimap icon red
	InfernoTimer m_itFlashTimer;
	// Timer for minimap warning ping
	InfernoTimer m_itWarningPing;
	//List of all abilities
	Ability * m_pAbilities[NUM_ABILITIES];
	//Particle System for when the control point is taken
	ParticleSystem * m_pCaptureParticles;
	//The level of the turrets and spawner
	int m_iLevel;
	//The channel that causes the control point to activate
	int m_iChannel;
	int m_iNumTowerNodes;
	// maximum size of warning icon
	float m_fMaxPingSize;
	// minimum size of warning icon
	float m_fMinPingSize;

	//DESCRIPTION: removes dead enemies from tracking
	//
	//IN: void
	//
	//OUT: void
	void CleanUpEnemies();

	//DESCRIPTION: Updates the progress bar above the control point
	//
	//IN:void
	//
	//OUT: void
	void UpdateProgressBar();

	//DESCRIPTION: Updates the minimap icon representing this control point
	//
	//IN: void
	//
	//OUT: void
	void UpdateMiniMapIcon();

	//DESCRIPTION: Updates the minimap warning icon when the control point is being taken
	//
	//IN: void
	//
	//OUT: void
	void UpdateMiniMapWarningIcon();

	//DESCRIPTION: Ends the minimap warning
	//
	//IN: void
	//
	//OUT: void
	void EndWarning();

	//DESCRIPTION: Flashes the Icon when the point is being taken
	//
	//IN: void
	//
	//OUT: void
	void FlashIcon();

	//
	//
	//
	//
	void HandleRespawnTower();

protected:
	virtual IComponent* CreateCopy(GameObject* const gameobject, Transform* const transform) override;

public:
	ControlPoint(GameObject* const gameObject, Transform* const transform);

	virtual ~ControlPoint()override;

	IComponent& operator=(IComponent&) override;
	ControlPoint& operator=(const ControlPoint& _rhs);
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void Init() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void Update() override;

	virtual void OnTriggerExit(Collider * _col) override;
	virtual void OnTriggerEnter(Collider * _col) override;


	//DESCRIPTION: Controls the player's capture progress
	//
	//IN:void
	//
	//OUT:void
	void Progress();

	//DESCRIPTION: Switches the owner of the control point
	//
	//IN: void
	//
	//OUT: void 
	void SwitchOwner();

	//DESCRIPTION: Adds a tower node to the array if there is a spot available
	//
	//IN:TowerNode * - tower node to be added
	//
	//OUT: bool - if the tower node was successfully added	
	bool AddTowerNode(TowerNode * _towerNode);

	//DESCRIPTION: Translates the tower node at the passed in index
	//
	//IN: Vector3 - position to be set
	//int - index of the tower node
	//
	//OUT: void
	void TranslateTowerNode(Vector3 translation, int _towerNodeIndex);

	//DESCRIPTION: Builds a tower at the specified node
	//
	//IN: TowerTypes - type of tower to build
	//int - index of the tower node
	//
	//OUT: void
	void BuildTowerAtNode(int _type, int _index);



	//DESCRIPTION: Sets isplayerpresent to false
	//
	//IN: void
	//
	//OUT: void
	void SetPlayerFalse(Entity * _entity);

	//DESCRIPTION: Build a tower of one of the two passed in types at the passed in index
	//
	//IN: int - index of the tower build node
	//int - tower type 1
	//int - tower type 2
	//
	//OUT: void
	void BuildRandomTowerInRange(int _index,int _type1, int _type2);
	
	//DESCRIPTION: At the end of the timer, respawns an enemy turret
	//
	//IN: void
	//
	//OUT: void
	void RespawnTower();

	float GetCaptureProgress();

	void SetBarrierButton(GameObject * _barrierButton);

	void SetChannel(int _channel);

	//DESCRIPTION: Increases the level of the control point, to a maximum of three
	//
	//IN: void
	//
	//OUT: void
	void LevelUp(int _level);

	void GainAbility();

	void SpawnParticles();

	//DESCRIPTION: Controls how a control point reacts to a barrier being lowered
	//
	//IN: void * - the channel of the barrier
	//
	//out: void
	void BarrierDisableEvent(void * _data);

	void SetParticleSystem(GameObject* particleSystem);

	void SetBarrier(GameObject* barrier);

	void SetEnemySpawner(GameObject * _enemySpawner);

	void SetHealthPickUp(GameObject* _healthPickUp);

	float GetProgressRatio();

	Team GetOwningTeam() const;
};