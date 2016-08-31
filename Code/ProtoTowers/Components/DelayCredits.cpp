#include "stdafx.h"
#include "DelayCredits.h"
#include "ModuleInterface.h"

DelayCredits::DelayCredits(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	
}

DelayCredits::~DelayCredits() 
{

}

IComponent& DelayCredits::operator=(IComponent&)
{
	return *this;
}

IComponent* DelayCredits::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	DelayCredits* newDC = new DelayCredits(gameObject, transform);
	(*newDC) = *this;

	return newDC;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void DelayCredits::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{

}

void DelayCredits::OnEnable()
{
	m_Timer.SetTime(3.0f);
	m_Timer.Reset();
}

void DelayCredits::Update()
{
	if (m_Timer.IsFinished()){
		InfernoEngine_SceneManagement::LoadScene("CreditsMenu");
	}
}