#include "Physics\Rigidbody.h"
#include "Components\Transform.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "GameObject.h"
#include "Physics\Physics.h"
#include "InfernoTime.h"
#include "GlobalIncludes.h"


Rigidbody::Rigidbody(GameObject* const gameObject, Transform* const transform) : PhysicsComponent(gameObject,transform){
	m_bSleep = false;
	m_bUseGravity = true;
	m_f3Velocity = Vector3::Zero();
	m_fMass = 1.0f;
}

Rigidbody::~Rigidbody() {
	InfernoEngine_Physics::UnregisterRigidbodyComponent(this);
}

IComponent& Rigidbody::operator=(IComponent& _rhs) {
	Rigidbody* rhsRigidbody = dynamic_cast<Rigidbody*>(&_rhs);
	if (rhsRigidbody == nullptr) {
		return *this;
	}

	return operator=(*rhsRigidbody);
}

Rigidbody& Rigidbody::operator=(const Rigidbody& _rhs) {
	if (this != &_rhs) {
		m_bSleep = _rhs.m_bSleep;
		m_bUseGravity = _rhs.m_bUseGravity;
		m_f3Velocity = _rhs.m_f3Velocity;
		m_fcFlags = _rhs.m_fcFlags;
	}

	return *this;
}
IComponent* Rigidbody::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	Rigidbody* rigid = new Rigidbody(gameObject, transform);
	(*rigid) = *this;

	return rigid;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void Rigidbody::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Rigidbody::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Velocity";
	Property* prop = data->GetProperty(propName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	m_f3Velocity.x = (float)atof(values[0].c_str()); m_f3Velocity.y = (float)atof(values[1].c_str()); m_f3Velocity.z = (float)atof(values[2].c_str());

	propName = "UseGravity";
	prop = data->GetProperty(propName);
	m_bUseGravity = atoi(prop->value.c_str()) != 0;

	propName = "Sleep";
	prop = data->GetProperty(propName);
	m_bSleep = atoi(prop->value.c_str()) != 0;

	propName = "Mass";
	prop = data->GetProperty(propName);
	if (prop)
		m_fMass = (float)atof(prop->value.c_str());
}

// IN: void
// OUT: void
//
// Update only called every 1/60th of second
void Rigidbody::FixedUpdate(){
	if (m_bSleep) return;

	Vector3 movement = m_f3Velocity;
	if (m_bUseGravity)
		movement = movement + Physics::Gravity();

	transform->Translate(movement);
}

// IN: Vector3 - the new position of the rigidbody
// OUT: void
//
// Sets the position of the rigidbody to the new position
void Rigidbody::MovePosition(const Vector3& newPosition) {
	transform->SetPosition(newPosition);
}

void Rigidbody::SetUseGravity(bool useGravity){
	m_bUseGravity = useGravity;
}

void Rigidbody::SetSleep(bool sleep){
	m_bSleep = sleep;
}

void Rigidbody::SetVelocity(const Vector3& velocity){
	m_f3Velocity = velocity;
}

void Rigidbody::SetMass(const float mass){
	m_fMass = mass;
}

bool Rigidbody::UseGravity() const{
	return m_bUseGravity;
}

bool Rigidbody::IsSleep() const{
	return m_bSleep;
}

const Vector3& Rigidbody::GetVelocity() const{
	return m_f3Velocity;
}

float Rigidbody::GetMass() const{
	return m_fMass;
}

void Rigidbody::OnEnable() {
	InfernoEngine_Physics::RegisterRigidbodyComponent(this);
	IComponent::OnEnable();
}

void Rigidbody::OnDisable(){
	InfernoEngine_Physics::UnregisterRigidbodyComponent(this);
	IComponent::OnDisable();
}

