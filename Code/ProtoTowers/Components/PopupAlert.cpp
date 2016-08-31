#include "stdafx.h"
#include "PopupAlert.h"

#include "GlobalIncludes.h"

// ===== Constructor / Destructor ===== //
PopupAlert::PopupAlert(GameObject* _gameObject, Transform* _transform) : IComponent(_gameObject, _transform)
{
	m_pTimer = new InfernoTimer;
}

PopupAlert::~PopupAlert()
{
	delete m_pTimer;
}
// ==================================== //

// ===== Interface ===== //
void PopupAlert::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;
	std::string propName;
	Property* prop;

	propName = "Direction";
	prop = data->GetProperty(propName);
	std::vector<std::string> splitValues = Inferno::StringParse(prop->value, ',');
	m_v3Direction.x = (float)atof(splitValues[0].c_str());
	m_v3Direction.y = (float)atof(splitValues[1].c_str());
	m_v3Direction.z = (float)atof(splitValues[2].c_str());

	propName = "Speed";
	prop = data->GetProperty(propName);
	SetSpeed((float)atof(prop->value.c_str()));

	propName = "AliveTime";
	prop = data->GetProperty(propName);
	SetAliveTime((float)atof(prop->value.c_str()));
}

void PopupAlert::OnEnable()
{
	m_pTimer->Reset();
}

void PopupAlert::Update()
{
	// === Update the Position
	transform->SetPosition(transform->GetPosition() + m_v3Direction * m_fSpeed * Time::DeltaTime);

	if (m_pTimer->IsFinished()) {
		Inferno::Destroy(this->gameObject);
	}
}
// ===================== //

// ===== Protected Interface ===== //
IComponent& PopupAlert::operator=(IComponent& _rhs)
{
	PopupAlert* rhsPA = dynamic_cast<PopupAlert*>(&_rhs);
	if (rhsPA == nullptr) {
		return *this;
	}

	return operator=(*rhsPA);
}

PopupAlert& PopupAlert::operator=(const PopupAlert& _rhs)
{
	if (this != &_rhs) {
		m_fcFlags = _rhs.m_fcFlags;
		m_fSpeed = _rhs.m_fSpeed;
		*m_pTimer = *_rhs.m_pTimer;
		m_v3Direction = _rhs.m_v3Direction;
	}

	return *this;
}

IComponent* PopupAlert::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	PopupAlert* newPA = new PopupAlert(_gameObject, _transform);
	*newPA = *this;

	return newPA;
}
// =============================== //

// ===== Mutators ===== //
void PopupAlert::SetDirection(Vector3 _direction)
{
	m_v3Direction = _direction;
}

void PopupAlert::SetDirection(float _xDir, float _yDir, float _zDir)
{
	m_v3Direction = Vector3(_xDir, _yDir, _zDir);
}

void PopupAlert::SetAliveTime(float _seconds)
{
	m_pTimer->SetTime(_seconds);
	m_pTimer->Reset();
}

void PopupAlert::SetSpeed(float _speed)
{
	m_fSpeed = _speed;
}
// ==================== //