#include "stdafx.h"
#include "PlayerController.h"

#include "..\Components\3rdPersonCamera.h"
#include "Components\Animator.h"
#include "Assets\AnimationController.h"
#include "..\Components\Player.h"
#include "EventSystem.h"
#include "InfernoTimer.h"
#include "../Components/Ability.h"
#include "iInteractable.h"
#include "../InfernoUI.h"
#include "../Components/AbilityPanelScript.h"
#include "UIComponents\UIRenderer.h"
#include "Components\PlayerRecall.h"

// === Defines
#define MouseMovementMultiplier 0.2f
#define MAX_RECALL_TIME 1.5f
#define DASH_MULTIPLIER 2.0f
#define SPRINT_DELAY_TIME 0.15f

PlayerController::PlayerController(GameObject* const gameObject, Transform* const transform) : Controller(gameObject, transform)
{
	m_pAnimator = nullptr;
	m_pCurrentInteractable = nullptr;
	m_pAbilityPanel = nullptr;
	m_pRecallBar = nullptr;
	m_pPlayerRecall = nullptr;
	m_pSprintDelayTimer = new InfernoTimer();
	m_pSprintDelayTimer->SetOnTimerEndFunction(BindOnTimerEndFunction(&PlayerController::OnCanSprint, this));

	m_pAbilities[0] = nullptr;
	m_pAbilities[1] = nullptr;
	m_pAbilities[2] = nullptr;
	m_pAbilities[3] = nullptr;

	m_bMoving = false;
	m_bStrafing = false;
	m_bFiring = false;
	m_bDashing = false;
	m_bThrowing = false;
	m_bPushing = false;
	m_bRecalling = false;
	m_bPauseAnimations = false;

	m_bIsSecondaryFire = false;

	UICanvas* hudCanvas = nullptr;
	unsigned int hudCanvasSlot;

	m_f3Velocity = Vector3::Zero();

	m_fRecallTimer = MAX_RECALL_TIME;

	hudCanvas = Inferno_UI::GetCanvasByName("HUD");
	if (hudCanvas == nullptr) {
		Inferno_UI::CreateNewCanvas("HUD", &hudCanvas, &hudCanvasSlot);
	}

	m_cIgnoreInput = 0;
}

PlayerController::~PlayerController()
{
	m_pPlayerRecall = nullptr;
	delete m_pSprintDelayTimer;
}

void PlayerController::UpdateState()
{
	switch (m_eState)
	{
	case PlayerController::IDLE:
	{
		IdleState();
	}
	break;
	case PlayerController::RUNNING:
	{
		RunState();
	}
	break;
	case PlayerController::STRAFING_LEFT:
	{
		StrafeState();
	}
	break;
	case PlayerController::STRAFING_RIGHT:
	{
		StrafeState();
	}
	break;
	case PlayerController::FIRING:
	{
		FireState();
	}
	break;
	case PlayerController::DYING:
	{
		DeathState();
	}
	break;
	case PlayerController::DASHING:
	{
		DashState();
	}
	break;
	case PlayerController::BUTTONPUSH:
	{
		ButtonPushState();
	}
	break;
	case PlayerController::THROWABILITY:
	{
		ThrowAbilityState();
	}
	break;
	case PlayerController::STATE_MAX:
		break;
	default:
		break;
	}
}

void PlayerController::IdleState()
{

}

void PlayerController::RunState()
{
	// Vector3 currPosition = transform->GetPosition();
	// Vector3 displacement = currPosition - m_vLastPosition;

	if (m_bMoving == false/*displacement == Vector3::Zero()*/)
		SetState(PlayerState::IDLE);
}

void PlayerController::StrafeState()
{
	// Vector3 currPosition = transform->GetPosition();
	// Vector3 displacement = currPosition - m_vLastPosition;

	if (m_bStrafing == false/*displacement == Vector3::Zero()*/)
		SetState(PlayerState::IDLE);
}

void PlayerController::FireState()
{
	if (m_bFiring == false)
		SetState(PlayerState::IDLE);
}

void PlayerController::DeathState()
{

}

void PlayerController::DashState(){

	if (m_bDashing == false)
		SetState(PlayerState::IDLE);
}

void PlayerController::ButtonPushState()
{

}

