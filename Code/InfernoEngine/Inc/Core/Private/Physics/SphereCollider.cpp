#include "Physics\SphereCollider.h"
#include "Components\Transform.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Gizmos.h"

#include "GameObject.h"

SphereCollider::SphereCollider(GameObject* const gameObject, Transform* const transform) : Collider(gameObject, transform){
	m_fRadius = 0;
}

SphereCollider::~SphereCollider() {
	InfernoEngine_Physics::UnregisterColliderComponent(this);
}

IComponent& SphereCollider::operator=(IComponent& _rhs) {
	SphereCollider* rhsSphCol = dynamic_cast<SphereCollider*>(&_rhs);
	if (rhsSphCol == nullptr) {
		return *this;
	}

	return operator=(*rhsSphCol);
}

SphereCollider& SphereCollider::operator=(const SphereCollider& _rhs)
{
	if (this != &_rhs) {
		m_fRadius = _rhs.m_fRadius;

		Collider::operator=(_rhs);
	}

	return *this;
}
IComponent* SphereCollider::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	SphereCollider* sColl = new SphereCollider(gameObject, transform);
	(*sColl) = *this;

	return sColl;
}

// IN : void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void SphereCollider::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void SphereCollider::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Center";
	Property* prop = data->GetProperty(propName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	m_f3Center.x = (float)atof(values[0].c_str()); m_f3Center.y = (float)atof(values[1].c_str()); m_f3Center.z = (float)atof(values[2].c_str());

	propName = "Radius";
	prop = data->GetProperty(propName);
	m_fRadius = (float)atof(prop->value.c_str());

	propName = "IsTrigger";
	prop = data->GetProperty(propName);
	m_bIsTrigger = atoi(prop->value.c_str()) != 0;
}

// IN: void
// OUT: void
//
// Draws Gizmos for the component
void SphereCollider::OnDrawGizmos()
{
	Gizmos::SetColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
	Gizmos::DrawWireSphere(transform->GetPosition() + m_f3Center, GetRadius());
}

float SphereCollider::GetRadius() const{
	Vector3 _scale = transform->GetScale();
	float scale = fmaxf(_scale.x, _scale.y);
	scale = fmaxf(scale, _scale.z);
	return scale * m_fRadius;
}

void SphereCollider::SetRadius(float radius){
	m_fRadius = radius;
}

const AABB SphereCollider::GetBoundingBox() const{
	float radius = GetRadius();
	return AABB(transform->GetPosition() + m_f3Center, Vector3(radius * 2.0f, radius * 2.0f, radius * 2.0f));
}

const Box2D SphereCollider::GetBox2D(){
	Vector3 transPosition = transform->GetPosition();
	float radius = GetRadius();
	m_boundingBox = Box2D(Vector2(transPosition.x + m_f3Center.x, transPosition.z + m_f3Center.z), Vector2(radius, radius));
	return m_boundingBox;
}

int SphereCollider::GetColliderType(){
	return 1;
}
