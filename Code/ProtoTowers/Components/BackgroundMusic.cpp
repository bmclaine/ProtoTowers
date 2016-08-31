#include "stdafx.h";
#include "BackgroundMusic.h"
#include "Engine.h"
#include "../Core/WWise_IDs.h"

BackgroundMusic::BackgroundMusic(GameObject* const _gameObject, Transform* const _transform) : IComponent(_gameObject, _transform)
{

}

BackgroundMusic::~BackgroundMusic()
{

}

IComponent& BackgroundMusic::operator=(IComponent& _rhs)
{
	BackgroundMusic* rhsBT = dynamic_cast<BackgroundMusic*>(&_rhs);
	if (rhsBT == nullptr) {
		return *this;
	}

	return operator=(*rhsBT);
}

BackgroundMusic& BackgroundMusic::operator=(const BackgroundMusic& _rhs)
{
	if (this != &_rhs) {
	}

	return *this;
}

void BackgroundMusic::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propertyName = "MusicID";
	Property* prop = data->GetProperty(propertyName);
	m_MusicID = atoi(prop->value.c_str());
}

void BackgroundMusic::Init()
{

}

void BackgroundMusic::OnEnable()
{
	Inferno::StopAllAudio();
	Inferno::RegisterAudioListener(gameObject);
	switch (m_MusicID)
	{
	case 0:
	{
		Inferno::PostAudioEvent(2829132051U);
		break;
	}
	case 1:
	{
		Inferno::PostAudioEvent(3897924658U);
		break;
	}
	case 2:
	{
		Inferno::PostAudioEvent(2727630976U);
		break;
	}
	default:
		break;
	}
	IComponent::OnEnable();
}

void BackgroundMusic::OnDisable()
{
	Inferno::UnregisterAudioListener(gameObject);
	//Inferno::StopAllAudio();
	IComponent::OnDisable();
}

IComponent* BackgroundMusic::CreateCopy(GameObject* const _gameObject, Transform* const _transform)
{
	BackgroundMusic* newComponent = new BackgroundMusic(_gameObject, _transform);
	(*newComponent) = *this;

	return newComponent;
}

void BackgroundMusic::Update()
{
	
}