#pragma once
#include "Components\IComponent.h"

class AIEntity;
class Entity;
class InfernoTimer;
class MeshRenderer;
class SphereCollider;
enum Team;

class FreezeBlastWave : public IComponent
{
private:
	// === Structures
	struct FrozenPlayer {
		Entity* Entity;
		PlayerController* PController = nullptr;
		Animator* Animator = nullptr;
		float PrevSpeed;
		float PrevAnimSpeed;
	};

	struct FrozenAI {
		AIEntity* Entity;
		NavAgent* NAgent = nullptr;
		Animator* Animator = nullptr;
		float PrevSpeed;
		float PrevAnimSpeed;
	};

	// === Variables
	Transform* m_pMeshTransform;
	InfernoTimer* m_pTimer;
	MeshRenderer* m_pRenderer;
	FrozenPlayer* m_pFrozenPlayer;
	SphereCollider* m_pCollider;
	std::vector<FrozenAI> m_vFrozenMinions;
	float m_fDamage;
	float m_fStartScale;
	float m_fEndScale;
	bool m_bExpanding;
	Team m_eTeam;

	// === Events === //
	void OnEntityRelease(Entity* _entity);
	void OnMeshGrowthDone();
	void OnLifetimeDone();
	// ============== //

protected:
	// === Protected Interface === //
	virtual IComponent& operator=(IComponent& _rhs) override;
	FreezeBlastWave& operator=(const FreezeBlastWave& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;
	// =========================== //

public:
	// === Constructor / Destructor === //
	FreezeBlastWave(GameObject* _gameObject, Transform* _transform);
	virtual ~FreezeBlastWave();
	// ================================ //

	// === Interface === //
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Update() override;
	// ================= //

	// === Collision Events === //
	virtual void OnTriggerEnter(Collider* _collider);
	// ======================== //

	// === Mutators === //
	void SetDamage(float _damage);
	void SetScales(float _startingScale, float _endingScale);
	void SetTeam(Team _team);
	// ================ //
};

