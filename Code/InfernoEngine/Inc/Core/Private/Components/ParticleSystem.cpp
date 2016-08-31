#include "Components\ParticleSystem.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "Components\Transform.h"
#include "InfernoTime.h"
#include "GlobalIncludes.h"

ParticleSystem::ParticleSystem(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	// Set default values
	m_eShape = EMITTER_SHAPE_CONE;
	m_eStyle = EMITTER_STYLE_INNER;

	m_Settings.WorldPosition = transform->GetPosition();
	m_Settings.Duration = 1.0f;
	m_Settings.AliveTime = 0.0f;
	m_Settings.Lifetime = 3.0f;
	m_Settings.StartSpeed = 5.0f;
	m_Settings.GravityScale = 0.0f;
	m_Settings.StartColor = Color(1, 1, 1, 1);
	m_Settings.EndColor = Color(1, 1, 1, 1);
	m_Settings.StartSize = 1.0f;
	m_Settings.EndSize = 1.0f;
	m_Settings.MaxParticles = 100;
	m_Settings.Emission = 0;
	m_Settings.ShapeDimensions = Vector3(0.1f, 1, 1);
	m_Settings.Flags = Looping;

	m_uiEmissionRate = 50;
	m_fAccumulator = 0.0f;
	m_bPaused = false;
}

ParticleSystem::~ParticleSystem()
{
	
}

