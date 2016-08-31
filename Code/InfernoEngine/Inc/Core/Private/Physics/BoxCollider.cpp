#include "Physics\BoxCollider.h"
#include "Components\Transform.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "GameObject.h"
#include "Gizmos.h"

BoxCollider::BoxCollider(GameObject* const gameObject, Transform* const transform) : Collider(gameObject, transform){
	m_f3Size = Vector3(1, 1, 1);
}

BoxCollider::~BoxCollider(){
	InfernoEngine_Physics::UnregisterColliderComponent(this);
}

IComponent& BoxCollider::operator=(IComponent& _rhs)
{
	BoxCollider* rhsBoxCol = dynamic_cast<BoxCollider*>(&_rhs);
	if (rhsBoxCol == nullptr) {
		return *this;
	}

	return operator=(*rhsBoxCol);
}

BoxCollider& BoxCollider::operator=(const BoxCollider& _rhs)
{
	if (this != &_rhs) {
		m_f3Size = _rhs.m_f3Size;
		memcpy_s(m_pBoundPoints, sizeof(Vector3) * 8, _rhs.m_pBoundPoints, sizeof(Vector3) * 8);

		Collider::operator=(_rhs);
	}

	return *this;
}
IComponent* BoxCollider::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	BoxCollider* bColl = new BoxCollider(gameObject, transform);
	(*bColl) = *this;

	return bColl;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void BoxCollider::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void BoxCollider::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	unsigned int instanceId = GetInstanceID();
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Center";
	Property* prop = data->GetProperty(propName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	m_f3Center.x = (float)atof(values[0].c_str()); m_f3Center.y = (float)atof(values[1].c_str()); m_f3Center.z = (float)atof(values[2].c_str());

	propName = "Size";
	prop = data->GetProperty(propName);
	values = Inferno::StringParse(prop->value, ',');
	m_f3Size.x = (float)atof(values[0].c_str()); m_f3Size.y = (float)atof(values[1].c_str()); m_f3Size.z = (float)atof(values[2].c_str());

	propName = "IsTrigger";
	prop = data->GetProperty(propName);
	m_bIsTrigger = atoi(prop->value.c_str()) != 0;
}

// IN: void
// OUT: void
//
// Draws Gizmos for the component
void BoxCollider::OnDrawGizmos()
{
	Vector3 position = transform->GetPosition() + m_f3Center;
	Vector3 size(transform->GetScale().x * m_f3Size.x, transform->GetScale().y * m_f3Size.y, transform->GetScale().z * m_f3Size.z);
	Quaternion rot;
	transform->WorldMatrix().DecomposeRotation(rot);
	Gizmos::DrawWireCube(position, size, rot);
}

// IN: void
// OUT: Vector3* - the bounding points
//
// returns the bounding points
Vector3* const BoxCollider::GetBoundPoints(){
	return m_pBoundPoints;
}

// IN: void
// OUT: Vector3 - size
//
// returns the size of the bounding box
Vector3 BoxCollider::GetSize() const{
	Vector3 scale = transform->GetScale();
	Vector3 size(scale.x * m_f3Size.x, scale.y * m_f3Size.y, scale.z * m_f3Size.z);
	return size;
}

// IN: void
// OUT: void
//
// Caluclates the bound points
void BoxCollider::CalculateBoundPoints(){

	Vector3 min = m_f3Center - m_f3Size * 0.5f;
	Vector3 max = m_f3Center + m_f3Size * 0.5f;

	m_pBoundPoints[0] = (Vector3(max.x, max.y, max.z));
	m_pBoundPoints[1] = (Vector3(min.x, max.y, max.z));
	m_pBoundPoints[2] = (Vector3(min.x, min.y, max.z));
	m_pBoundPoints[3] = (Vector3(max.x, min.y, max.z));

	m_pBoundPoints[4] = (Vector3(max.x, max.y, min.z));
	m_pBoundPoints[5] = (Vector3(min.x, max.y, min.z));
	m_pBoundPoints[6] = (Vector3(min.x, min.y, min.z));
	m_pBoundPoints[7] = (Vector3(max.x, min.y, min.z));

	Matrix4 worldMatrix = transform->WorldMatrix();

	Vec3TransformCoordArray(&m_pBoundPoints[0], sizeof(Vector3), &m_pBoundPoints[0], sizeof(Vector3), 8, &worldMatrix);
}

// IN: Vector3* - the array of edges / should have three elements
// OUT: Vector3 - the array of edges
//
// Caluclates the edges and stores them in the array of edges
void BoxCollider::GetEdges(Vector3* edges){
	edges[0] = m_pBoundPoints[1] - m_pBoundPoints[0];
	edges[1] = m_pBoundPoints[2] - m_pBoundPoints[1];
	edges[2] = m_pBoundPoints[4] - m_pBoundPoints[0];
}

// IN: Vector3* - the array of face normals / should have three elements
// OUT: Vector3 - the array of face normals
//
// Caluclates the face normals and stores them in the array of face normals
void BoxCollider::GetFaceNormals(Vector3* faceNormals){

	faceNormals[0] = (m_pBoundPoints[2] - m_pBoundPoints[0]) ^ (m_pBoundPoints[3] - m_pBoundPoints[1]);
	faceNormals[1] = (m_pBoundPoints[1] - m_pBoundPoints[0]) ^ (m_pBoundPoints[4] - m_pBoundPoints[0]);
	faceNormals[2] = (m_pBoundPoints[2] - m_pBoundPoints[1]) ^ (m_pBoundPoints[5] - m_pBoundPoints[1]);
}

float BoxCollider::GetPenetrationOnAxis(BoxCollider* lhs, BoxCollider* rhs, const Vector3& axis, const Vector3& toCenter){

	float oneProject = TransformToAxis(lhs, axis);
	float twoProject = TransformToAxis(rhs, axis);

	float distance = fabsf(toCenter * axis);

	//Return the overlap
	//positive indicates overlap
	//negative indicates seperation
	return oneProject + twoProject - distance;
}

float BoxCollider::TransformToAxis(BoxCollider* collider, const Vector3& axis){
	Vector3* boundPoints = collider->GetBoundPoints();

	float min = FLT_MAX;
	float max = -FLT_MAX;

	for (int i = 0; i < 8; ++i){
		float value = (boundPoints[i] * 0.5f)* axis;

		min = fminf(min, value);
		max = fmaxf(max, value);
	}

	return max - min;
}

const AABB BoxCollider::GetBoundingBox() const{
	return AABB(transform->GetPosition() + m_f3Center, m_f3Size);
}

const Box2D BoxCollider::GetBox2D() {
	return m_boundingBox;
}

void BoxCollider::SetSize(const Vector3& size){
	m_f3Size = size;
}

int BoxCollider::GetColliderType(){
	return 0;
}

void BoxCollider::OnEnable(){
	CalculateBoundPoints();
	Vector3 min = Vector3(0, 0, 0);
	Vector3 max = Vector3(0, 0, 0);

	for (int i = 0; i < 8; ++i){
		Vector3 point = m_pBoundPoints[i];
		min.x = fminf(min.x, point.x);
		min.y = fminf(min.y, point.y);
		min.z = fminf(min.z, point.z);

		max.x = fmaxf(max.x, point.x);
		max.y = fmaxf(max.y, point.y);
		max.z = fmaxf(max.z, point.z);
	}

	Vector2 position = Vector2(transform->GetPosition().x + m_f3Center.x, transform->GetPosition().z + m_f3Center.z);
	Vector2 size = Vector2((max.x - min.x), (max.z - min.z));
	m_boundingBox = Box2D(position, size);
	Collider::OnEnable();
}