void PlayerController::ThrowAbilityState()
{

}

void PlayerController::Recalling(){
	m_fRecallTimer += Time::DeltaTime;
	UIElement* element = Inferno_UI::GetUIElementByName(std::string("Recall BKG"), std::string("HUD"));

	if (m_pRecallBar == nullptr){
		if (element){
			m_pRecallBar = element->GetComponentInChildren<UIRenderer>();
		}
	}
	if (m_pRecallBar){
		m_pRecallBar->SetImageFill(m_fRecallTimer / MAX_RECALL_TIME);
	}
	if (m_fRecallTimer >= MAX_RECALL_TIME && m_bRecalling){
		if (element)
			element->SetActive(false);
		m_pPossessedPawn->Recall();
		m_bRecalling = false;
		SetIgnoreInput(false);
		if (m_pPlayerRecall)
			m_pPlayerRecall->StopRecall();
		GameObject* recallParticles = Inferno::Instantiate("FinishRecallParticles");
		recallParticles->transform->SetPosition(transform->GetPosition() + Vector3(0.0f, 3.5f, 0.0f));
		Inferno::Destroy(recallParticles, recallParticles->GetComponent<ParticleSystem>()->GetDuration());
	}
	else{
		if (element)
			element->SetActive(true);
	}
}

// === Protected Interface === //
IComponent& PlayerController::operator=(IComponent& _rhs)
{
	PlayerController* rhsPC = dynamic_cast<PlayerController*>(&_rhs);
	if (rhsPC == nullptr) {
		return *this;
	}

	return operator=(*rhsPC);
}

PlayerController& PlayerController::operator=(const PlayerController& _rhs)
{
	if (this != &_rhs) {
		m_fMovementSpeed = _rhs.m_fMovementSpeed;
		m_pCamera = _rhs.m_pCamera;
		m_pAnimator = _rhs.m_pAnimator;
		m_pPossessedPawn = _rhs.m_pPossessedPawn;
		m_cIgnoreInput = _rhs.m_cIgnoreInput;

		m_bPauseAnimations = _rhs.m_bPauseAnimations;
	}

	return *this;
}

IComponent* PlayerController::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	PlayerController* newComponent = new PlayerController(gameObject, transform);
	(*newComponent) = *this;

	return newComponent;
}

// ===== Private Interface ===== //
void PlayerController::OnSetRecieveInput(void* _pData)
{
	bool recieveInput = *((bool*)_pData);
	SetIgnore((unsigned int)IGNORE_ALL, !recieveInput);
}

void PlayerController::OnCanSprint()
{
	m_bCanSprint = true;
}

void PlayerController::Init()
{

}

void PlayerController::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "ThirdPersonCamera";
	Property* prop = data->GetProperty(propName);
	int ID = atoi(prop->value.c_str());
	if (ID != 0 && ID > 0)
		m_pCamera = (ThirdPersonCamera*)objectMap.find(ID)->second;

	propName = "Animator";
	prop = data->GetProperty(propName);
	ID = atoi(prop->value.c_str());
	if (ID != 0 && ID > 0)
		m_pAnimator = (Animator*)objectMap.find(ID)->second;

	propName = "Player";
	prop = data->GetProperty(propName);
	ID = atoi(prop->value.c_str());
	if (ID != 0 && ID > 0)
		m_pPossessedPawn = (Player*)objectMap.find(ID)->second;

	propName = "MovementSpeed";
	prop = data->GetProperty(propName);
	m_fMovementSpeed = (float)atof(prop->value.c_str());
}

