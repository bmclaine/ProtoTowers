#include "stdafx.h"
#include "FreeCameraController.h"

FreeCameraController::FreeCameraController(GameObject* const gameObject, Transform* const transform) : Controller(gameObject, transform)
{
	m_f3Rotation = Vector3::Zero();
	m_fMovementSpeed = 20.0f;

	//RegisterForAxisEvent("Forward", this, BindAxisFunction(&FreeCameraController::ForwardMovement, this));
	//RegisterForAxisEvent("Right", this, BindAxisFunction(&FreeCameraController::StrafeMovement, this));
	//RegisterForAxisEvent("RotationX", this, BindAxisFunction(&FreeCameraController::HorizontalLook, this));
	//RegisterForAxisEvent("RotationY", this, BindAxisFunction(&FreeCameraController::VerticalLook, this));
	//RegisterForKeyEvent("AbilityOne", this, BindActionFunction(&FreeCameraController::ResetRotation, this));
	//RegisterForMouseMovementEvent(this, BindMouseMovementFunction(&FreeCameraController::MouseMovement, this));
}

FreeCameraController::~FreeCameraController()
{
	//UnregisterForAxisEvent("Forward", this, BindAxisFunction(&FreeCameraController::ForwardMovement, this));
	//UnregisterForAxisEvent("Right", this, BindAxisFunction(&FreeCameraController::StrafeMovement, this));
	//UnregisterForAxisEvent("RotationX", this, BindAxisFunction(&FreeCameraController::HorizontalLook, this));
	//UnregisterForAxisEvent("RotationY", this, BindAxisFunction(&FreeCameraController::VerticalLook, this));
	//UnregisterForMouseMovementEvent(this, BindMouseMovementFunction(&FreeCameraController::MouseMovement, this));
}

IComponent& FreeCameraController::operator=(IComponent& _rhs)
{
	return *this;
}
FreeCameraController& FreeCameraController::operator=(const FreeCameraController& _rhs)
{
	m_fMovementSpeed = _rhs.m_fMovementSpeed;

	return *this;
}
IComponent* FreeCameraController::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	FreeCameraController* newController = new FreeCameraController(gameObject, transform);
	(*newController) = *this;

	return newController;
}

void FreeCameraController::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "MovementSpeed";
	Property* prop = data->GetProperty(propName);
	m_fMovementSpeed = (float)atof(prop->value.c_str());
}

void FreeCameraController::ForwardMovement(float multiplier)
{
	transform->Translate(Vector3(0, 0, m_fMovementSpeed * multiplier * Time::DeltaTime));
}
void FreeCameraController::StrafeMovement(float multiplier)
{
	transform->Translate(Vector3(m_fMovementSpeed * multiplier * Time::DeltaTime, 0, 0));
}
void FreeCameraController::MouseMovement(POINT _cursorPos, POINT _displacement)
{

}
void FreeCameraController::VerticalLook(float multiplier)
{
	m_f3Rotation.y += ToRadians(180 * -multiplier * Time::DeltaTime);

	transform->Rotate(Vector3(ToRadians(180 * -multiplier * Time::DeltaTime), 0, 0));
}
void FreeCameraController::HorizontalLook(float multiplier)
{
	m_f3Rotation.x += ToRadians(180 * -multiplier * Time::DeltaTime);

	transform->Rotate(Vector3(0, ToRadians(180 * -multiplier * Time::DeltaTime), 0));
}
	 
void FreeCameraController::ResetRotation(ACTION_STATUS status)
{
	transform->SetRotation(IE_DEGTORAD * 90.0f, IE_DEGTORAD * 180.0f, 0);
}

void FreeCameraController::SetMovementSpeed(float movementSpeed)
{
	m_fMovementSpeed = movementSpeed;
}