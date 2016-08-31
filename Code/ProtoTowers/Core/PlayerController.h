#pragma once
#include "Components\Controller.h"

#include "MathLib.h"
#include <vector>

#define MAX_ABILITIES 4

typedef unsigned int UINT;

class iInteractable;
class ThirdPersonCamera;
class InfernoTimer;
class Animator;
class Player;
class Ability;
class AbilityPanelScript;
class UIRenderer;
class PlayerRecall;

typedef enum DisableFlags {
	IGNORE_MOVEMENT = 1,
	IGNORE_SHOOTING = 2,
	IGNORE_CAMERA = 4,
	IGNORE_ALL = 7,
};

class PlayerController : public Controller
{

private: enum InputFlags
{
	MOVEMENT = 1,
	SHOOTING = 2,
	CAMERA = 4,
};

public: enum PlayerState
{
	IDLE = 0,
	RUNNING = 1,
	STRAFING_LEFT = 2,
	STRAFING_RIGHT = 3,
	FIRING = 4,
	DYING = 5,
	DASHING = 6,
	BUTTONPUSH = 7,
	THROWABILITY = 8,
	STATE_MAX 
};

private:
	// === Variables === //
	std::vector<InfernoTimer> m_fAnimTimers;

	float m_fRecallTimer;

	Vector3 m_f3Velocity;
	Vector3 m_vLastPosition;

	ThirdPersonCamera* m_pCamera;
	Animator* m_pAnimator;
	Player* m_pPossessedPawn;
	Ability* m_pAbilities[MAX_ABILITIES];
	iInteractable* m_pCurrentInteractable;
	InfernoTimer* m_pSprintDelayTimer;
	AbilityPanelScript* m_pAbilityPanel;
	UIRenderer* m_pRecallBar;
	PlayerRecall* m_pPlayerRecall;

	PlayerState m_eState;

	float m_fMovementSpeed;
	char m_cIgnoreInput;

	bool m_bMoving;
	bool m_bStrafing;
	bool m_bFiring;
	bool m_bDashing;
	bool m_bThrowing;
	bool m_bPushing;
	bool m_bRecalling;
	bool m_bPauseAnimations;
	bool m_bCanSprint;

	bool m_bIsSecondaryFire;
	// ================= //

	virtual ~PlayerController() override;

	// === Private Interface === //
	void OnSetRecieveInput(void* _pData);
	void OnCanSprint();
	// ========================= //

	void UpdateState();

	void IdleState();
	void RunState();
	void StrafeState();
	void FireState();
	void DeathState();
	void DashState();
	void ButtonPushState();
	void ThrowAbilityState();

	void Recalling();

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PlayerController& operator=(const PlayerController& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	PlayerController(GameObject* const gameObject, Transform* const transform);

	// === Interface === //
	virtual void Init();
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void OnDestroy() override;
	virtual void Update() override;
	virtual void FixedUpdate() override;

	void ForwardMovement(float multiplier);
	void StrafeMovement(float multiplier);
	void DashMovement(float multiplier);
	void MouseMovement(POINT _cursorPos, POINT _displacement);
	void VerticalLook(float multiplier);
	void HorizontalLook(float multiplier);
	void ToggleDash(ACTION_STATUS status);
	void Shoot(float _axisValue);
	void SecondaryShot(float _axisValue);
	void AbilityOne(ACTION_STATUS);
	void AbilityTwo(ACTION_STATUS);
	void AbilityThree(ACTION_STATUS);
	void AbilityFour(ACTION_STATUS);
	void Interact(ACTION_STATUS _status);
	void DebugButton(ACTION_STATUS _status);
	void Recall(ACTION_STATUS _status);
	void SetState(PlayerState _state);
	void CancelRecall();
	// ================= //

	// === Accessors === //
	float GetMovementSpeed() const;
	Player* GetPossessedPawn() const;
	float GetStateTime(PlayerState _state);
	Ability* GetAbility(unsigned int ability) const;
	float GetAnimationTime(PlayerController::PlayerState);
	// ================= //

	// === Mutators === //
	void SetMovementSpeed(float _speed);
	void SetCamera(ThirdPersonCamera* _camera);
	void SetAnimator(Animator* _animator);
	void SetPossessedPawn(Player* _pawn);
	void SetIgnoreInput(bool _value);
	void SetIgnore(unsigned int _flag, bool _value);
	void SetAbility(Ability* ability, unsigned int slot);
	void SetAbilityActiveTime(float time, UINT slot);
	void SetCurrentInteractable(iInteractable* _interactableObject);
	void PauseAnimations(bool _value);
	void SetCanSprint(bool _value);
	// ================ //

	friend class GameObject;
};