void PlayerController::OnEnable()
{
	if (!m_pAbilityPanel){
		UIElement* AbilityPanel = new UIElement();
		AbilityPanel->SetName("Ability Panel");
		AbilityPanel->GetTransform()->SetPosition(Vector2(-465, -365));
		m_pAbilityPanel = AbilityPanel->AddComponent<AbilityPanelScript>();
		m_pAbilityPanel->SetPlayerController(this);

		Inferno_UI::AddToView(AbilityPanel, "HUD");
	}

	if (m_pAbilityPanel){
		m_pAbilityPanel->SetAbilityButtonTexture(Inferno::GetImage(L"UI_XboxBtn_LB.dds"), 0);
		m_pAbilityPanel->SetAbilityButtonTexture(Inferno::GetImage(L"UI_XboxBtn_RB.dds"), 1);
		m_pAbilityPanel->SetAbilityButtonTexture(Inferno::GetImage(L"UI_XboxBtn_X.dds"), 2);
		m_pAbilityPanel->SetAbilityButtonTexture(Inferno::GetImage(L"UI_XboxBtn_Y.dds"), 3);
	}

	RegisterForKeyEvent("Interact", this, BindActionFunction(&PlayerController::Interact, this));
	RegisterForAxisEvent("Forward", this, BindAxisFunction(&PlayerController::ForwardMovement, this));
	RegisterForAxisEvent("Right", this, BindAxisFunction(&PlayerController::StrafeMovement, this));
	RegisterForAxisEvent("RotationX", this, BindAxisFunction(&PlayerController::HorizontalLook, this));
	RegisterForAxisEvent("RotationY", this, BindAxisFunction(&PlayerController::VerticalLook, this));
	RegisterForAxisEvent("Shoot", this, BindAxisFunction(&PlayerController::Shoot, this));
	RegisterForAxisEvent("SecondaryShoot", this, BindAxisFunction(&PlayerController::SecondaryShot, this));
	RegisterForKeyEvent("Recall", this, BindActionFunction(&PlayerController::Recall, this));
	RegisterForKeyEvent("Dash", this, BindActionFunction(&PlayerController::ToggleDash, this));
	RegisterForKeyEvent("AbilityOne", this, BindActionFunction(&PlayerController::AbilityOne, this));
	RegisterForKeyEvent("AbilityTwo", this, BindActionFunction(&PlayerController::AbilityTwo, this));
	RegisterForKeyEvent("AbilityThree", this, BindActionFunction(&PlayerController::AbilityThree, this));
	RegisterForKeyEvent("AbilityFour", this, BindActionFunction(&PlayerController::AbilityFour, this));
	RegisterForMouseMovementEvent(this, BindMouseMovementFunction(&PlayerController::MouseMovement, this));

	AnimationController* animController = m_pAnimator->GetAnimationController();

	if (m_fAnimTimers.size() < STATE_MAX)
		animController->GetAnimationDurations(m_fAnimTimers);

	SetState(PlayerState::IDLE);

	UIElement* element = Inferno_UI::GetUIElementByName(std::string("Recall BKG"), std::string("HUD"));
	if (m_pRecallBar == nullptr && element){
		m_pRecallBar = element->GetComponentInChildren<UIRenderer>();
	}
	if (element)
		element->SetActive(false);

	GameObject* foundObject = Inferno::GetGameObjectByName(std::string("RecallParticles"));
	if (foundObject)
		m_pPlayerRecall = foundObject->GetComponent<PlayerRecall>();

	if (m_pPlayerRecall)
		m_pPlayerRecall->StopRecall();

	m_bCanSprint = true;

#if defined (_DEBUG) 
	RegisterForKeyEvent("Debug", this, BindActionFunction(&PlayerController::DebugButton, this));
#endif

	EventSystem::GetInstance()->RegisterForEvent("PlayerRecieveInput", this, BindEventFunction(&PlayerController::OnSetRecieveInput, this));
}

