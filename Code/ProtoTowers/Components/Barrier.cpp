//Author: Zane Lewis
//
//A barrier that kills friendly minions and prevents the player from rushing the enemy base 
#include "stdafx.h"
#include "Barrier.h"
#include "EventSystem.h"
#include "Assets\Material.h"
#include "UIElement.h"
#include "UIComponents\UIRenderer.h"
#include "Assets\Texture.h"
#include "AIEntity.h"

Barrier::Barrier(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_bIsActive = true;
	m_fDistanceMoved = 0.0f;
	m_fVelocity = 40.0f;
	m_iChannel = -2;
	m_pMaterial = nullptr;

	m_pIconRenderer = nullptr;
	m_pIconElement = nullptr;

	m_pIcon = new Texture();
	m_pIcon->SetImage(Inferno::GetImage(L"IconBarrier.dds"));
}

Barrier::~Barrier()
{
	if (isEnabled()) {
		OnDisable();
	}

	delete m_pIcon;
	delete m_pIconElement;
}

IComponent& Barrier::operator=(IComponent& _rhs)
{
	Barrier* rhsBT = dynamic_cast<Barrier*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

Barrier& Barrier::operator= (const Barrier& _rhs)
{
	if (this != &_rhs) {
		m_bIsActive = _rhs.m_bIsActive;
		m_fDistanceMoved = _rhs.m_fDistanceMoved;
		m_fVelocity = _rhs.m_fVelocity;
	}

	return *this;
}

void Barrier::Init()
{
}

void Barrier::OnEnable()
{
	EventSystem::GetInstance()->RegisterForEvent("BarrierSetActiveFalse", this, BindEventFunction(&Barrier::SetActiveFalse, this));

	MeshRenderer* mRend = gameObject->GetComponent<MeshRenderer>();

	if (mRend)
		m_pMaterial = mRend->GetMaterial();

	// MiniMap Icon stuff
	if (m_pIconElement == nullptr)
	{
		m_pIconElement = new UIElement();
		m_pIconElement->SetName("BarrierIcon");
		m_pIconRenderer = m_pIconElement->AddComponent<UIRenderer>();

		RectTransform* rectTransform = m_pIconElement->GetTransform();
		rectTransform->SetAnchors(Anchors::ANCHOR_TOPLEFT);
		rectTransform->SetBounds(Vector2(10.0f, 10.0f));

		m_pIconRenderer->SetTexture(m_pIcon);

		m_pIconRenderer->SetLayer(2);

		m_pMapElement.element = m_pIconElement;
		m_pMapElement.transform = transform;
	}

	m_pIconRenderer->SetEnabled(true);
	EventSystem::GetInstance()->SendEventNow("EntityInstantiate", &m_pMapElement);

	IComponent::OnEnable();
}

void Barrier::OnDisable()
{
	EventSystem::GetInstance()->UnregisterFromEvent("BarrierSetActiveFalse", this, BindEventFunction(&Barrier::SetActiveFalse, this));

	EventSystem::GetInstance()->SendEventNow("EntityDestroy", &m_pMapElement);

	IComponent::OnDisable();
}

void Barrier::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "Channel";
	Property* prop = data->GetProperty(propertyName);
	m_iChannel = atoi(prop->value.c_str());
}

IComponent* Barrier::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	Barrier* newComponent = new Barrier(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;

}

void Barrier::Update()
{
	Inferno::PostAudioEventAtObjectPosition(BARRIER_WHITE_NOISE, gameObject);

	if (m_pMaterial)
	{
		m_pMaterial->SetOffsets(Vector2(0.0f, m_pMaterial->GetOffsets().y + 0.1f * Time::DeltaTime));
	}
	if (m_bIsActive && m_fDistanceMoved < 0)
	{
		Activate();
	}
	else if (!m_bIsActive && m_fDistanceMoved > INACTIVE_OFFSET)
	{
		Deactivate();
	}
//	if (m_fDistanceMoved <= INACTIVE_OFFSET)
//	{
//		Inferno::Destroy(gameObject);
//	}
	if (m_bIsActive == false)
	{
		m_pIconRenderer->SetEnabled(false);
	}
}
void Barrier::OnCollisionEnter(Collider * _col)
{
	AIEntity * minion = _col->gameObject->GetComponent<AIEntity>();
	if (minion && minion->GetTeam() == BLUE)
	{
		DamageInfo _DamInfo(90000);
		minion->TakeDamage(_DamInfo);
		Inferno::PostAudioEventAtObjectPosition(BARRIER_KILL_MINION, gameObject);
	}
}

void Barrier::Activate()
{
	transform->Translate(Vector3(0.0f, m_fVelocity * Time::DeltaTime, 0.0f));
}

void Barrier::Deactivate()
{
	float tempMove = -m_fVelocity * Time::DeltaTime;
	transform->Translate(Vector3(0.0f, tempMove, 0.0f));
	m_fDistanceMoved += tempMove;
}

//DESCRIPTION: sets the state of the barrier
//
//IN: bool - boolean to be set
//
//OUT: void
void Barrier::SetActiveFalse(void* _data)
{
	int _channel = (int)(_data);
	if (_channel == m_iChannel)
	{
		m_bIsActive = false;
		gameObject->GetComponent<BoxCollider>()->SetEnabled(false);
		gameObject->GetComponent<Collider>()->SetEnabled(false);
	}
}

bool Barrier::GetActive()
{
	return m_bIsActive;
}

void Barrier::SetChannel(int _channel)
{
	m_iChannel = _channel;
}