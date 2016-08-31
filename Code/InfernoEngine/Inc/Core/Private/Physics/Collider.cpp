#include "Physics\Collider.h"
#include "Physics\CollisionLibrary.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Components\Transform.h"
#include "GameObject.h"
#include "AssertHelper.h"

Collider::Collider(GameObject* const gameObject, Transform* const transform) : PhysicsComponent(gameObject, transform) {
	m_bIsTrigger = false;
	m_timeStamp = INT_MAX;
}

Collider& Collider::operator=(const Collider& _rhs)
{
	if (this != &_rhs) {
		m_bIsTrigger = _rhs.m_bIsTrigger;
		m_f3Center = _rhs.m_f3Center;
		m_fcFlags = _rhs.m_fcFlags;
	}

	return *this;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void Collider::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Collider::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

}

void Collider::AddCollision(Collider* collider){
	if (collider){
		m_Collisions.emplace(collider, COL_ENTER);
	}
}

void Collider::RemoveCollision(Collider* collider){
	auto it = m_Collisions.find(collider);

	if (it != m_Collisions.end())
		it = m_Collisions.erase(it);
}

bool Collider::Contains(Collider* collider){
	return m_Collisions.find(collider) != m_Collisions.end();
}

void Collider::UpdateCollision(Collider* collider, CollisionState state){
	std::unordered_map<Collider*, CollisionState>::iterator it;
	it = m_Collisions.find(collider);

	if (it != m_Collisions.end()){
		it->second = state;
	}
	else{
		if (state != COL_EXIT)
			AddCollision(collider);
	}
}

int Collider::GetColliderType(){
	return -1;
}

int Collider::GetTimeStamp() const{
	return m_timeStamp;
}

bool Collider::IsTrigger() const{
	return m_bIsTrigger;
}

bool Collider::IsStatic() const{
	return gameObject->IsStatic();
}

Vector3 Collider::GetCenter() const{
	return transform->TransformPoint(m_f3Center);
}

void Collider::SetIsTrigger(bool isTrigger){
	m_bIsTrigger = isTrigger;
}

void Collider::SetCenter(const Vector3& center){
	m_f3Center = center;
}

void Collider::SetTimeStamp(const int timeStamp){
	m_timeStamp = timeStamp;
}

void Collider::CollisionEvents(){

	int exitCount = 0;
	auto it = m_Collisions.begin();
	while (it != m_Collisions.end()){
		Collider* collider = it->first;
		CollisionState state = it->second;

		if (collider->IsTrigger() || this->IsTrigger()){
			if (state == COL_ENTER){
				collider->gameObject->OnTriggerEnter(this);
				this->gameObject->OnTriggerEnter(collider);
			}
			else if (state == COL_STAY){
				collider->gameObject->OnTriggerStay(this);
				this->gameObject->OnTriggerStay(collider);
			}
			else if (state == COL_EXIT){
				collider->gameObject->OnTriggerExit(this);
				this->gameObject->OnTriggerExit(collider);
				++exitCount;
			}
		}
		else{
			if (state == COL_ENTER){
				collider->gameObject->OnCollisionEnter(this);
				this->gameObject->OnCollisionEnter(collider);
			}
			else if (state == COL_STAY){
				collider->gameObject->OnCollisionStay(this);
				this->gameObject->OnCollisionStay(collider);
			}
			else if (state == COL_EXIT){
				collider->gameObject->OnCollisionExit(this);
				this->gameObject->OnCollisionExit(collider);
				++exitCount;
			}
		}

		++it;
	}

	it = m_Collisions.begin();
	int count = 0;
	while (count < exitCount){
		CollisionState state = it->second;
		if (state != COL_EXIT){
			++it;
		}
		else{
			m_Collisions.erase(it->first);
			it = m_Collisions.begin();
			++count;
		}
	}

	SetTimeStamp(INT_MAX);
}

void Collider::OnEnable(){
	IComponent::OnEnable();
	m_Collisions.clear();
	SetTimeStamp(INT_MAX);
	InfernoEngine_Physics::RegisterColliderComponent(this);
}

void Collider::OnDisable(){
	IComponent::OnDisable();
	SetTimeStamp(INT_MAX);
	InfernoEngine_Physics::UnregisterColliderComponent(this);
}


