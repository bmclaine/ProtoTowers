#include "Stdafx.h"
#include "MinionSpawnerShield.h"

#include "BaseProjectile.h"
#include "EventSystem.h"

// ===== Constructor / Destructor ===== //
MinionSpawnerShield::MinionSpawnerShield(GameObject* _gameObject, Transform* _transform) :IComponent(_gameObject, _transform)
{
	m_bCPShield = false;
}

MinionSpawnerShield::~MinionSpawnerShield()
{

}
// ==================================== //

// ===== Interface ===== //
void MinionSpawnerShield::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "CP Shield";
	Property* prop = data->GetProperty(propName);

	if (prop != nullptr) {
		m_bCPShield = ((int)atoi(prop->value.c_str())) == 0 ? false : true;
	}
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& MinionSpawnerShield::operator=(IComponent& _rhs)
{
	return *this;
}

MinionSpawnerShield& MinionSpawnerShield::operator=(const MinionSpawnerShield& _rhs)
{
	return *this;
}

IComponent* MinionSpawnerShield::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	MinionSpawnerShield* newMSS = new MinionSpawnerShield(gameObject, transform);

	return newMSS;
}
// =============================== //

// ===== Collision Events ===== //
void MinionSpawnerShield::OnTriggerEnter(Collider* _collider)
{
	BaseProjectile* playerProjectile = _collider->gameObject->GetComponent<BaseProjectile>();
	if (playerProjectile == nullptr) {
		return;
	}

	// === ToolTip
	if (m_bCPShield) {
		EventSystem::GetInstance()->SendEventNow("ToolTip_CPSpawners", nullptr);
	}

	GameObject* popUp = Inferno::Instantiate("NoDamageAlert");
	popUp->transform->SetPosition(_collider->transform->GetPosition());
}
// ============================ //