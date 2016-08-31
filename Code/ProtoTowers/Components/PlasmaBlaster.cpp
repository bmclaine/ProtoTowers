//Author: Zane Lewis
//
//Player's main weapon
#include "stdafx.h"
#include "PlasmaBlaster.h"
#include "Entity.h"
#include "BaseProjectile.h"
#include "Components\SkinnedMeshRenderer.h"
#include "../Components/SpringArm.h"
#include "Components\Transform.h"
#include "../InfernoUI.h"
#include "Engine.h"
#include "InfernoTimer.h"

#define BLAST_BULLET_LEVEL 4
#define PROJECTILE_OFFSET 0.75f
#define STUN_CHANCE 15
#define UI_TIMER 3.5f

PlasmaBlaster::PlasmaBlaster(GameObject* const _gameObject, Transform* const _transform) : BaseWeapon(_gameObject, _transform)
{
	m_pSkinnedMeshRenderer = nullptr;
	m_mOffsetMatrix = Matrix4::Scale(Vector3(0.75f, 0.75f, 0.75f)) * Matrix4::RotationZ(0.0f) * Matrix4::RotationX(0.0f) * Matrix4::RotationY(0.0f) * Matrix4::Translate(Vector3(0.0f, 0.0f, 0.0f));
	m_projectileCount = 1;

	m_LevelUpTimer.SetTime(UI_TIMER);
	m_LevelUpTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&PlasmaBlaster::DisableLevelUpUI, this));

	m_LevelDownTimer.SetTime(UI_TIMER);
	m_LevelDownTimer.SetOnTimerEndFunction(BindOnTimerEndFunction(&PlasmaBlaster::DisableLevelDownUI, this));

	ResetLevel();
}

PlasmaBlaster::~PlasmaBlaster(){

}

IComponent& PlasmaBlaster::operator=(IComponent& _rhs)
{
	PlasmaBlaster* rhsPB = dynamic_cast<PlasmaBlaster*>(&_rhs);
	if (rhsPB == nullptr) {
		return *this;
	}

	return operator=(*rhsPB);
}

PlasmaBlaster& PlasmaBlaster::operator=(const PlasmaBlaster& _rhs)
{
	if (this != &_rhs) {
		m_mOffsetMatrix = _rhs.m_mOffsetMatrix;
		m_projectileCount = _rhs.m_projectileCount;
		m_LevelUpTimer.SetTime(_rhs.m_LevelUpTimer.GetSetTimeSeconds());

		BaseWeapon::operator=(_rhs);
	}

	return *this;
}