void PlayerController::OnDisable()
{
	UnregisterForKeyEvent("Interact", this, BindActionFunction(&PlayerController::Interact, this));
	UnregisterForAxisEvent("Forward", this, BindAxisFunction(&PlayerController::ForwardMovement, this));
	UnregisterForAxisEvent("Right", this, BindAxisFunction(&PlayerController::StrafeMovement, this));
	UnregisterForAxisEvent("RotationX", this, BindAxisFunction(&PlayerController::HorizontalLook, this));
	UnregisterForAxisEvent("RotationY", this, BindAxisFunction(&PlayerController::VerticalLook, this));
	UnregisterForAxisEvent("Shoot", this, BindAxisFunction(&PlayerController::Shoot, this));
	UnregisterForAxisEvent("SecondaryShoot", this, BindAxisFunction(&PlayerController::SecondaryShot, this));
	UnregisterForKeyEvent("Recall", this, BindActionFunction(&PlayerController::Recall, this));
	UnregisterForKeyEvent("Dash", this, BindActionFunction(&PlayerController::ToggleDash, this));
	UnregisterForKeyEvent("AbilityOne", this, BindActionFunction(&PlayerController::AbilityOne, this));
	UnregisterForKeyEvent("AbilityTwo", this, BindActionFunction(&PlayerController::AbilityTwo, this));
	UnregisterForKeyEvent("AbilityThree", this, BindActionFunction(&PlayerController::AbilityThree, this));
	UnregisterForKeyEvent("AbilityFour", this, BindActionFunction(&PlayerController::AbilityFour, this));
	UnregisterForMouseMovementEvent(this, BindMouseMovementFunction(&PlayerController::MouseMovement, this));

	for (int i = 0; i < 4; i++)
	{
		SetAbility(nullptr, i);
	}

#if defined (_DEBUG) 
	UnregisterForKeyEvent("Debug", this, BindActionFunction(&PlayerController::DebugButton, this));
#endif

	EventSystem::GetInstance()->UnregisterFromEvent("PlayerRecieveInput", this, BindEventFunction(&PlayerController::OnSetRecieveInput, this));
}

void PlayerController::OnDestroy()
{
	Controller::OnDestroy();

	// Since the Ability Panel is a UIElement it will get deleted between scenes. If the player controller is getting destroyed this means
	// that we are transitioning and we need to reset our pointer.
	m_pAbilityPanel = nullptr;
}

void PlayerController::Update()
{
	if (m_bRecalling)
		Recalling();
}

void PlayerController::FixedUpdate()
{
	UpdateState();

	m_bMoving = false;
	m_bStrafing = false;
	m_bFiring = false;
	m_bThrowing = false;
	m_bPushing = false;

	m_bIsSecondaryFire = false;

	//if (m_f3Velocity == Vector3::Zero())
	//	return;

	Vector3 movDir = Vector3::Zero();

	if (!(m_f3Velocity == Vector3::Zero()))
	{
		if (m_f3Velocity.LengthSq() > (m_fMovementSpeed * m_fMovementSpeed))
		{
			movDir = m_f3Velocity = m_f3Velocity.Norm();
			m_f3Velocity *= m_fMovementSpeed * (m_bDashing ? DASH_MULTIPLIER : 1) * Time::FixedDeltaTime;
		}
		else
		{
			movDir = m_f3Velocity.Norm();
			m_f3Velocity *= (m_bDashing ? DASH_MULTIPLIER : 1) * Time::FixedDeltaTime;
		}

		float dot = movDir * Vector3::Forward();
		if (abs(dot) > 0.8f)
		{
			if (m_bMoving == false && !(m_cIgnoreInput & MOVEMENT))
			{
				m_bMoving = true;

				SetState(PlayerState::RUNNING);
			}
		}
		else
		{
			if (m_bStrafing == false && !(m_cIgnoreInput & MOVEMENT))
			{
				m_bStrafing = true;

				if ((movDir * Vector3::Right()) < 0)
					SetState(PlayerState::STRAFING_LEFT);
				else
					SetState(PlayerState::STRAFING_RIGHT);
			}
		}

		transform->Translate(m_f3Velocity);
	}

	m_f3Velocity = Vector3::Zero();
}

void PlayerController::ForwardMovement(float multiplier)
{
	if (m_cIgnoreInput & MOVEMENT)
		return;

	//if (m_bDashing && m_eState != PlayerState::DASHING){
	//	SetState(PlayerState::DASHING);
	//}
	//else if (!m_bDashing && m_eState != PlayerState::RUNNING){
	//	m_bMoving = true;
	//	SetState(PlayerState::RUNNING);
	//}

	//if (m_bDashing == true && m_eState != PlayerState::DASHING)
	//	SetState(PlayerState::DASHING);
	//else if (m_bDashing == false && m_bMoving == false)
	//{
	//	m_bMoving = true;
	//	SetState(PlayerState::RUNNING);
	//}

	/*if (m_bMoving == false)
	{
	m_bMoving = true;
	SetState(PlayerState::RUNNING);
	}*/

	//float speed = m_bDashing ? m_fMovementSpeed * DASH_MULTIPLIER : m_fMovementSpeed;
	/*float speed = m_fMovementSpeed;
	if (m_bDashing && multiplier > 0.0f)
	speed *= DASH_MULTIPLIER;*/

	m_f3Velocity.z = m_fMovementSpeed * multiplier;
	//transform->Translate(Vector3(0, 0, m_fMovementSpeed * multiplier * Time::DeltaTime));
}

