#pragma once

#include "IComponent.h"
#include "MathLib.h"
#include "Color.h"
#include "GameObject.h"
#include "ObjectRegistry.h"

typedef enum EMITTER_SHAPE
{
	EMITTER_SHAPE_SPHERE = 0,
	EMITTER_SHAPE_HEMISPHERE = 1,
	EMITTER_SHAPE_CIRCLE = 2,
	EMITTER_SHAPE_CONE = 3,
	TOTAL_SHAPES,
};

typedef enum EMITTER_STYLE
{
	EMITTER_STYLE_INNER = 0,
	EMITTER_STYLE_EDGE = 1,
};

class ParticleSystem : public IComponent
{
private:
	enum FlagTypes
	{
		ColorOverLife = 1,
		SizeOverLife = 2,
		Collision = 4,
		Looping = 8,
	};

	struct EmitterSettings
	{
		Vector3 WorldPosition;		// Position in world space
		float Duration;				// Play time of the emitter
		float AliveTime;			// Time the emitter has been alive
		float Lifetime;				// The total time a particle is alive
		float StartSpeed;			// Speed of the particle when spawned
		float GravityScale;			// Scalar to use for gravity
		Color StartColor;			// Color of the particle when spawned
		Color EndColor;				// Color of the particle at death
		float StartSize;			// Size of the particle when spawned
		float EndSize;				// Size of the particle at death
		unsigned int MaxParticles;	// Holds the total number of particles the emitter can have
		unsigned int Emission;		// How many particles per frame can spawn
		Vector3 ShapeDimensions;	// Dimensions for the shape
		unsigned int Flags;			// Flags to be used by the emitter and particles
	};

public:
	enum ParticleTextures
	{
		Standard = 0,
		Smoke = 1,
		Snow = 2,
		Ray1 = 3,
		Money = 4,
		Repair = 5,
		Resource = 6,
		Stretched = 7,
	};

private:
	EmitterSettings m_Settings;
	EMITTER_SHAPE m_eShape;
	EMITTER_STYLE m_eStyle;

	unsigned int m_uiEmissionRate;	// How many particles per second to emit
	float m_fAccumulator;			// Accumulates particles to emit
	bool m_bPaused;					// States if the emitter is paused or not

	virtual ~ParticleSystem() override;

	virtual IComponent& operator=(IComponent&) override;
	ParticleSystem& operator=(ParticleSystem& rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	ParticleSystem(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init();

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap) override;

	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable() final;
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
	virtual void OnDisable() final;

	// IN: void
	// OUT: void
	//
	// Update that is called every frame
	virtual void Update() override;

	// IN: void
	// OUT: void
	//
	// Resets the particle system and plays it
	void Play();

	void SetShape(EMITTER_SHAPE shape);
	void SetStyle(EMITTER_STYLE style);
	void SetDuration(float duration);
	void SetLooping(bool looping);
	void SetColorOverLife(bool col);
	void SetSizeOverLife(bool sol);
	void UseCollision(bool coll);
	void SetTexture(ParticleTextures texture);
	void SetLifetime(float lifetime);
	void SetStartSpeed(float speed);
	void SetGravityScale(float scale);
	void SetStartColor(Color col);
	void SetEndColor(Color col);
	void SetStartSize(float size);
	void SetEndSize(float size);
	void SetMaxParticles(unsigned int max);
	void SetEmissionRate(unsigned int rate);
	void SetBoxDimensions(Vector3 dim);
	void SetSphereDimensions(float radius);
	void SetHemisphereDimensions(float radius);
	void SetCircleDimensions(float radius);
	void SetConeDimensions(float baseRadius, float topRadius, float height);

	void Pause(bool pause);

	EMITTER_SHAPE GetShape() const;
	EMITTER_STYLE GetStyle() const;
	float GetDuration() const;
	bool IsLooping() const;
	bool GetColorOverTime() const;
	bool GetSizeOverTime() const;
	bool IsCollidable() const;
	float GetLifetime() const;
	float GetStartSpeed() const;
	float GetGravityScale() const;
	Color GetStartColor() const;
	Color GetEndColor() const;
	float GetStartSize() const;
	float GetEndSize() const;
	unsigned int GetMaxParticles() const;
	unsigned int GetEmissionRate() const;
	Vector3 GetDimensions() const;

	bool IsPaused() const;
	bool IsFinished() const;

	friend class ParticleManager;
	friend ParticleSystem* GameObject::AddComponent();
};