IComponent* PlasmaBlaster::CreateCopy(GameObject* const _gameobject, Transform* const _transform)
{
	PlasmaBlaster* newComponent = new PlasmaBlaster(_gameobject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

void PlasmaBlaster::Init()
{
	m_pSkinnedMeshRenderer = gameObject->transform->GetParent()->gameObject->GetComponentInChildren<SkinnedMeshRenderer>(true);
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void PlasmaBlaster::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	BaseWeapon::PostInit(objectMap, dataMap);
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	ResetLevel();
}

void PlasmaBlaster::FixedUpdate()
{
	Recharge();

	UpdateLocation();
}

bool PlasmaBlaster::FireProjectile(Entity* _owningObject)
{
	if (!m_FireRate.IsFinished()) return false;

	Transform* pTran = transform->GetParent();
	Vector3 firePosition = Vector3::Zero();
	float xThing = pTran->Forward().x;
	float multiplier = xThing < 0 ? 1.0f : -1.0f;

	Vector3 direction(pTran->Forward().x, m_pRotationTransfrom->transform->Forward().y + 0.025f, pTran->Forward().z + (.05f * multiplier));
	direction.Normalize();
	firePosition = transform->GetPosition() + direction;

	GameObject* projectile = Inferno::Instantiate("BaseProjectile");
	BaseProjectile* proj = projectile->GetComponent<BaseProjectile>();
	proj->SetAddEffect(m_Level >= BLAST_BULLET_LEVEL);
	proj->FireProjectile(firePosition, direction);
	proj->SetDamage(m_currentDamage);
	proj->SetOwningEntity(_owningObject);

	MuzzleFlash();

	Reset();

	return true;
}

bool PlasmaBlaster::FireSecondProjectile(Entity* _owningEntity){

	if ((!m_SecondFireRate.IsFinished() || m_bIsOverheated) && m_bBuffed == false) return false;

	if (m_iNumRoundsLeft < 0){
		Inferno::PostAudioEventAtObjectPosition(PLASME_BLASTER_OUT_OF_AMMO, gameObject);
		m_SecondFireRate.Reset();
		m_SecondFireRate.SetTime(m_currentFireRate);
		m_bIsOverheated = true;
		return true;
	}

	Transform* pTran = transform->GetParent();
	Vector3 firePosition = Vector3::Zero();
	float xThing = pTran->Forward().x;
	float multiplier = xThing < 0 ? 1.0f : -1.0f;

	Vector3 direction(pTran->Forward().x, m_pRotationTransfrom->transform->Forward().y + 0.025f, pTran->Forward().z + (.05f * multiplier));
	direction.Normalize();
	firePosition = transform->GetPosition() + direction;

	for (int i = 0; i < m_projectileCount; ++i){
		if (m_Level == 5){
			if (i == 0)
				firePosition += (pTran->Right() * PROJECTILE_OFFSET);
			else if (i == 1)
				firePosition += (pTran->Right() * -PROJECTILE_OFFSET);
		}

		GameObject* projectile = Inferno::Instantiate("SecondaryProjectile");
		BaseProjectile* proj = projectile->GetComponent<BaseProjectile>();
		proj->SetAddEffect(m_Level >= BLAST_BULLET_LEVEL);
		proj->FireProjectile(firePosition, direction);
		proj->SetDamage(m_currentDamage);
		proj->SetOwningEntity(_owningEntity);
		proj->SetStunChance(STUN_CHANCE);
	}

	if (m_bBuffed == false)
		--m_iNumRoundsLeft;

	if (m_EnergyUpdateFunc)
		m_EnergyUpdateFunc(GetEnergyLevel());

	MuzzleFlash();

	ResetSecondary();

	return true;
}

void PlasmaBlaster::Recharge(void)
{
	if (m_RechargeDelay.IsFinished() && m_RechargeRate.IsFinished() && m_iNumRoundsLeft < m_iMaxNumRounds)
	{
		m_RechargeRate.Reset();
		m_RechargeRate.SetTime(RECHARGE_RATE);
		m_iNumRoundsLeft++;
		m_EnergyUpdateFunc(GetEnergyLevel());
		if (m_iNumRoundsLeft > OVERHEAT_LIMIT)
			m_bIsOverheated = false;
		return;
	}
}

void PlasmaBlaster::UpdateLocation()
{
	Matrix4 boneWorldMatrix = m_pSkinnedMeshRenderer->GetBoneWorld(RIFLE_BONE_INDEX);
	transform->SetLocalMatrix(m_mOffsetMatrix * boneWorldMatrix);
}

void PlasmaBlaster::Reset(){
	m_FireRate.Reset();
	m_FireRate.SetTime(FIRE_RATE* m_fFireRateScalar);
}

void PlasmaBlaster::ResetSecondary(){
	m_SecondFireRate.Reset();
	m_RechargeDelay.Reset();
	m_SecondFireRate.SetTime(m_currentFireRate* m_fSecFireRateScalar);
	m_RechargeDelay.SetTime(RECHARGE_DELAY);
}

void PlasmaBlaster::LevelUp() {
	BaseWeapon::LevelUp();
	switch (m_Level)
	{
	case 2:
		m_currentDamage = 35;
		Inferno::PostAudioEvent(PLAYER_LEVEL_UP_1);
		SetLevelUpText("Weapon Damage Increased");
		break;
	case 3:
		m_currentFireRate = 0.095f;
		m_SecondFireRate.SetTime(m_currentFireRate);
		m_FireRate.SetTime(0.125f);
		Inferno::PostAudioEvent(PLAYER_LEVEL_UP_1);
		SetLevelUpText("Fire Rate Increased");
		break;
	case 4:
		Inferno::PostAudioEvent(PLAYER_LEVEL_UP_2);
		SetLevelUpText("Blast Bullet Upgrade");
		break;
	case 5:
		m_projectileCount = 2;
		Inferno::PostAudioEvent(PLAYER_LEVEL_UP_3);
		SetLevelUpText("Spreadshot Upgrade");
		break;
	default:
		break;
	}

	EnableLevelUpUI();

	CreateLevelUpParticles();
}

void PlasmaBlaster::LevelDown(){

	if (m_Level == 1) return;

	--m_Level;
	SetKillCount(0);

	switch (m_Level){
	case 1:
		m_currentDamage = SECONDARY_BASE_DAMAGE;
		SetLevelDownText("Damage Decreased");
		break;
	case 2:
		m_currentFireRate = SECONDARY_FIRE_RATE;
		m_FireRate.SetTime(FIRE_RATE);
		m_SecondFireRate.SetTime(m_currentFireRate);
		SetLevelDownText("Fire rate Decreased");
		break;
	case 3:
		SetLevelDownText("Blast Bullet Disabled");
		break;
	case 4:
		m_projectileCount = 1;
		SetLevelDownText("SpreadShot Disabled");
		break;
	}

	EnableLevelDownUI();

	CreateLevelDownParticles();

	Inferno::PostAudioEventAtObjectPosition(PLAYER_LOSE_LEVEL, gameObject);
}

void PlasmaBlaster::ResetLevel(){
	BaseWeapon::ResetLevel();
	m_FireRate.SetTime(FIRE_RATE);
	m_SecondFireRate.SetTime(SECONDARY_FIRE_RATE);
	m_projectileCount = 1;
	m_currentDamage = SECONDARY_BASE_DAMAGE;
	m_currentFireRate = SECONDARY_FIRE_RATE;
}

void PlasmaBlaster::MuzzleFlash(){

	GameObject* muzzleFlash = Inferno::Instantiate("MuzzleFlash");
	PlasmaBlaster* blaster = gameObject->GetComponent<PlasmaBlaster>();
	muzzleFlash->transform->SetParent(blaster->transform);
	muzzleFlash->transform->Translate(Vector3(0.0f, 1.75f, 0.0f));
	muzzleFlash->GetComponent<ParticleSystem>()->Play();
	Inferno::Destroy(muzzleFlash, 0.2f);
}

void PlasmaBlaster::CreateLevelUpParticles(){
	Vector3 position = transform->GetParent()->GetPosition();
	GameObject* particleObject = Inferno::GetNewGameObject();
	particleObject->transform->SetPosition(position);
	ParticleSystem* particles = particleObject->AddComponent<ParticleSystem>();
	particles->SetShape(EMITTER_SHAPE_CIRCLE);
	particles->SetStyle(EMITTER_STYLE_EDGE);
	particles->SetSphereDimensions(1.5f);
	particles->SetEmissionRate(200);
	particles->SetDuration(0.8f);
	particles->SetLifetime(1.0f);
	particles->SetStartSpeed(0.5f);
	particles->SetStartSize(0.15f);
	particles->SetEndSize(0.15f);
	particles->SetEndColor(Color(1, 0, 0, 1));
	particles->SetStartColor(Color(1, 0, 0.5, 1));
	particles->Play();
	Inferno::Destroy(particleObject, particles->GetDuration());

	GameObject* particleObjectCone = Inferno::GetNewGameObject();
	particleObjectCone->transform->SetPosition(position);
	particles = particleObjectCone->AddComponent<ParticleSystem>();
	particles->SetShape(EMITTER_SHAPE_CONE);
	particles->SetStyle(EMITTER_STYLE_INNER);
	particles->SetConeDimensions(0.8f, 1.2f, 2.0f);
	particles->SetEmissionRate(200);
	particles->SetDuration(0.8f);
	particles->SetLifetime(1.0f);
	particles->SetStartSpeed(4.0f);
	particles->SetStartSize(0.15f);
	particles->SetEndSize(0.15f);
	particles->SetEndColor(Color(1, 0, 0, 1));
	particles->SetStartColor(Color(1, 0, 0.5, 1));
	particles->Play();
	Inferno::Destroy(particleObjectCone, particles->GetDuration());
}

void PlasmaBlaster::CreateLevelDownParticles(){
	Vector3 position = transform->GetParent()->GetPosition();
	GameObject* particleObject = Inferno::GetNewGameObject();
	particleObject->transform->SetPosition(position);
	ParticleSystem* particles = particleObject->AddComponent<ParticleSystem>();
	particles->SetShape(EMITTER_SHAPE_CIRCLE);
	particles->SetStyle(EMITTER_STYLE_EDGE);
	particles->SetSphereDimensions(1.5f);
	particles->SetEmissionRate(200);
	particles->SetDuration(0.8f);
	particles->SetLifetime(1.0f);
	particles->SetStartSpeed(0.5f);
	particles->SetStartSize(0.15f);
	particles->SetEndSize(0.15f);
	particles->SetStartColor(Color(0, 0, 1, 1));
	particles->SetEndColor(Color(0.5f, 0.0f, 1, 1));
	particles->Play();
	Inferno::Destroy(particleObject, particles->GetDuration());

	GameObject* particleObjectCone = Inferno::GetNewGameObject();
	particleObjectCone->transform->SetPosition(position + Vector3(0.0f, 2.5f, 0.0f));
	particles = particleObjectCone->AddComponent<ParticleSystem>();
	particles->SetShape(EMITTER_SHAPE_CONE);
	particles->SetStyle(EMITTER_STYLE_INNER);
	particles->SetConeDimensions(0.8f, 1.2f, 2.0f);
	particles->SetEmissionRate(200);
	particles->SetDuration(0.8f);
	particles->SetLifetime(1.0f);
	particles->SetGravityScale(1.2f);
	particles->SetStartSpeed(-4.0f);
	particles->SetStartSize(0.15f);
	particles->SetEndSize(0.15f);
	particles->SetStartColor(Color(0, 0, 1, 1));
	particles->SetEndColor(Color(0.5f, 0.0, 1, 1));
	particles->Play();
	Inferno::Destroy(particleObjectCone, particles->GetDuration());
}

void PlasmaBlaster::OnEnable(){
	ToggleLevelUpUI(false);
	ToggleLevelDownUI(false);
	ResetLevel();
	SetLevelUpBar();
}

void PlasmaBlaster::ToggleLevelUpUI(bool value){

	UIElement* m_pLevelUpUI = Inferno_UI::GetUIElementByName(std::string("LevelUp"), std::string("HUD"));

	if (m_pLevelUpUI)
		m_pLevelUpUI->SetActive(value);
}

void PlasmaBlaster::ToggleLevelDownUI(bool value){
	UIElement* element = Inferno_UI::GetUIElementByName(std::string("LevelDown"), std::string("HUD"));
	if (element)
		element->SetActive(value);
}

void PlasmaBlaster::DisableLevelUpUI(){
	ToggleLevelUpUI(false);
}

void PlasmaBlaster::EnableLevelUpUI(){
	ToggleLevelUpUI(true);
	m_LevelUpTimer.Reset();
	ToggleLevelDownUI(false);
}

void PlasmaBlaster::SetLevelUpText(const char* text){
	UIElement* m_pLevelUpUI = Inferno_UI::GetUIElementByName(std::string("LevelUp"), std::string("HUD"));

	if (m_pLevelUpUI){
		UIElement* textElement = m_pLevelUpUI->GetTransform()->GetChildByName(std::string("LevelUpText"));
		if (textElement){
			textElement->GetComponent<UIText>()->SetText(std::string(text));
		}
	}
}

void PlasmaBlaster::SetLevelDownText(const char* text){
	UIElement* element = Inferno_UI::GetUIElementByName(std::string("LevelDown"), std::string("HUD"));

	if (element){
		UIElement* textElement = element->GetTransform()->GetChildByName(std::string("LevelDownText"));
		if (textElement){
			textElement->GetComponent<UIText>()->SetText(std::string(text));
		}
	}
}

void PlasmaBlaster::DisableLevelDownUI(){
	ToggleLevelDownUI(false);
}

void PlasmaBlaster::EnableLevelDownUI(){
	ToggleLevelDownUI(true);
	m_LevelDownTimer.Reset();
	ToggleLevelUpUI(false);
}






