#include "Rendering\Effect.h"

Effect::Effect() : m_iPixelShader(-1), m_iBlendState(-1), m_iRasterizerState(-1), m_vSamplerStates(1)
{}

Effect::~Effect()
{}

// IN: int - shader id
// OUT: void
//
// Sets the current shader id
void Effect::SetPixelShader(int index) { m_iPixelShader = index; }


// IN: int - state id
// OUT: void
//
// Sets the current state id
void Effect::SetRasterizerState(int index) { m_iRasterizerState = index; }
// IN: int - state id
// OUT: void
//
// Sets the current state id
void Effect::SetBlendState(int index) { m_iBlendState = index; }
// IN: int - sampler state id to add
// OUT: void
//
// Adds a sampler state id to the sampler state list
void Effect::AddSamplerState(int index)
{
	m_vSamplerStates.push_back(index);
}

// IN: void
// OUT: int - shader id
//
// Returns the current shader id
int Effect::GetPixelShaderID() const { return m_iPixelShader; }

// IN: void
// OUT: int - state id
//
// Returns the current state id
int Effect::GetRasterizerStateID() const { return m_iRasterizerState; }
// IN: void
// OUT: int - state id
//
// Returns the current state id
int Effect::GetBlendStateID() const { return m_iBlendState; }
// IN: void
// OUT: std::vector<int>& - list of sampler states
//
// Returns the list of sampler states
const std::vector<int>& Effect::GetSamplerStateIDs() { return m_vSamplerStates; }