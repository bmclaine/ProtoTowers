#include "stdafx.h"
#include "Ability.h"
#include "../Core/PlayerController.h"
#include "GameObject.h"
#include "Assets\Texture.h"

Ability::Ability(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_cActiveTimerColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
	m_fCastTime = 0.0f;
	m_fCooldown = 0.0f;
	m_pController = gameObject->GetComponent<PlayerController>();
	m_pTexture = nullptr;
}

Ability::~Ability()
{
	SAFE_DELETE(m_pTexture);
}

Ability& Ability::operator=(const Ability& rhs)
{
	m_fCastTime = rhs.m_fCastTime;
	m_fCooldown = rhs.m_fCooldown;
	m_pController = rhs.m_pController;

	if (m_pTexture)
		SAFE_DELETE(m_pTexture);

	if (rhs.m_pTexture)
	{
		m_pTexture = new Texture();
		m_pTexture->SetImage(rhs.m_pTexture->GetImage());
		m_pTexture->SetOffsets(rhs.m_pTexture->GetOffsets());
	}

	return *this;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void Ability::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Ability::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "CastTime";
	Property* prop = data->GetProperty(propName);
	/*m_fCastTime = (float)atof(prop->value.c_str());

	propName = "CoolDownTimer";
	prop = data->GetProperty(propName);
	m_fCooldown = (float)atof(prop->value.c_str());*/

	propName = "PlayerController";
	prop = data->GetProperty(propName);
	int ID = atoi(prop->value.c_str());
	if (ID > 0)
		m_pController = (PlayerController*)objectMap.find(ID)->second;
}

// IN: void
// OUT: void
//
// Reenables controls that were disabled
void Ability::ReenableControls()
{
	// Most abilities should just disable Movement and Shooting, otherwise this can be overrided to re-enable the others
	if (m_pController)
		m_pController->SetIgnore(IGNORE_MOVEMENT | IGNORE_SHOOTING, false);
}

// IN: bool - To show the indicator or not
// OUT: void
//
// Sets the Indicator Circles display status to the incoming bool
void Ability::DisplayIndicatorCircle(bool display) {}

void Ability::SetActiveTimerColor(Color color)
{
	m_cActiveTimerColor = color;
}
void Ability::SetCastTime(float time)
{
	m_fCastTime = time;
}
void Ability::SetCooldown(float time)
{
	m_fCooldown = time;
}
void Ability::SetTexture(Texture* texture)
{
	if (m_pTexture)
		SAFE_DELETE(m_pTexture);

	m_pTexture = new Texture();
	m_pTexture->SetImage(texture->GetImage());
	m_pTexture->SetOffsets(texture->GetOffsets());
}

Color Ability::GetActiveTimerColor() const { return m_cActiveTimerColor; }
float Ability::GetCastTime() const { return m_fCastTime; }
float Ability::GetCastTimeRemaining() const { return m_itCastTimer.TimeRemaining(); }
float Ability::GetCooldown() const { return m_fCooldown; }
float Ability::GetCooldownRemaining() const { return m_itCooldownTimer.TimeRemaining(); }
Texture* Ability::GetTexture() const { return m_pTexture; }
bool  Ability::CanCast() const { return m_bCanCast; }