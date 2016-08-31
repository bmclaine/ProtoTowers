#include "stdafx.h"
#include "PlayerRecall.h"

#include "Components\ParticleSystem.h"
#include "FadeAway.h"
#include "UIElement.h"

#define ROTATION_SPEED 120.0f

#define FADE_IN_TIME 1.5f
#define FADE_OUT_TIME 1.5f

PlayerRecall::PlayerRecall(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform){
	m_pFadeAway = nullptr;
	m_bRecalling = false;
}

PlayerRecall::~PlayerRecall(){
	m_pFadeAway = nullptr;
	m_bRecalling = false;
}

PlayerRecall& PlayerRecall::operator=(const PlayerRecall& rhs){

	if (this != &rhs)
		m_bRecalling = rhs.m_bRecalling;

	return *this;
}

IComponent* PlayerRecall::CreateCopy(GameObject* const gameObject, Transform* const transform){

	PlayerRecall* pRecall = new PlayerRecall(gameObject, transform);
	(*pRecall) = (*this);
	return pRecall;
}

void PlayerRecall::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;
}

void PlayerRecall::OnEnable(){
	IComponent::OnEnable();

	if (m_pFadeAway == nullptr) {
		UIElement* foundElement = Inferno_UI::GetUIElementByName(std::string("Fade Away Effect"), std::string("ExtendedHUD"));
		if (foundElement != nullptr) {
			m_pFadeAway = foundElement->GetComponent<FadeAway>(true);
		}
	}

	m_bRecalling = false;
}

void PlayerRecall::OnDisable(){
	IComponent::OnDisable();
}

void PlayerRecall::OnDestroy(){
	IComponent::OnDestroy();

	m_pFadeAway = nullptr;
}

void PlayerRecall::Update(){
	if (!m_bRecalling) return;

	transform->Rotate(Vector3(0, ToRadians(ROTATION_SPEED * 2.0f * Time::DeltaTime), 0));
}

void PlayerRecall::StartRecall(){
	SetRecalling(true);
	SetRecallTimer(0.0f);

	transform->GetChild(0)->gameObject->GetComponent<ParticleSystem>()->Play();
	transform->GetChild(1)->gameObject->GetComponent<ParticleSystem>()->Play();
	transform->GetChild(2)->gameObject->GetComponent<ParticleSystem>()->Play();

	if (m_pFadeAway){
		m_pFadeAway->SetFadeTime(FADE_OUT_TIME, FADE_IN_TIME);
		m_pFadeAway->SetFadeColor(0.4f, 1.0f, 1.0f, 1.0f);
		m_pFadeAway->SetEnabled(true);
	}
}

void PlayerRecall::StopRecall(){
	SetRecalling(false);

	transform->GetChild(0)->gameObject->GetComponent<ParticleSystem>()->Pause(true);
	transform->GetChild(1)->gameObject->GetComponent<ParticleSystem>()->Pause(true);
	transform->GetChild(2)->gameObject->GetComponent<ParticleSystem>()->Pause(true);
}

void PlayerRecall::SetRecalling(bool isRecalling){
	m_bRecalling = isRecalling;
}

bool PlayerRecall::GetRecalling() const{
	return m_bRecalling;
}

void PlayerRecall::SetRecallTimer(float recallTimer){
	m_fRecallTimer = recallTimer;
}
