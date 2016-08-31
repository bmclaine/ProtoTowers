#pragma once

#include <vector>

class Effect
{
private:
	int m_iPixelShader;						// Index to the pixel shader the effect uses

	int m_iRasterizerState;					// Index to which rasterizer state the effect uses
	int m_iBlendState;						// Index to which blend state the effect uses
	std::vector<int> m_vSamplerStates;		// Expanding array that holds the sampler state IDs for this effect

public:
	std::string m_sName;					// Holds the name of the effect to try to avoid duplicates

public:
	Effect();
	~Effect();

	// IN: int - shader id
	// OUT: void
	//
	// Sets the current shader id
	void SetPixelShader(int index);

	// IN: int - state id
	// OUT: void
	//
	// Sets the current state id
	void SetRasterizerState(int index);
	// IN: int - state id
	// OUT: void
	//
	// Sets the current state id
	void SetBlendState(int index);
	// IN: int - sampler state id to add
	// OUT: void
	//
	// Adds a sampler state id to the sampler state list
	void AddSamplerState(int index);


	// IN: void
	// OUT: int - shader id
	//
	// Returns the current shader id
	int GetPixelShaderID() const;

	// IN: void
	// OUT: int - state id
	//
	// Returns the current state id
	int GetRasterizerStateID() const;
	// IN: void
	// OUT: int - state id
	//
	// Returns the current state id
	int GetBlendStateID() const;
	// IN: void
	// OUT: std::vector<int>& - list of sampler states
	//
	// Returns the list of sampler states
	const std::vector<int>& GetSamplerStateIDs();
};