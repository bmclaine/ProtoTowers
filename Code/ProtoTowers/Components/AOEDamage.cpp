#include "stdafx.h"
#include "AOEDamage.h"
#include "ModuleInterface.h"
#include "Components\Transform.h"
#include "GameObject.h"
#include "Entity.h"
#include "Physics\Collider.h"
#include "Engine.h"
#include "AIEntity.h"

AOEDamage::AOEDamage(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform){
	m_fDamage = 0;
	m_fRange = 0;
}

AOEDamage::~AOEDamage(){

}

IComponent& AOEDamage::operator=(IComponent& _rhs) {

	AOEDamage* aoe = dynamic_cast<AOEDamage*>(&_rhs);
	if (aoe == nullptr) {
		return *this;
	}

	return operator=(*aoe);
}

AOEDamage& AOEDamage::operator=(const AOEDamage& _rhs){

	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_fDamage = _rhs.m_fDamage;
		m_fRange = _rhs.m_fRange;
	}

	return *this;
}

IComponent* AOEDamage::CreateCopy(GameObject* const _gameObject, Transform* const _transform){

	AOEDamage* aoe = new AOEDamage(_gameObject, _transform);
	*aoe = *this;

	return aoe;
}

void AOEDamage::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Damage";
	Property* prop = data->GetProperty(propName);
	m_fDamage = (float)atof(prop->value.c_str());

	propName = "Range";
	prop = data->GetProperty(propName);
	m_fRange = (float)atof(prop->value.c_str());
}

void AOEDamage::OnEnable(){
	IComponent::OnEnable();

	std::vector<ParticleSystem*>& components = gameObject->GetComponents<ParticleSystem>();
	for (unsigned int i = 0; i < (unsigned int)components.size(); ++i){
		components[i]->Play();
	}
}

void AOEDamage::OnDisable(){
	IComponent::OnDisable();
}

void AOEDamage::Cast(){
	std::vector<RayCastHit> hitInfo;
	InfernoEngine_Physics::SphereOverlap(transform->GetPosition(), m_fRange, hitInfo);

	for (unsigned int i = 0; i < (unsigned int)hitInfo.size(); ++i){
		Collider* collider = hitInfo[i].collider;
		if (collider->gameObject->GetLayer() != (unsigned int)CollisionLayers::LAYER_ENEMY) continue;
		AIEntity* entity = collider->gameObject->GetComponent<AIEntity>();
		if (entity){
			float damage = m_fDamage * (entity != nullptr && entity->IsBuffed()) ? 0.5f : 1.0f;
			entity->TakeDamage(DamageInfo(damage));
		}
	}

	Inferno::Destroy(gameObject, 1.5f);
}