void PlayerController::StrafeMovement(float multiplier)
{
	if (m_cIgnoreInput & MOVEMENT)
		return;

	//if (m_bDashing && m_eState != PlayerState::DASHING){
	//	SetState(PlayerState::DASHING);
	//}
	//else if (!m_bDashing && m_eState != PlayerState::RUNNING){
	//	if (multiplier < 0.0f)
	//		SetState(PlayerState::STRAFING_LEFT);
	//	else if (multiplier > 0.0f)
	//		SetState(PlayerState::STRAFING_RIGHT);		m_bStrafing = true;
	//}

	/*if (m_bStrafing == false)
	{
	m_bStrafing = true;

	if (multiplier < 0.0f)
	SetState(PlayerState::STRAFING_LEFT);
	else if (multiplier > 0.0f)
	SetState(PlayerState::STRAFING_RIGHT);
	}*/

	//float speed = m_bDashing ? m_fMovementSpeed * DASH_MULTIPLIER : m_fMovementSpeed;
	/*float speed = m_fMovementSpeed;
	if (m_bDashing)
	speed *= DASH_MULTIPLIER;*/

	m_f3Velocity.x = m_fMovementSpeed * multiplier;
	//transform->Translate(Vector3(m_fMovementSpeed * multiplier * Time::DeltaTime, 0, 0));
}

void PlayerController::DashMovement(float multiplier){

	if ((m_cIgnoreInput & MOVEMENT) && !m_bCanSprint)
		return;

	if (!m_bDashing){
		m_bDashing = true;

	}
}

void PlayerController::MouseMovement(POINT _cursorPos, POINT _displacement)
{
	if (m_cIgnoreInput & CAMERA)
		return;
	m_pCamera->HorizontalRotate(ToRadians((float)_displacement.x * MouseMovementMultiplier));
	m_pCamera->VerticalRotate(ToRadians((float)_displacement.y * MouseMovementMultiplier));
}

void PlayerController::VerticalLook(float multiplier)
{
	if (m_cIgnoreInput & CAMERA)
		return;
	m_pCamera->VerticalRotate(ToRadians(120 * -multiplier * Time::DeltaTime));
}

void PlayerController::HorizontalLook(float multiplier)
{
	if (m_cIgnoreInput & CAMERA)
		return;
	m_pCamera->HorizontalRotate(ToRadians(140 * -multiplier * Time::DeltaTime));
}

void PlayerController::Shoot(float _axisValue)
{
	if ((m_cIgnoreInput & SHOOTING) || m_bIsSecondaryFire == true)
		return;

	m_bFiring = true;
	SetState(PlayerState::FIRING);

	if (_axisValue > 0.0f) {
		if (m_pPossessedPawn != nullptr) {
			m_pPossessedPawn->Shoot(0);
		}
	}
}

void PlayerController::SecondaryShot(float _axisValue){
	if (m_cIgnoreInput & SHOOTING)
		return;

	m_bFiring = true;
	m_bIsSecondaryFire = true;
	SetState(PlayerState::FIRING);

	if (_axisValue > 0.0f) {
		if (m_pPossessedPawn != nullptr) {
			m_pPossessedPawn->Shoot(1);
		}
	}
}

void PlayerController::AbilityOne(ACTION_STATUS status)
{
	if (status == ACTION_STATUS::AS_DOWN)
		return;
	if (m_cIgnoreInput & SHOOTING)
		return;

	if (status == ACTION_STATUS::AS_PRESSED)
	{
		if (m_pAbilities[0])
			m_pAbilities[0]->DisplayIndicatorCircle(true);
	}
	else
	{
		if (m_pAbilities[0])
		{
			if (m_pAbilities[0]->CanCast())
				m_pAbilities[0]->UseAbility();
			else
				Inferno::PostAudioEvent(PLASME_BLASTER_OUT_OF_AMMO);

			m_pAbilities[0]->DisplayIndicatorCircle(false);
		}
		else
			Inferno::PostAudioEvent(PLASME_BLASTER_OUT_OF_AMMO);
	}
}

