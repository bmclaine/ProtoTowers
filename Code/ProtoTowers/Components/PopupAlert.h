#pragma once
#include "Components\IComponent.h"

class InfernoTimer;
class SpriteRenderer;

class PopupAlert : public IComponent
{
private:
	// === Variables
	Vector3 m_v3Direction;
	InfernoTimer* m_pTimer;
	float m_fSpeed;

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	PopupAlert& operator=(const PopupAlert& _rhs);
	virtual IComponent* CreateCopy(GameObject* const _gameObject, Transform* const _transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	PopupAlert(GameObject* _gameObject, Transform* _transform);
	virtual ~PopupAlert();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void Update() override;
	// ================= //

	// === Mutators === //
	void SetDirection(Vector3 _direction);
	void SetDirection(float _xDir, float _yDir, float _zDir);
	void SetAliveTime(float _seconds);
	void SetSpeed(float _speed);
	// ================ //
};

