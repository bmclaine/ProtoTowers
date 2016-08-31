#include "../Public/Physics/CapsuleCollider.h"
#include "Components\Transform.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Gizmos.h"

CapsuleCollider::CapsuleCollider(GameObject* const gameObject, Transform* const transform) : Collider(gameObject, transform){
	m_fRadius = 0.0f;
	m_fHeight = 0.0f;
}

CapsuleCollider::~CapsuleCollider() {
	InfernoEngine_Physics::UnregisterColliderComponent(this);
}

IComponent& CapsuleCollider::operator=(IComponent& _rhs) {
	CapsuleCollider* rhsCapCol = dynamic_cast<CapsuleCollider*>(&_rhs);
	if (rhsCapCol == nullptr) {
		return *this;
	}

	return operator=(*rhsCapCol);
}

CapsuleCollider& CapsuleCollider::operator=(const CapsuleCollider& _rhs) {
	if (this != &_rhs) {
		m_fHeight = _rhs.m_fHeight;
		m_fRadius = _rhs.m_fRadius;

		Collider::operator=(_rhs);
	}

	return *this;
}

IComponent* CapsuleCollider::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	CapsuleCollider* cColl = new CapsuleCollider(gameObject, transform);
	(*cColl) = *this;

	return cColl;
}

// IN : void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void CapsuleCollider::Init() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void CapsuleCollider::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Center";
	Property* prop = data->GetProperty(propName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	m_f3Center.x = (float)atof(values[0].c_str()); m_f3Center.y = (float)atof(values[1].c_str()); m_f3Center.z = (float)atof(values[2].c_str());

	propName = "Radius";
	prop = data->GetProperty(propName);
	m_fRadius = (float)atof(prop->value.c_str());

	propName = "Height";
	prop = data->GetProperty(propName);
	m_fHeight = (float)atof(prop->value.c_str());

	propName = "IsTrigger";
	prop = data->GetProperty(propName);
	m_bIsTrigger = atoi(prop->value.c_str()) != 0;
}

// IN: void
// OUT: void
//
// Draws Gizmos for the component
void CapsuleCollider::OnDrawGizmos(){
	Gizmos::SetColor(Color(0.0f, 1.0f, 0.0f, 1.0f));
	Gizmos::DrawWireCapsule(transform->GetPosition() + m_f3Center, m_fHeight, m_fRadius);
}

float CapsuleCollider::GetRadius() const{
	return m_fRadius;
}

void CapsuleCollider::SetRadius(float radius){
	m_fRadius = radius;
}

float CapsuleCollider::GetHeight() const{
	return m_fHeight;
}

void CapsuleCollider::SetHeight(float h){
	m_fHeight = h;
}

Vector3 CapsuleCollider::GetStart(){
	m_f3Start = transform->TransformPoint(m_f3Center + Vector3(0.0f, -m_fHeight * 0.5f, 0.0f));
	return m_f3Start;
}

Vector3 CapsuleCollider::GetEnd(){
	m_f3End = transform->TransformPoint(m_f3Center + Vector3(0.0f, m_fHeight * 0.5f, 0.0f));
	return m_f3End;
}

Vector3 CapsuleCollider::GetDirection(){
	return GetEnd() - GetStart();
}

const AABB CapsuleCollider::GetBoundingBox() const{
	AABB box;
	box.center = transform->GetPosition() + m_f3Center;
	box.size = Vector3(m_fRadius * 2.0f, m_fHeight + m_fHeight * 0.5f, m_fRadius * 2.0f);
	return box;
}

const Box2D CapsuleCollider::GetBox2D(){
	Vector3 transPosition = transform->GetPosition();
	m_boundingBox = Box2D(Vector2(transPosition.x + m_f3Center.x, transPosition.z + m_f3Center.z), Vector2(m_fRadius * 2.0f, m_fRadius * 2.0f));
	return m_boundingBox;
}

int CapsuleCollider::GetColliderType(){
	return 2;
}