void PlayerController::AbilityTwo(ACTION_STATUS status)
{
	if (status == ACTION_STATUS::AS_DOWN)
		return;
	if (m_cIgnoreInput & SHOOTING)
		return;

	if (status == ACTION_STATUS::AS_PRESSED)
	{
		if (m_pAbilities[1])
			m_pAbilities[1]->DisplayIndicatorCircle(true);
	}
	else
	{
		if (m_pAbilities[1])
		{
			if (m_pAbilities[1]->CanCast())
				m_pAbilities[1]->UseAbility();
			else
				Inferno::PostAudioEvent(PLASME_BLASTER_OUT_OF_AMMO);

			m_pAbilities[1]->DisplayIndicatorCircle(false);
		}
		else
			Inferno::PostAudioEvent(PLASME_BLASTER_OUT_OF_AMMO);
	}
}

void PlayerController::AbilityThree(ACTION_STATUS status)
{
	if (status == ACTION_STATUS::AS_DOWN)
		return;
	if (m_cIgnoreInput & SHOOTING)
		return;

	if (status == ACTION_STATUS::AS_PRESSED)
	{
		if (m_pAbilities[2])
			m_pAbilities[2]->DisplayIndicatorCircle(true);
	}
	else
	{
		if (m_pAbilities[2])
		{
			if (m_pAbilities[2]->CanCast())
				m_pAbilities[2]->UseAbility();
			else
				Inferno::PostAudioEvent(PLASME_BLASTER_OUT_OF_AMMO);

			m_pAbilities[2]->DisplayIndicatorCircle(false);
		}
		else
			Inferno::PostAudioEvent(PLASME_BLASTER_OUT_OF_AMMO);
	}
}

void PlayerController::AbilityFour(ACTION_STATUS status)
{
	if (status == ACTION_STATUS::AS_DOWN)
		return;
	if (m_cIgnoreInput & SHOOTING)
		return;

	if (status == ACTION_STATUS::AS_PRESSED)
	{
		if (m_pAbilities[3])
			m_pAbilities[3]->DisplayIndicatorCircle(true);
	}
	else
	{
		if (m_pAbilities[3])
		{
			if (m_pAbilities[3]->CanCast())
				m_pAbilities[3]->UseAbility();
			else
				Inferno::PostAudioEvent(PLASME_BLASTER_OUT_OF_AMMO);

			m_pAbilities[3]->DisplayIndicatorCircle(false);
		}
		else
			Inferno::PostAudioEvent(PLASME_BLASTER_OUT_OF_AMMO);
	}
}

void PlayerController::Recall(ACTION_STATUS _status){
	if (_status == ACTION_STATUS::AS_PRESSED){
		if ((m_cIgnoreInput & SHOOTING) && !m_bRecalling)
			return;

		m_bMoving = m_bStrafing = m_bFiring = false;
		m_bRecalling = !m_bRecalling;
		
		if (m_bRecalling){
			m_fRecallTimer = 0.0f;
			SetIgnoreInput(true);
			SetState(PlayerState::IDLE);
			if (m_pPlayerRecall){
				m_pPlayerRecall->StartRecall();
				m_pPlayerRecall->transform->SetPosition(transform->GetPosition());

				Inferno::PostAudioEvent(WWiseIDs::RECALL);
			}
		}
		else {
			UIElement* element = Inferno_UI::GetUIElementByName(std::string("Recall BKG"), std::string("HUD"));
			if (element)
				element->SetActive(false);
			SetIgnoreInput(false);
			SetState(PlayerState::IDLE);
			if (m_pPlayerRecall)
				m_pPlayerRecall->StopRecall();
		}
	}
}

void PlayerController::CancelRecall(){
	m_bRecalling = false;
	m_fRecallTimer = 0.0f;
	UIElement* element = Inferno_UI::GetUIElementByName(std::string("Recall BKG"), std::string("HUD"));
	if (element)
		element->SetActive(false);

	if (m_pPlayerRecall)
		m_pPlayerRecall->StopRecall();
}

