// Author : Jack Gillette
//
// A camera controller class for handling rotation and position of a camera that follows a 3rd person character
#pragma once
#include "Components\IComponent.h"

class ThirdPersonCamera : public IComponent
{
private:
	// === Variables
	GameObject* m_pTargetObject;
	float m_fCurrentYRotation;
	float m_fCurrentXRotation;
	float m_fMinVertRotation, m_fMaxVertRotation;
	float m_fMaxDistance;
	bool m_bFixRotation;

	// === Private Interface === //
	void FixRotation();
	// ========================= //

protected:
	// === Protected Interface === // 
	virtual IComponent& operator=(IComponent& _rhs) override;
	ThirdPersonCamera& operator=(const ThirdPersonCamera& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	ThirdPersonCamera(GameObject* const _owner, Transform* const _transform);
	virtual ~ThirdPersonCamera() override;
	// ================================ //

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	// === Interface === //
	virtual void Update() override;

	void HorizontalRotate(float _radians);
	void VerticalRotate(float _radians);
	// ================= //

	// === Accessors === //
	float GetMinVerticalRotation() const;
	float GetMaxVerticalRotation() const;
	// ================= //

	// === Mutators === //
	void SetTargetObject(GameObject* _target);
	void SetMinVerticalRotation(float _minVert);
	void SetMaxVerticalRotation(float _maxVert);
	// ================ //
};