//Author: Zane Lewis
//
//Prevents camera from going through files
#include "stdafx.h"
#include "SpringArm.h"
#include "Components\IComponent.h"
#include "GameObject.h"

SpringArm::SpringArm(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{
	m_bIsColliding = false;
	m_bIsTriggered = false;
	m_fMaxDistance = -1.0f;
	goCollidingObject = nullptr;
}

SpringArm::~SpringArm()
{

}

void SpringArm::Update()
{
	if (transform->GetParent() && !m_bIsColliding && !m_bIsTriggered)
	{
		if (m_fMaxDistance == -1.0f)
		{
			m_fMaxDistance = 2.5f;
		}
		Vector3 distance = transform->LocalPosition();
		if (distance.z > .15f)
			transform->Translate(Vector3(0.0f, 2.5f - distance.y, -Time::DeltaTime));
		if (distance.x < 1.0f)
		{
			transform->Translate(Vector3(Time::DeltaTime*.45f,0.0f,0.0f));
		}

	}
	else
	{
		if (goCollidingObject == nullptr||!goCollidingObject->IsActive())
		{
			SetCollisionFalse(nullptr);
		}
	}

}

void SpringArm::OnCollisionStay(Collider * _col)
{
	if (transform->GetParent())
	{
		m_bIsColliding = true;
		float distance = transform->LocalPosition().z;
		if (distance < m_fMaxDistance-.1f)
		{
			transform->Translate(Vector3(.0f, .0f, Time::DeltaTime * 15.0f ));
		}
		float distX = transform->LocalPosition().x;
		if (distX > .05f)
		{
			transform->Translate(Vector3(-Time::DeltaTime*10.0f,0.0f,0.0f));
		}
	}
}

void SpringArm::OnCollisionExit(Collider * _col)
{
	m_bIsColliding = false;
}

void SpringArm::OnTriggerEnter(Collider * _col)
{
	SpringArm* spArm = _col->gameObject->GetComponent<SpringArm>();
	if (spArm == nullptr&&!_col->IsTrigger())
	{
		m_bIsTriggered = true;
		Entity* thing = _col->gameObject->GetComponent<Entity>();
		if (thing)
		{
			thing->AddForTracking(this, BindEntityOnReleaseFunction(&SpringArm::SetCollisionFalse, this));

		}
		goCollidingObject = _col->gameObject;
	}
}

void SpringArm::OnTriggerExit(Collider * _col)
{
	SpringArm* spArm = _col->gameObject->GetComponent<SpringArm>();
	if (spArm == nullptr&&!_col->IsTrigger())
	{
		m_bIsTriggered = false;
		Entity * thing = _col->gameObject->GetComponent<Entity>();
		if (thing)
		{
			thing->RemoveFromTracking(this);
		}
		goCollidingObject = nullptr;
	}
}

IComponent& SpringArm::operator=(IComponent& _rhs)
{
	SpringArm* rhsBT = dynamic_cast<SpringArm*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

SpringArm& SpringArm::operator= (const SpringArm& _rhs)
{
	if (this != &_rhs) {
		m_fMaxDistance = _rhs.m_fMaxDistance;
		m_bIsColliding = _rhs.m_bIsColliding;
		m_bIsTriggered = _rhs.m_bIsTriggered;
		m_fcFlags = _rhs.m_fcFlags;
	}

	return *this;
}


void SpringArm::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "MaxDistance";
	Property* prop = data->GetProperty(propName);
	m_fMaxDistance = (float)atof(prop->value.c_str());
}

IComponent* SpringArm::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	SpringArm* newComponent = new SpringArm(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;

}

void SpringArm::SetCollisionFalse(Entity * _entity)
{
	m_bIsColliding = false;
	m_bIsTriggered = false;
}