void PlayerController::ToggleDash(ACTION_STATUS status){
	if (!m_bCanSprint || !m_pSprintDelayTimer->IsFinished())
		return;

	if (status != ACTION_STATUS::AS_DOWN)
		m_bDashing = false;
	else
		m_bDashing = true;

	SetIgnore(SHOOTING, m_bDashing);
}

void PlayerController::Interact(ACTION_STATUS _status)
{
	if (_status != ACTION_STATUS::AS_PRESSED) {
		return;
	}
	if (m_cIgnoreInput & SHOOTING)
		return;

	if (m_pCurrentInteractable != nullptr) {
		m_pCurrentInteractable->Interact();
		m_pCurrentInteractable = nullptr;		// <-- Temp for now, as we have no need to keep it. Can change later, just talk to Jack about it beforehand
	}
}

void PlayerController::DebugButton(ACTION_STATUS _status)
{
	if (_status != ACTION_STATUS::AS_PRESSED) {
		return;
	}
	if (m_cIgnoreInput & SHOOTING)
		return;

	m_pPossessedPawn->ShootDebugProjectile();
}

void PlayerController::SetState(PlayerState _state)
{
	if (m_eState == _state || _state < 0 || _state > STATE_MAX || m_bPauseAnimations)
		return;

	AnimationController* animController = m_pAnimator->GetAnimationController();

	bool idle = false;
	bool running = false;
	bool strafingLeft = false;
	bool strafingRight = false;
	bool firing = false;
	bool dying = false;
	bool dashing = false;
	bool pushing = false;
	bool throwing = false;

	switch (_state)
	{
	case PlayerController::IDLE:
	{
		m_fAnimTimers[IDLE].Reset();
		idle = true;
		m_bDashing = false;
		SetIgnore(SHOOTING, false);
	}
	break;
	case PlayerController::RUNNING:
	{
		m_fAnimTimers[RUNNING].Reset();
		running = true;
	}
	break;
	case PlayerController::STRAFING_LEFT:
	{
		m_fAnimTimers[STRAFING_LEFT].Reset();
		strafingLeft = true;
		//m_bDashing = false;
		//SetIgnore(SHOOTING, false);
	}
	break;
	case PlayerController::STRAFING_RIGHT:
	{
		m_fAnimTimers[STRAFING_RIGHT].Reset();
		strafingRight = true;
		//m_bDashing = false;
		//SetIgnore(SHOOTING, false);
	}
	break;
	case PlayerController::FIRING:
	{
		m_fAnimTimers[FIRING].Reset();
		firing = true;
	}
	break;
	case PlayerController::DYING:
	{
		m_fAnimTimers[DYING].Reset();
		dying = true;
		m_bDashing = false;
		//SetIgnore(SHOOTING, true);
	}
	break;
	case PlayerController::DASHING:
	{
		m_fAnimTimers[DASHING].Reset();
		dashing = true;
	}
	case PlayerController::BUTTONPUSH:
	{
		m_fAnimTimers[BUTTONPUSH].Reset();
		pushing = true;
		m_bDashing = false;
	}
	break;
	case PlayerController::THROWABILITY:
	{
		m_fAnimTimers[THROWABILITY].Reset();
		throwing = true;
		m_bDashing = false;
	}
	break;
	default:
		break;
	}

	animController->SetBool(std::string("Idle"), idle);
	animController->SetBool(std::string("Running"), running);
	animController->SetBool(std::string("StrafingLeft"), strafingLeft);
	animController->SetBool(std::string("StrafingRight"), strafingRight);
	animController->SetBool(std::string("Firing"), firing);
	animController->SetBool(std::string("Dying"), dying);
	animController->SetBool(std::string("Dashing"), dashing);
	animController->SetBool(std::string("ButtonPush"), pushing);
	animController->SetBool(std::string("ThrowAbility"), throwing);

	m_eState = _state;

	if (m_bDashing == true)
		m_pAnimator->SetSpeed(DASH_MULTIPLIER);
	else if (m_pAnimator->GetSpeed() > 1.0f)
		m_pAnimator->ResetSpeed();
}

// ===== Accessors ===== //
float PlayerController::GetMovementSpeed() const
{
	return m_fMovementSpeed;
}

Player* PlayerController::GetPossessedPawn() const
{
	return m_pPossessedPawn;
}

