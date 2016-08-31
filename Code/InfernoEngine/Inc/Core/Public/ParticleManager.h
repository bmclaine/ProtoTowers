#pragma once

#include "Registry.h"
#include "MathLib.h"

typedef unsigned int UINT;

#define MAX_PARTICLES 30208
#define MAX_COARSE_TILES_X 16
#define MAX_COARSE_TILES_Y 8
#define CULLING_TILE_SIZE_X 32
#define CULLING_TILE_SIZE_Y 32
#define MAX_CULLING_TILES MAX_COARSE_TILES_X * MAX_COARSE_TILES_Y
#define PARTICLES_PER_TILE_BUFFER 1024

struct ParticlePartA
{
	Vector3 Position;
	UINT    Flags;

	Vector3 Velocity;
	float   Age;

	Vector4 Color;
};
struct ParticlePartB
{
	Vector4 StartColor;
	Vector4 EndColor;

	float   Lifetime;
	float   DistanceToEye;
	UINT    IsSleeping;
	float   GravityScale;

	float   StartSize;
	float   EndSize;
	Vector2 PartBPad;
};

class ParticleSystem;

class ParticleManager : public Registry<ParticleSystem>
{
private:
	struct FrameInformation
	{
		Vector4 CameraPostion;
		float   DeltaTime;
		UINT    ScreenWidth;
		UINT    ScreenHeight;
		float   pad;
	};

	struct TilingConstantBuffer
	{
		UINT NumTilesX;
		UINT NumTilesY;
		UINT NumCoarseCullingTilesX;
		UINT NumCoarseCullingTilesY;

		UINT NumCullingTilesPerCoarseTileX;
		UINT NumCullingTilesPerCoarseTileY;
		UINT TilingPads[2];
	};

private:
	FrameInformation m_FrameInfo;
	TilingConstantBuffer m_TilingConstantBuffer;
	std::vector<std::vector<int>> m_vComputeShaders;
	std::vector<int> m_vImageIds;

	// Needed resources for Emitting
	int m_iPartAUAV;
	int m_iPartBUAV;
	int m_iDeadListUAV;
	int m_iRandTexture;
	int m_iSamplerID;
	int m_iEmitterSettingsBuffer;
	int m_iEmitterRotationBuffer;
	int m_iDeadCountBuffer;

	// Additional resources for simulating
	int m_iParticleSimulateComputeShader;
	int m_iParticleIndexUAV;
	int m_iParticleViewSpaceUAV;
	int m_iParticleDrawArgsUAV;
	int m_iFrameInformationBuffer;
	int m_iAliveBuffer;
	
	// Additional resources for rendering
	int m_iVertexShader;
	int m_iGeometryShader;
	int m_iPixelShader;
	int m_iParticleDrawArgsBuffer;
	int m_iBlendState;
	int m_iPartASRV;
	int m_iViewSpaceSRV;
	int m_iParticleIndexSRV;
	int m_iRasterizerState;
	int m_iDepthTexture;

	// Additional resources for Coarse Culling
	int m_iCoarseCullingComputeShader;
	int m_iCoarseTiledIndexBufferUAV;
	int m_iCoarseTiledBufferCountersUAV;
	int m_iTilingConstantBuffer;

	// Additional resources for Fine Grained Culling
	int m_iCullingComputeShader;
	int m_iCoarseTiledIndexBufferSRV;
	int m_iCoarseTiledBufferCountersSRV;
	int m_iTiledIndexBufferUAV;

	// Additional resources for Tiled Rendering
	int m_iTiledRenderingcomputeShader;
	int m_iTiledIndexBufferSRV;
	int m_iParticleSRVArray;
	int m_iParticleTextureBufferUAV;
	int m_iParticleTextureBufferSRV;

	// Additional resources for cleaning up particles
	int m_iDestroyParticlesComputeShader;

	void OnCameraMove(void *pData);
	void OnResolutionChange(void *pData);

	void CullParticles();

public:
	ParticleManager();
	~ParticleManager();

	// IN: void
	// OUT: bool - success of initializing
	//
	// Initializes the manager
	bool Initialize();
	// IN: void
	// OUT: void
	//
	// Handles any post Initialize operations
	void PostInit();
	// IN: void
	// OUT: bool - success of destroying
	//
	// Destorys the manager
	bool Destory();

	// IN: void
	// OUT: void
	//
	// Goes through each emitter and emits any built up particles
	void Emit();

	// IN: void
	// OUT: void
	//
	// Simulates all the particle movements
	void Simulate();

	// IN: void
	// OUT: void
	//
	// Renders all particles
	void Render();

	// IN: void
	// OUT: void
	//
	// Destroys all particles that are alive
	void CleanupParticles();
};

