//Author: Zane Lewis
//
//projectile fired by baseweapon
#pragma once
class IComponent;
class Entity;

class BaseProjectile : public IComponent
{
protected:
	float m_fVelocity;
	float m_fDistTraveled;
	float m_fKillDist;
	float m_fDamage;
	int m_iStunChance;
	bool m_bIsAlive;
	bool m_bIsFireable;
	bool m_bAddEffect;
	Vector3 m_pMoveDirection;
	Entity* m_pOwningEntity;	
	//DESCRIPITON: After killtime is up, set m_bIsAlive to false
	//
	//IN: void
	//
	//OUT: void
	void Kill(void);
public:
	BaseProjectile(GameObject* const gameObject, Transform* const transform);
	virtual ~BaseProjectile() override;

	virtual BaseProjectile& operator=(const BaseProjectile& _rhs);
	IComponent& operator=(IComponent&) override;

	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);

	//DESCRIPTION: returns if the projectile is alive or not
	//
	//IN: void
	//
	//OUT: bool - true if the projectile is alive, false otherwise
	bool IsAlive(void){ return m_bIsAlive; }

	//DESCRIPTION: returns if the projectile is fireable or not
	//
	//IN: void
	//
	//OUT: bool - true if the projecile is fireable, false otherwise
	bool IsFireable(void){ return m_bIsFireable; }

	//DESCRIPITON: Sets the projectile to alive and moves by the passed in vector
	//
	//IN: Vector3 - Position to fire from
	//    Vector3 - direction to move in
	//
	//OUT: void
	void FireProjectile(const Vector3& _position, const Vector3& _Direction = Vector3(0, 0, 1));

	//DESCRIPTION: Updates the projectile
	//
	//IN:void
	//
	//OUT:void
	void Update() override;

	void OnTriggerEnter(Collider*) override;

	//DESCRIPTION: Sets fireable to true
	//
	//IN: void
	//
	//OUT: void
	void MakeFireable();

	//DESCRIPTION: Sets the owning entity of this projectile
	//
	//IN: Entity* - The entity that shot this projectile
	//
	//OUT: void
	void SetOwningEntity(Entity* _entity);

	void SetDamage(float damage);

	void SetAddEffect(bool value);

	void SetStunChance(int chance);

	bool GetAddEffect() const;

	//DESCRIPTION: Returns the owning entity of this projectile
	//
	//IN: void
	//
	//OUT: Entity* - The owning entity of the projectile
	Entity* GetOwningEntity() const;

	void CreateImpactParticles();
};

