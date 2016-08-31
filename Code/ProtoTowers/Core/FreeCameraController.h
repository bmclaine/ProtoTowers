#pragma once

#include "Components\Controller.h"

class FreeCameraController : public Controller
{
private:
	Vector3 m_f3Rotation;
	float m_fMovementSpeed;

	virtual ~FreeCameraController() override;

protected:
	virtual IComponent& operator=(IComponent& _rhs) override;
	FreeCameraController& operator=(const FreeCameraController& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	FreeCameraController(GameObject* const gameObject, Transform* const transform);

	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	void ForwardMovement(float multiplier);
	void StrafeMovement(float multiplier);
	void MouseMovement(POINT _cursorPos, POINT _displacement);
	void VerticalLook(float multiplier);
	void HorizontalLook(float multiplier);
	void ResetRotation(ACTION_STATUS status);

	void SetMovementSpeed(float movementSpeed);

	friend class GameObject;
};

