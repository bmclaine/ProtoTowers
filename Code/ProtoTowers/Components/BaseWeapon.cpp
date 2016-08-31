#include "stdafx.h"
#include "BaseWeapon.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "BaseProjectile.h"
#include "Engine.h"
#include "Components\IComponent.h"
#include "Components\MeshRenderer.h"
#include "InfernoTimer.h"
#include "Physics\SphereCollider.h"
#include "Physics\Rigidbody.h"
#include "3rdPersonCamera.h"
#include "UIComponents\UIRenderer.h"
#include "UIComponents\UIText.h"
#include "UIElement.h"

BaseWeapon::BaseWeapon(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_currentFireRate = 0.25f;
	m_fFireRateScalar = 1.0f;
	m_fSecFireRateScalar = 1.0f;
	m_fShotCostScalar = 1.0f;
	m_iNumRoundsLeft = NUM_PROJECTILES;
	m_iMaxNumRounds = NUM_PROJECTILES; 
	m_FireRate.SetTime(FIRE_RATE);
	m_SecondFireRate.SetTime(SECONDARY_FIRE_RATE);
	m_RechargeDelay.SetTime(RECHARGE_DELAY);
	m_RechargeRate.SetTime(RECHARGE_RATE);
	m_bIsOverheated = false;
	m_bBuffed = false;
	m_pRotationTransfrom = nullptr;
	m_Level = 1;
	m_currentDamage = BASE_DAMAGE;
	m_currentKillCount = 0;
}
BaseWeapon::~BaseWeapon()
{

}
IComponent& BaseWeapon::operator=(IComponent& _rhs)
{
	BaseWeapon* rhsBW = dynamic_cast<BaseWeapon*>(&_rhs);
	if (rhsBW == nullptr) {
		return *this;
	}

	return operator=(*rhsBW);
}

BaseWeapon& BaseWeapon::operator=(const BaseWeapon& _rhs)
{
	if (this != &_rhs) {
		m_RechargeDelay = _rhs.m_RechargeDelay;
		m_FireRate = _rhs.m_FireRate;
		m_RechargeRate = _rhs.m_RechargeRate;
		m_EnergyUpdateFunc = _rhs.m_EnergyUpdateFunc;
		m_iNumRoundsLeft = _rhs.m_iNumRoundsLeft;
		m_iMaxNumRounds = _rhs.m_iMaxNumRounds;
		m_Level = _rhs.m_Level;
		m_currentFireRate = _rhs.m_currentFireRate;
		m_currentDamage = _rhs.m_currentDamage;
		m_fFireRateScalar = _rhs.m_fFireRateScalar;
		m_fShotCostScalar = _rhs.m_fShotCostScalar;
		m_bIsOverheated = _rhs.m_bIsOverheated;
		m_currentKillCount = _rhs.m_currentKillCount;
	}

	return *this;
}

IComponent* BaseWeapon::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	BaseWeapon* newComponent = new BaseWeapon(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void BaseWeapon::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "RotationTransform";
	Property* prop = data->GetProperty(propName);
	int ID = atoi(prop->value.c_str());
	if (ID > 0){
		SetRotationTransform((Transform*)objectMap.find((unsigned int)ID)->second);
	}

}

void BaseWeapon::Update()
{

}

bool BaseWeapon::FireProjectile(Entity* _owningObject)
{
	return false;
}

bool BaseWeapon::FireSecondProjectile(Entity* _owningEntity){
	return false;
}

void BaseWeapon::Recharge(void)
{

}

void BaseWeapon::LevelUp(){
	if (m_Level < MAX_LEVEL){
		++m_Level;
		m_currentKillCount = 0;
	}
}

void BaseWeapon::LevelDown(){
	if (m_Level > 0){
		--m_Level;
		m_currentKillCount = 0;
	}
}

void BaseWeapon::ResetLevel(){
	m_Level = 1;
	m_currentKillCount = 0;

	SetKillCount(m_currentKillCount);
}

float BaseWeapon::GetEnergyLevel() const
{
	float ammo = (float)m_iNumRoundsLeft;
	return ammo / NUM_PROJECTILES;
}

float BaseWeapon::GetFireRateScalar() const
{
	return m_fFireRateScalar;
}

float BaseWeapon::GetShotCostScalar() const
{
	return m_fShotCostScalar;
}


void BaseWeapon::SetEnergyUpdateFunction(EnergyUpdateFunction _func)
{
	m_EnergyUpdateFunc = _func;
}

void BaseWeapon::SetFireRateScalar(float scalar)
{
	m_fFireRateScalar = scalar;
}

void BaseWeapon::SetSecFireRateScalar(float scalar)
{
	m_fSecFireRateScalar = scalar;
}

void BaseWeapon::SetShotCostScalar(float scalar)
{
	m_fShotCostScalar = scalar;
}


void BaseWeapon::SetRotationTransform(Transform * _transform)
{
	m_pRotationTransfrom = _transform;
}

void BaseWeapon::SetMaxAmmo(int _maxAmmo)
{
	m_iMaxNumRounds = _maxAmmo;
}

int BaseWeapon::GetKillCount() const{
	return m_currentKillCount;
}

void BaseWeapon::SetKillCount(int killCount){
	m_currentKillCount = killCount;
	if (m_currentKillCount == MAX_KILLCOUNT * m_Level && m_Level < MAX_LEVEL){
		LevelUp();
	}

	SetLevelUpBar();
	SetLevelIndicator();
}

void BaseWeapon::SetLevelUpBar(){
	UIElement* levelUpBar = Inferno_UI::GetUIElementByName(std::string("LevelUpBar"), std::string("HUD"));
	if (levelUpBar){
		UIRenderer* image = levelUpBar->GetComponent<UIRenderer>();
		image->SetImageFill((float)m_currentKillCount / (float)(MAX_KILLCOUNT * m_Level));
	}
}

void BaseWeapon::SetLevelIndicator(){
	UIElement* levelIndicator = Inferno_UI::GetUIElementByName(std::string("LevelIndicator"), std::string("HUD"));
	if (levelIndicator){
		UIElement* textElement = levelIndicator->GetTransform()->GetChildByName(std::string("LevelText"));
		if (textElement){
			textElement->GetComponent<UIText>()->SetText(std::to_string(m_Level));
		}
	}
}

void BaseWeapon::ToggleWeaponBuff(bool buffed)
{
	m_bBuffed = buffed;

	if (m_bBuffed == true)
	{
		m_iNumRoundsLeft = NUM_PROJECTILES;
		m_bIsOverheated = false;
	}
}
