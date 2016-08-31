#include "stdafx.h"
#include "3rdPersonCamera.h"

// === Defines
#define PI 3.14159f

// === Constructor / Destructor === //
ThirdPersonCamera::ThirdPersonCamera(GameObject* const _owner, Transform* const _transform) : IComponent(_owner, _transform)
{
	m_bFixRotation = false;
	m_fCurrentXRotation = 0.0f;
	m_fCurrentYRotation = 0.0f;
	m_fMinVertRotation = -1.0f;
	m_fMaxVertRotation = 1.0f;
}
ThirdPersonCamera::~ThirdPersonCamera()
{

}
// ================================ //

// ===== Interface ===== //
/*virtual*/ void ThirdPersonCamera::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "TargetObject";
	Property* prop = data->GetProperty(propName);
	int ID = atoi(prop->value.c_str());
	if (ID > 0)
		m_pTargetObject = (GameObject*)objectMap.find((unsigned int)ID)->second;
}

void ThirdPersonCamera::Update()
{

}

void ThirdPersonCamera::HorizontalRotate(float _radians)
{
	if (_radians == 0.0f)
		return;

	m_fCurrentYRotation += _radians;

	m_pTargetObject->transform->Rotate(Vector3(0.0f, _radians, 0.0f));
}

void ThirdPersonCamera::VerticalRotate(float _radians)
{
	float newRot = (_radians < 0 ? Clamp(m_fMinVertRotation - m_fCurrentXRotation, m_fMaxVertRotation, _radians) : Clamp(m_fMinVertRotation, m_fMaxVertRotation - m_fCurrentXRotation, _radians));
	
	if (newRot == 0)
		return;

	m_fCurrentXRotation += newRot;

	transform->Rotate(Vector3(newRot, 0.0f, 0.0f));
}

// ===================== //

// === Protected Interface === // 
IComponent& ThirdPersonCamera::operator=(IComponent& _rhs)
{
	ThirdPersonCamera* rhs3rdPC = dynamic_cast<ThirdPersonCamera*>(&_rhs);
	if (rhs3rdPC == nullptr) {
		return *this;
	}

	return operator=(*rhs3rdPC);
}

ThirdPersonCamera& ThirdPersonCamera::operator=(const ThirdPersonCamera& _rhs)
{
	if (this != &_rhs) {
		m_bFixRotation = _rhs.m_bFixRotation;
		m_fcFlags = _rhs.m_fcFlags;
		m_fCurrentXRotation = _rhs.m_fCurrentXRotation;
		m_fCurrentYRotation = _rhs.m_fCurrentYRotation;
		m_fMaxVertRotation = _rhs.m_fMaxVertRotation;
		m_fMinVertRotation = _rhs.m_fMinVertRotation;
		m_pTargetObject = _rhs.m_pTargetObject;
		m_fMaxDistance = _rhs.m_fMaxDistance;
	}

	return *this;
}

IComponent* ThirdPersonCamera::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	ThirdPersonCamera* newComponent = new ThirdPersonCamera(gameObject, transform);
	(*newComponent) = *this;

	return newComponent;
}
// =========================== //

// ===== Private Interface ===== //
void ThirdPersonCamera::FixRotation()
{
	float rotation;
	if (m_fCurrentYRotation < 0) {
		rotation = Clamp(m_fCurrentYRotation, 0.0f, ToRadians(180 * -Time::DeltaTime));
	}
	else {
		rotation = Clamp(0.0f, m_fCurrentYRotation, ToRadians(180 * Time::DeltaTime));
	}
	m_pTargetObject->transform->Rotate(Vector3(0.0f, rotation, 0.0f));
	transform->Rotate(Vector3(0.0f, -rotation, 0.0f));

	m_fCurrentYRotation -= rotation;
	if (m_fCurrentYRotation == 0) {
		m_bFixRotation = false;
	}
}
// ============================= //

// ===== Accessors ===== //
float ThirdPersonCamera::GetMinVerticalRotation() const
{
	return m_fMinVertRotation;
}

float ThirdPersonCamera::GetMaxVerticalRotation() const
{
	return m_fMaxVertRotation;
}
// ===================== //

// ===== Mutators ===== //
void ThirdPersonCamera::SetTargetObject(GameObject* _target)
{
	m_pTargetObject = _target;
}

void ThirdPersonCamera::SetMinVerticalRotation(float _minVert)
{
	m_fMinVertRotation = _minVert;
}

void ThirdPersonCamera::SetMaxVerticalRotation(float _maxVert)
{
	m_fMaxVertRotation = _maxVert;
}
// ==================== //