#include "Components\Light.h"
#include "../../Interface/Public/ModuleInterface.h"
#include "GlobalIncludes.h"

Light::Light(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
}

Light::~Light()
{
}

IComponent& Light::operator=(IComponent& _rhs) 
{ 
	Light* rhsLight = dynamic_cast<Light*>(&_rhs);
	if (rhsLight == nullptr) {
		return *this;
	}

	return operator=(*rhsLight);
}

Light& Light::operator=(const Light& rhs)
{
	m_f4ColorAndIntensity = rhs.m_f4ColorAndIntensity;
	m_fRadius = rhs.m_fRadius;

	return *this;
}
IComponent* Light::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	Light* newLight = new Light(gameObject, transform);
	(*newLight) = *this;

	return newLight;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void Light::Init()
{
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Light::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Color";
	Property* prop = data->GetProperty(propName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	SetColor(Vector3((float)atof(values[0].c_str()), (float)atof(values[1].c_str()), (float)atof(values[2].c_str())));

	propName = "Intensity";
	prop = data->GetProperty(propName);
	SetIntensity((float)atof(prop->value.c_str()));

	propName = "Radius";
	prop = data->GetProperty(propName);
	SetRadius((float)atof(prop->value.c_str()));
}

// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void Light::OnEnable()
{
	IComponent::OnEnable();
	InfernoEngine_Graphics::RegisterLightComponent(this);
}
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void Light::OnDisable()
{
	IComponent::OnDisable();
	InfernoEngine_Graphics::UnregisterLightComponent(this);
}

void Light::SetColor(Vector3 color)
{
	m_f4ColorAndIntensity.x = color.x;
	m_f4ColorAndIntensity.y = color.y;
	m_f4ColorAndIntensity.z = color.z;
}
void Light::SetColor(Vector4 color)
{
	m_f4ColorAndIntensity.x = color.x;
	m_f4ColorAndIntensity.y = color.y;
	m_f4ColorAndIntensity.z = color.z;
}
void Light::SetIntensity(float intensity)
{
	m_f4ColorAndIntensity.w = intensity;
}
void Light::SetRadius(float radius) { m_fRadius = radius; }

Vector3 Light::GetColor() const { return Vector3(m_f4ColorAndIntensity); }
float Light::GetIntensity() const { return m_f4ColorAndIntensity.w; }
float Light::GetRadius() const { return m_fRadius; }