float PlayerController::GetStateTime(PlayerState _state)
{
	return m_fAnimTimers[(unsigned int)_state].GetSetTimeSeconds();
}

Ability* PlayerController::GetAbility(unsigned int ability) const
{
	if (ability < MAX_ABILITIES)
		return m_pAbilities[ability];

	return nullptr;
}

float PlayerController::GetAnimationTime(PlayerController::PlayerState _state)
{
	float animTime = 0.0f;

	switch (_state)
	{
	case PlayerController::IDLE:
		animTime = m_fAnimTimers[IDLE].GetSetTimeSeconds();
		break;
	case PlayerController::RUNNING:
		animTime = m_fAnimTimers[RUNNING].GetSetTimeSeconds();
		break;
	case PlayerController::STRAFING_LEFT:
		animTime = m_fAnimTimers[STRAFING_LEFT].GetSetTimeSeconds();
		break;
	case PlayerController::STRAFING_RIGHT:
		animTime = m_fAnimTimers[STRAFING_RIGHT].GetSetTimeSeconds();
		break;
	case PlayerController::FIRING:
		animTime = m_fAnimTimers[FIRING].GetSetTimeSeconds();
		break;
	case PlayerController::DYING:
		animTime = m_fAnimTimers[DYING].GetSetTimeSeconds();
		break;
	case PlayerController::DASHING:
		animTime = m_fAnimTimers[DASHING].GetSetTimeSeconds();
		break;
	case PlayerController::BUTTONPUSH:
		animTime = m_fAnimTimers[BUTTONPUSH].GetSetTimeSeconds();
		break;
	case PlayerController::THROWABILITY:
		animTime = m_fAnimTimers[THROWABILITY].GetSetTimeSeconds();
		break;
	default:
		break;
	}

	return animTime;
}
// ===================== //

// ===== Mutators ===== //
void PlayerController::SetMovementSpeed(float _speed)
{
	m_fMovementSpeed = _speed;
}

void PlayerController::SetCamera(ThirdPersonCamera* _camera)
{
	m_pCamera = _camera;
}

void PlayerController::SetAnimator(Animator* _animator)
{
	m_pAnimator = _animator;

	if (m_pAnimator != nullptr)
	{
		AnimationController* animController = m_pAnimator->GetAnimationController();
		if (animController != nullptr)
		{
			animController->GetAnimationDurations(m_fAnimTimers);
		}
	}
}

void PlayerController::SetPossessedPawn(Player* _pawn)
{
	m_pPossessedPawn = _pawn;
	m_pPossessedPawn->SetPlayerController(this);
}

void PlayerController::SetIgnoreInput(bool _value)
{
	if (_value)
		m_cIgnoreInput |= IGNORE_ALL;
	else
		m_cIgnoreInput &= ~IGNORE_ALL;
}

void PlayerController::SetIgnore(unsigned int _flag, bool _value)
{
	if (_value)
		m_cIgnoreInput |= _flag;
	else
		m_cIgnoreInput &= ~_flag;
}

void PlayerController::SetAbility(Ability* ability, unsigned int slot)
{
	if (slot < MAX_ABILITIES)
	{
		m_pAbilities[slot] = ability;

		if (m_pAbilityPanel != nullptr) {
			m_pAbilityPanel->SetAbility(ability, slot);
		}
	}
}
void PlayerController::SetAbilityActiveTime(float time, UINT slot)
{
	if (slot < MAX_ABILITIES)
	{
		if (m_pAbilityPanel)
			m_pAbilityPanel->SetAbilityActiveTime(time, slot);
	}
}

void PlayerController::SetCurrentInteractable(iInteractable* _interactableObject)
{
	m_pCurrentInteractable = _interactableObject;
}

void PlayerController::PauseAnimations(bool _value)
{
	m_bPauseAnimations = _value;
}

void PlayerController::SetCanSprint(bool _value)
{
	if (!_value) {
		m_bCanSprint = _value;
		m_bDashing = false;
		m_pSprintDelayTimer->SetTime(0.0f);
		m_pSprintDelayTimer->Reset();
	}
	else {
		m_pSprintDelayTimer->SetTime(SPRINT_DELAY_TIME);
		m_pSprintDelayTimer->Reset();
	}
}
// ==================== //