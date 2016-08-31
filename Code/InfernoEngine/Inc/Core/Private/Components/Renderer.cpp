#include "Components\Renderer.h"
#include "MathLib.h"
#include "GameObject.h"

Renderer::Renderer(GameObject* const gameObject, Transform* const transform) : IComponent(gameObject, transform)
{
	m_fDistFromCamera = 0.0f;
	m_bAlphaBlending = false;
}

Renderer::~Renderer() {}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void Renderer::Init()
{

}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void Renderer::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){

}

// IN: Color - color to use
// OUT: void
//
// Sets the objects color to the incoming color
void Renderer::SetColor(Color newColor)
{
	m_cColor = newColor;

	m_cColor.Alpha = Clamp(0.0f, 1.0f, m_cColor.Alpha);
	m_cColor.Red = Clamp(0.0f, 1.0f, m_cColor.Red);
	m_cColor.Green = Clamp(0.0f, 1.0f, m_cColor.Green);
	m_cColor.Blue = Clamp(0.0f, 1.0f, m_cColor.Blue);
}

bool Renderer::IsStatic() const {
	return gameObject->IsStatic();
}
bool Renderer::IsBlendingRenderer() const {
	return m_bAlphaBlending;
}