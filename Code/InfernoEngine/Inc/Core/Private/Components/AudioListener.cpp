#include "Components\AudioListener.h"
#include "../../Interface/Public/Engine.h"

AudioListener::AudioListener(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform){
	m_bRegistered = false;
}

IComponent& AudioListener::operator=(IComponent& _rhs){

	AudioListener* rhsLlstener = dynamic_cast<AudioListener*>(&_rhs);
	if (rhsLlstener == nullptr) {
		return *this;
	}

	return operator=(*rhsLlstener);
}

AudioListener& AudioListener::operator=(const AudioListener& _rhs){
	return *this;
}

IComponent* AudioListener::CreateCopy(GameObject* const gameObject, Transform* const transform){
	AudioListener* listener = new AudioListener(gameObject, transform);
	return listener;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void AudioListener::Init(){

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void AudioListener::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

}

void AudioListener::OnEnable(){
	if (!m_bRegistered){
		Inferno::RegisterAudioListener(gameObject);
		m_bRegistered = true;
	}
}