IComponent& ParticleSystem::operator=(IComponent& _rhs)
{
	ParticleSystem* rhsPS = dynamic_cast<ParticleSystem*>(&_rhs);
	if (rhsPS == nullptr) {
		return *this;
	}

	return operator=(*rhsPS);
}
ParticleSystem& ParticleSystem::operator=(ParticleSystem& _rhs)
{
	if (this != &_rhs) {
		m_bPaused = _rhs.m_bPaused;
		m_eShape = _rhs.m_eShape;
		m_eStyle = _rhs.m_eStyle;
		m_fAccumulator = _rhs.m_fAccumulator;
		m_fcFlags = _rhs.m_fcFlags;
		m_Settings = _rhs.m_Settings;
		m_uiEmissionRate = _rhs.m_uiEmissionRate;

		m_fAccumulator = ceil(m_uiEmissionRate * 0.0167f);
		m_Settings.AliveTime = 0.0f;
		m_Settings.Emission = 0;
	}

	return *this;
}
IComponent* ParticleSystem::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	ParticleSystem* psys = new ParticleSystem(gameObject, transform);
	(*psys) = *this;

	return psys;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void ParticleSystem::Init()
{
	// Set up an initail emission based on the values set for this emitter
	m_fAccumulator = ceil(m_uiEmissionRate * 0.0167f);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void ParticleSystem::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "WorldPosition";
	Property* prop = data->GetProperty(propName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	m_Settings.WorldPosition = Vector3((float)atof(values[0].c_str()), (float)atof(values[1].c_str()), (float)atof(values[2].c_str()));

	propName = "Duration";
	prop = data->GetProperty(propName);
	m_Settings.Duration = (float)atof(prop->value.c_str());

	propName = "AliveTime";
	prop = data->GetProperty(propName);
	m_Settings.AliveTime = (float)atof(prop->value.c_str());

	propName = "LifeTime";
	prop = data->GetProperty(propName);
	m_Settings.Lifetime = (float)atof(prop->value.c_str());

	propName = "StartSpeed";
	prop = data->GetProperty(propName);
	m_Settings.StartSpeed = (float)atof(prop->value.c_str());

	propName = "GravityScale";
	prop = data->GetProperty(propName);
	m_Settings.GravityScale = (float)atof(prop->value.c_str());

	propName = "StartColor";
	prop = data->GetProperty(propName);
	values = Inferno::StringParse(prop->value, ',');
	m_Settings.StartColor.Red = (float)atof(values[0].c_str()); m_Settings.StartColor.Green = (float)atof(values[1].c_str()); 
	m_Settings.StartColor.Blue = (float)atof(values[2].c_str()); m_Settings.StartColor.Alpha = (float)atof(values[3].c_str());

	propName = "EndColor";
	prop = data->GetProperty(propName);
	values = Inferno::StringParse(prop->value, ',');
	m_Settings.EndColor.Red = (float)atof(values[0].c_str()); m_Settings.EndColor.Green = (float)atof(values[1].c_str());
	m_Settings.EndColor.Blue = (float)atof(values[2].c_str()); m_Settings.EndColor.Alpha = (float)atof(values[3].c_str());

	propName = "StartSize";
	prop = data->GetProperty(propName);
	m_Settings.StartSize = (float)atof(prop->value.c_str());

	propName = "EndSize";
	prop = data->GetProperty(propName);
	m_Settings.EndSize = (float)atof(prop->value.c_str());

	propName = "MaxParticles";
	prop = data->GetProperty(propName);
	m_Settings.MaxParticles = (unsigned int)atoi(prop->value.c_str());

	propName = "Emission";
	prop = data->GetProperty(propName);
	m_Settings.Emission = (unsigned int)atoi(prop->value.c_str());

	propName = "ShapeDimensions";
	prop = data->GetProperty(propName);
	values = Inferno::StringParse(prop->value, ',');
	m_Settings.ShapeDimensions = Vector3((float)atof(values[0].c_str()), (float)atof(values[1].c_str()), (float)atof(values[2].c_str()));

	propName = "Flags";
	prop = data->GetProperty(propName);
	m_Settings.Flags = (unsigned int)atoi(prop->value.c_str());

	propName = "EShape";
	prop = data->GetProperty(propName);
	m_eShape = (EMITTER_SHAPE)atoi(prop->value.c_str());

	propName = "EStyle";
	prop = data->GetProperty(propName);
	m_eStyle = (EMITTER_STYLE)atoi(prop->value.c_str());

	propName = "EmissionRate";
	prop = data->GetProperty(propName);
	m_uiEmissionRate = (unsigned int)atoi(prop->value.c_str());

	propName = "Accumulator";
	prop = data->GetProperty(propName);
	m_fAccumulator = (float)atof(prop->value.c_str());

	propName = "Paused";
	prop = data->GetProperty(propName);
	m_bPaused = atoi(prop->value.c_str()) != 0;

	propName = "Texture";
	prop = data->GetProperty(propName);
	SetTexture((ParticleTextures)atoi(prop->value.c_str()));

	propName = "Looping";
	prop = data->GetProperty(propName);
	SetLooping(atoi(prop->value.c_str()) != 0);
}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void ParticleSystem::OnEnable()
{
	InfernoEngine_Graphics::RegisterParticleSystem(this);
}
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void ParticleSystem::OnDisable()
{
	InfernoEngine_Graphics::UnregisterParticleSystem(this);
}

// IN: void
// OUT: void
//
// Update that is called every frame
void ParticleSystem::Update()
{
	//Check if we are over the time we alloted
	if (IsFinished() || m_bPaused)
		return;

	m_Settings.AliveTime += Time::DeltaTime;

	// Check to see if we went over our alloted time, and if so react accordingly
	if (m_Settings.Duration <= m_Settings.AliveTime)
	{
		if (IsLooping())
			m_Settings.AliveTime -= m_Settings.Duration;
		else
		{
			m_Settings.Emission = 0;
			m_fAccumulator = 0.0f;
			return;
		}
	}

	// Update our accumulator based on our delta time
	m_fAccumulator += m_uiEmissionRate * Time::DeltaTime;

	// If we have acquired at least one particle
	if (m_fAccumulator > 1.0f)
	{
		float particlesToEmit = 0.0f;
		// Gather the number whole number portion while returning the remainder and store it back into our accumulator
		m_fAccumulator = modf(m_fAccumulator, &particlesToEmit);

		m_Settings.Emission = (unsigned int)particlesToEmit;
		m_Settings.WorldPosition = transform->GetPosition();
	}
	else
		m_Settings.Emission = 0;
}

// IN: void
// OUT: void
//
// Resets the particle system and plays it
void ParticleSystem::Play()
{
	m_fAccumulator = ceil(m_uiEmissionRate * 0.0167f);
	m_Settings.AliveTime = 0.0f;
	m_Settings.Emission = 0;
	
	Pause(false);
}

void ParticleSystem::SetShape(EMITTER_SHAPE shape)
{
	m_eShape = shape;
}
void ParticleSystem::SetStyle(EMITTER_STYLE style)
{
	m_eStyle = style;
}
void ParticleSystem::SetDuration(float duration) { m_Settings.Duration = duration; }
void ParticleSystem::SetLooping(bool looping)
{
	if (looping)
		m_Settings.Flags |= Looping;
	else
		m_Settings.Flags &= ~Looping;
}
void ParticleSystem::SetColorOverLife(bool col)
{
	if (col)
		m_Settings.Flags |= ColorOverLife;
	else
		m_Settings.Flags &= ~ColorOverLife;
}
void ParticleSystem::SetSizeOverLife(bool sol)
{
	if (sol)
		m_Settings.Flags |= SizeOverLife;
	else
		m_Settings.Flags &= ~SizeOverLife;
}
void ParticleSystem::UseCollision(bool coll)
{
	if (coll)
		m_Settings.Flags |= Collision;
	else
		m_Settings.Flags &= ~Collision;
}
void ParticleSystem::SetTexture(ParticleTextures texture) { m_Settings.Flags |= (int)texture << 24; }
void ParticleSystem::SetLifetime(float lifetime) { m_Settings.Lifetime = lifetime; }
void ParticleSystem::SetStartSpeed(float speed) { m_Settings.StartSpeed = speed; }
void ParticleSystem::SetGravityScale(float scale) { m_Settings.GravityScale = scale; }
void ParticleSystem::SetStartColor(Color col) { m_Settings.StartColor = col; }
void ParticleSystem::SetEndColor(Color col) { m_Settings.EndColor = col; }
void ParticleSystem::SetStartSize(float size) { m_Settings.StartSize = size; }
void ParticleSystem::SetEndSize(float size) { m_Settings.EndSize = size; }
void ParticleSystem::SetMaxParticles(unsigned int max) { m_Settings.MaxParticles = max; }
void ParticleSystem::SetEmissionRate(unsigned int rate) { m_uiEmissionRate = rate; }
void ParticleSystem::SetBoxDimensions(Vector3 dim) { m_Settings.ShapeDimensions = dim; }
void ParticleSystem::SetSphereDimensions(float radius)
{
	if (radius >= 0.0f)
		m_Settings.ShapeDimensions.x = radius;
	else
		m_Settings.ShapeDimensions.x = 0.0f;
}
void ParticleSystem::SetHemisphereDimensions(float radius) { SetSphereDimensions(radius); }
void ParticleSystem::SetCircleDimensions(float radius) { SetSphereDimensions(radius); }
void ParticleSystem::SetConeDimensions(float baseRadius, float topRadius, float height)
{
	if (baseRadius < 0.0f)
		baseRadius = 0.0f;

	if (topRadius < baseRadius)
		topRadius = baseRadius;

	if (height < 0.01f)
		height = 0.01f;

	m_Settings.ShapeDimensions = Vector3(baseRadius, topRadius, height);
}
void ParticleSystem::Pause(bool pause) { m_bPaused = pause; }

EMITTER_SHAPE ParticleSystem::GetShape() const { return m_eShape; }
EMITTER_STYLE ParticleSystem::GetStyle() const { return m_eStyle; }
float ParticleSystem::GetDuration() const { return m_Settings.Duration; }
bool ParticleSystem::IsLooping() const { return (m_Settings.Flags & Looping) != 0; }
bool ParticleSystem::GetColorOverTime() const { return (m_Settings.Flags & ColorOverLife) != 0; }
bool ParticleSystem::GetSizeOverTime() const { return (m_Settings.Flags & SizeOverLife) != 0; }
bool ParticleSystem::IsCollidable() const { return (m_Settings.Flags & Collision) != 0; }
float ParticleSystem::GetLifetime() const { return m_Settings.Lifetime; }
float ParticleSystem::GetStartSpeed() const { return m_Settings.StartSpeed; }
float ParticleSystem::GetGravityScale() const { return m_Settings.GravityScale; }
Color ParticleSystem::GetStartColor() const { return m_Settings.StartColor; }
Color ParticleSystem::GetEndColor() const { return m_Settings.EndColor; }
float ParticleSystem::GetStartSize() const { return m_Settings.StartSize; }
float ParticleSystem::GetEndSize() const { return m_Settings.EndSize; }
unsigned int ParticleSystem::GetMaxParticles() const { return m_Settings.MaxParticles; }
unsigned int ParticleSystem::GetEmissionRate() const { return m_uiEmissionRate; }
Vector3 ParticleSystem::GetDimensions() const { return m_Settings.ShapeDimensions; }

bool ParticleSystem::IsPaused() const { return m_bPaused; }
bool ParticleSystem::IsFinished() const
{
	if (m_Settings.Flags & Looping || m_Settings.Duration > m_Settings.AliveTime)
		return false;

	return true;
}