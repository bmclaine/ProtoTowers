#include "stdafx.h"
#include "BarrierButton.h"
#include "EventSystem.h"
#include "Physics\Collider.h"
#include "../Core/PlayerController.h"
#include "InputManager.h"
#include "GameObject.h"
#include "ParticleManager.h"

BarrierButton::BarrierButton(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_iChannel = -1;
	m_bIsAvailable = false;
	m_pInteractingPlayer = nullptr;
	m_pPromptTexture = new Texture();
	m_pPromptTexture->SetImage(Inferno::GetImage(L"MinionWaveWarning.dds"));
	m_pButtonPrompt = nullptr;
	m_pParticleEffect = nullptr;
	m_bSpawnsMinionWave = false;
}

BarrierButton::~BarrierButton()
{
	delete m_pPromptTexture;
}

IComponent& BarrierButton::operator=(IComponent& _rhs)
{
	BarrierButton* rhsBT = dynamic_cast<BarrierButton*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

BarrierButton& BarrierButton::operator=(const BarrierButton& _rhs)
{
	if (this != &_rhs) {

		m_bIsAvailable = _rhs.m_bIsAvailable;
		m_fcFlags = _rhs.m_fcFlags;
		m_iChannel = _rhs.m_iChannel;
		m_pInteractingPlayer = _rhs.m_pInteractingPlayer;
		m_pButtonPrompt = _rhs.m_pButtonPrompt;

		if (m_pPromptTexture)
		{
			delete m_pPromptTexture;
		}

		m_pPromptTexture = new Texture();
		m_pPromptTexture->SetImage(_rhs.m_pPromptTexture->GetImage());
	}

	return *this;
}

void BarrierButton::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "Channel";
	Property* prop = data->GetProperty(propertyName);
	m_iChannel = atoi(prop->value.c_str());

	propertyName = "SpawnsMinionWave";
	prop = data->GetProperty(propertyName);
	if (prop != nullptr)
	{
		if (std::strcmp(prop->value.c_str(), "False") == 0)
		{
			m_bSpawnsMinionWave = false;
		}
		else
		{
			m_bSpawnsMinionWave = true;
		}
	}

	propertyName = "Level";
	prop = data->GetProperty(propertyName);
	if (prop != nullptr)
	{

		m_iLevel = atoi(prop->value.c_str());
	}

	propertyName = "ParticleEffect";
	prop = data->GetProperty(propertyName);
	int ID = atoi(prop->value.c_str());
	if (ID > 0){
		GameObject* _particleEffect = (GameObject*)objectMap.find((unsigned int)ID)->second;
		if (_particleEffect)
			SetParticleEffect(_particleEffect);
	}
}

void BarrierButton::Init()
{

}

void BarrierButton::OnEnable()
{
	if (m_pButtonPrompt == nullptr) {
		GameObject* foundObject = transform->GetChildByName(std::string("Barrier Button Prompt"));
		if (foundObject != nullptr) {
			m_pButtonPrompt = foundObject->GetComponent<SpriteRenderer>();
		}
	}

	if (m_pButtonPrompt != nullptr) {
		m_pButtonPrompt->gameObject->SetActive(false, true);
	}
	if (m_pParticleEffect)
	{
		m_pParticleEffect->GetComponent<ParticleSystem>()->Pause(true);
		m_pParticleEffect->GetComponent<ParticleSystem>()->SetStartColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
		m_pParticleEffect->GetComponent<ParticleSystem>()->SetEndColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
	}
	IComponent::OnEnable();
}

void BarrierButton::OnDisable()
{
	IComponent::OnDisable();
	m_pButtonPrompt = nullptr;
}

void BarrierButton::OnTriggerEnter(Collider * _col)
{
	if (!_col->IsTrigger() && _col->gameObject->GetTag() == "Player" && m_bIsAvailable)
	{
		if (m_bSpawnsMinionWave)
		{
			m_pPromptTexture->SetImage(Inferno::GetImage(L"MinionWaveWarning.dds"));
			m_pButtonPrompt->SetDimensions(Vector2(2.0f, 2.0f));
		}
		else
		{
			m_pButtonPrompt->SetDimensions(Vector2(1.0f, 1.0f));
			if (InputManager::UsingController())
			{
				m_pPromptTexture->SetImage(Inferno::GetImage(L"UI_XboxBtn_A.dds"));
			}
			else
			{
				m_pPromptTexture->SetImage(Inferno::GetImage(L"UI_KeyBtn_E.dds"));
			}
		}
		m_pButtonPrompt->gameObject->SetActive(true);
		m_pButtonPrompt->SetImage(m_pPromptTexture->GetImage());

		m_pInteractingPlayer = _col->gameObject->GetComponent<PlayerController>();
		m_pInteractingPlayer->SetCurrentInteractable(this);
	}
}

void BarrierButton::OnTriggerExit(Collider * _col)
{
	if (!_col->IsTrigger() && _col->gameObject->GetTag() == "Player")
	{
		if (m_pInteractingPlayer != nullptr)
		{
			m_pButtonPrompt->gameObject->SetActive(false);
			m_pInteractingPlayer->SetCurrentInteractable(nullptr);
			m_pInteractingPlayer = nullptr;
		}
	}
}


void BarrierButton::Update()
{
}

IComponent* BarrierButton::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	BarrierButton* newComponent = new BarrierButton(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

void BarrierButton::SetIsAvailable(bool _isAvailable)
{
	m_bIsAvailable = _isAvailable;
	if (_isAvailable)
	{
		m_pParticleEffect->GetComponent<ParticleSystem>()->Pause(false);
	}
}

bool BarrierButton::GetIsAvailable()
{
	return m_bIsAvailable;
}

void BarrierButton::SetChannel(int _channel)
{
	m_iChannel = _channel;
}

void BarrierButton::SetParticleEffect(GameObject * _particleEffect)
{
	m_pParticleEffect = _particleEffect;


}

void BarrierButton::Interact()
{
	EventSystem::GetInstance()->SendEventNow("BarrierSetActiveFalse", (void*)m_iChannel);
	m_bIsAvailable = false;

	EventSystem::GetInstance()->SendEventNow("UpgradeControlPoint", (void*)m_iLevel);

	m_pParticleEffect->GetComponent<ParticleSystem>()->Pause(true);
	if (m_pInteractingPlayer != nullptr)
	{
		m_pButtonPrompt->gameObject->SetActive(false);
		m_pInteractingPlayer->SetCurrentInteractable(nullptr);
		m_pInteractingPlayer = nullptr;
	}
}