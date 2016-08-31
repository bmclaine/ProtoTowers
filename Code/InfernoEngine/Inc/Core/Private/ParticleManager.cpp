#include "ParticleManager.h"
#include "CameraManager.h"
#include "EventSystem.h"
#include "InfernoTime.h"
#include "FileSystem.h"

#include "../../Interface/Public/ModuleInterface.h"

#include "GameObject.h"

#include "Components\Transform.h"
#include "Components\ParticleSystem.h"
#include "Components\Camera.h"

ParticleManager::ParticleManager() : Registry<ParticleSystem>(20)
{
	m_TilingConstantBuffer.NumTilesX = m_TilingConstantBuffer.NumTilesY = 0;
	m_TilingConstantBuffer.NumCoarseCullingTilesX = MAX_COARSE_TILES_X;
	m_TilingConstantBuffer.NumCoarseCullingTilesY = MAX_COARSE_TILES_Y;

	EventSystem::GetInstance()->RegisterForEvent("ResolutionChange", this, BindEventFunction(&ParticleManager::OnResolutionChange, this));
	EventSystem::GetInstance()->RegisterForEvent("MAIN_CAMERA_VIEW_UPDATE", this, BindEventFunction(&ParticleManager::OnCameraMove, this));
}

ParticleManager::~ParticleManager()
{
	EventSystem::GetInstance()->UnregisterFromEvent("ResolutionChange", this, BindEventFunction(&ParticleManager::OnResolutionChange, this));
	EventSystem::GetInstance()->UnregisterFromEvent("MAIN_CAMERA_VIEW_UPDATE", this, BindEventFunction(&ParticleManager::OnCameraMove, this));
}

// IN: void
// OUT: bool - success of initializing
//
// Initializes the manager
bool ParticleManager::Initialize()
{
	m_vComputeShaders.resize(TOTAL_SHAPES);
	
	// Sphere
	m_vComputeShaders[EMITTER_SHAPE_SPHERE].push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"SphereParticleEmitterComputeShader"));
	m_vComputeShaders[EMITTER_SHAPE_SPHERE].push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"SphereEdgeParticleEmitterComputeShader"));
	
	// Hemisphere
	m_vComputeShaders[EMITTER_SHAPE_HEMISPHERE].push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"HemisphereParticleEmitterComputeShader"));
	m_vComputeShaders[EMITTER_SHAPE_HEMISPHERE].push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"HemisphereEdgeParticleEmitterComputeShader"));
	
	// Circle
	m_vComputeShaders[EMITTER_SHAPE_CIRCLE].push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"CircleParticleEmitterComputeShader"));
	m_vComputeShaders[EMITTER_SHAPE_CIRCLE].push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"CircleEdgeParticleEmitterComputeShader"));

	// Cone
	m_vComputeShaders[EMITTER_SHAPE_CONE].push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"ConeParticleEmitterComputeShader"));
	m_vComputeShaders[EMITTER_SHAPE_CONE].push_back(InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"ConeOutterParticleEmitterComputeShader"));

	m_iParticleSRVArray = InfernoEngine_Graphics::LoadTexture(L"Particles.dds");

	// Grab all needed resource IDs for Emitting
	m_iPartAUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"ParticlePartABuffer");
	m_iPartBUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"ParticlePartBBuffer");
	m_iDeadListUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"ParticleDeadList");
	m_iRandTexture = InfernoEngine_Graphics::GetImage(L"RandomValueTexture");
	m_iSamplerID = InfernoEngine_Graphics::GetSamplerState(L"WrapSamplerState");
	m_iEmitterSettingsBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"EmitterSettings");
	m_iEmitterRotationBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"EmitterRotation");
	m_iDeadCountBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"DeadListCount");

	// Grab additional needed resource IDs for Simulation
	m_iParticleSimulateComputeShader = InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"ParticleSimulationComputeShader");
	m_iParticleIndexUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"ParticleActiveIndexList");
	m_iParticleViewSpaceUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"ParticleViewSpacePositions");
	m_iParticleDrawArgsUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"ParticleDrawArgs");
	m_iFrameInformationBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"FrameInformation");
	m_iAliveBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"ActiveList");

	// Grab additional needed resource IDs for Rendering
	/*m_iVertexShader = InfernoEngine_Graphics::GetShader(VERTEX_SHADER, L"ParticleVertexShader");
	m_iGeometryShader = InfernoEngine_Graphics::GetShader(GEOMETRY_SHADER, L"ParticleGeometryShader");
	m_iPixelShader = InfernoEngine_Graphics::GetShader(PIXEL_SHADER, L"ParticlePixelShader");*/
	m_iVertexShader = InfernoEngine_Graphics::GetShader(VERTEX_SHADER, L"UIVertexShader");
	m_iGeometryShader = InfernoEngine_Graphics::GetShader(GEOMETRY_SHADER, L"ScreenQuadGeometryShader");
	m_iPixelShader = InfernoEngine_Graphics::GetShader(PIXEL_SHADER, L"TiledParticlePixelShader");
	m_iParticleDrawArgsBuffer = InfernoEngine_Graphics::GetBuffer(STRUCTURED_BUFFER, L"ParticleDrawArgs");
	m_iPartASRV = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"ParticlePartABuffer");
	m_iViewSpaceSRV = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"ParticleViewSpacePositions");
	m_iParticleIndexSRV = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"ParticleActiveIndexList");
	m_iBlendState = InfernoEngine_Graphics::GetBlendState(L"AlphaBlendingBlendState");
	m_iRasterizerState = InfernoEngine_Graphics::GetRasterizerState(L"FillRasterizerState");

	// Additional resources for Coarse Culling
	m_iCoarseCullingComputeShader = InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"CoarseCullingComputeShader");
	m_iCoarseTiledIndexBufferUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"CoarseTiledIndexBuffer");
	m_iCoarseTiledBufferCountersUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"CoarseTiledBufferCounters");
	m_iTilingConstantBuffer = InfernoEngine_Graphics::GetConstantBuffer(L"TilingConstantBuffer");

	// Additional resources for Fine Grained Culling
	m_iCullingComputeShader = InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"ParticleCullingComputeShader");
	m_iCoarseTiledIndexBufferSRV = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"CoarseTiledIndexBuffer");
	m_iCoarseTiledBufferCountersSRV = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"CoarseTiledBufferCounters");
	m_iTiledIndexBufferUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"TiledIndexBuffer");

	// Additional resources for Tiled Rendering
	m_iTiledRenderingcomputeShader = InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"ParticleTileRenderingComputeShader");
	m_iTiledIndexBufferSRV = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"TiledIndexBuffer");
	m_iParticleTextureBufferUAV = InfernoEngine_Graphics::GetUnorderedAccessView(L"ParticleTextureBuffer");
	m_iParticleTextureBufferSRV = InfernoEngine_Graphics::GetShaderResource(SHADER_RESOURCE_STRUCTURED_BUFFER, L"ParticleTextureBuffer");

	// Additional resources for cleaning up particles
	m_iDestroyParticlesComputeShader = InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"DestroyAllParticlesComputeShader");

	// Set up and call the Init Dead Particle Compute Shader so we can have a list of particles crated for us
	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, -1);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);
	InfernoEngine_Graphics::SetShader(PIXEL_SHADER, -1);
	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, InfernoEngine_Graphics::GetShader(COMPUTE_SHADER, L"InitDeadParticlesComputeShader"));
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iDeadListUAV, 0);
	InfernoEngine_Graphics::Disptach(align(MAX_PARTICLES, 256) / 256, 1, 1);

	InfernoEngine_Graphics::CopyStructureCount(m_iDeadCountBuffer, 0, m_iDeadListUAV);

	// Clear the pipeline of what we just assigned
	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, -1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);

	return true;
}

// IN: void
// OUT: void
//
// Handles any post Initialize operations
void ParticleManager::PostInit()
{
	
}

// IN: void
// OUT: bool - success of destroying
//
// Destorys the manager
bool ParticleManager::Destory()
{
	return true;
}

// IN: void
// OUT: void
//
// Goes through each emitter and emits any built up particles
void ParticleManager::Emit()
{
	if (m_vRegisteredObjects.size() == 0)
		return;

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(L"Particle Emit");
#endif

	InfernoEngine_Graphics::SetInputLayout(-1);
	InfernoEngine_Graphics::SetVertexBuffer(-1, 0, 0);
	InfernoEngine_Graphics::SetIndexBuffer(-1);
	InfernoEngine_Graphics::SetDepthStencilState(-1, 0);
	InfernoEngine_Graphics::SetBlendState(-1);
	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, -1);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);
	InfernoEngine_Graphics::SetShader(PIXEL_SHADER, -1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iPartAUAV, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iPartBUAV, 1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iDeadListUAV, 2);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iRandTexture, 0);
	InfernoEngine_Graphics::SetSamplerStates(COMPUTE_SHADER, m_iSamplerID, 0);

	for (size_t system = 0; system < m_vRegisteredObjects.size(); system++)
	{
		ParticleSystem* currSystem = m_vRegisteredObjects[system];

		if (!currSystem->gameObject->ActiveInHierarchy() || currSystem->IsPaused() || currSystem->IsFinished() || currSystem->m_Settings.Emission == 0)
			continue;

#if defined(DEBUG) || defined(_DEBUG)
		InfernoEngine_Graphics::StartEvent(Inferno::ToWideString(currSystem->gameObject->GetName()).c_str());
#endif

		Matrix4 world = currSystem->transform->WorldMatrix();

		InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, m_vComputeShaders[currSystem->m_eShape][currSystem->m_eStyle]);
		InfernoEngine_Graphics::UpdateSubResource(m_iEmitterSettingsBuffer, &currSystem->m_Settings, sizeof(ParticleSystem::EmitterSettings));
		InfernoEngine_Graphics::UpdateSubResource(m_iEmitterRotationBuffer, &world, sizeof(Matrix4));

		InfernoEngine_Graphics::Disptach(align(currSystem->m_Settings.Emission, 256) / 256, 1, 1);

		InfernoEngine_Graphics::CopyStructureCount(m_iDeadCountBuffer, 0, m_iDeadListUAV);

#if defined(DEBUG) || defined(_DEBUG)
		InfernoEngine_Graphics::EndEvent();
#endif
	}

	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, -1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 2);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetSamplerStates(COMPUTE_SHADER, -1, 0);

	InfernoEngine_Graphics::ApplyPipelineState();

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif
}

// IN: void
// OUT: void
//
// Simulates all the particle movements
void ParticleManager::Simulate()
{
#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(L"Particle Simulate");
#endif

	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, -1);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);
	InfernoEngine_Graphics::SetShader(PIXEL_SHADER, -1);
	InfernoEngine_Graphics::SetDepthStencilView(-1);
	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, m_iParticleSimulateComputeShader);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iPartAUAV, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iPartBUAV, 1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iDeadListUAV, 2);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iParticleIndexUAV, 3, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iParticleViewSpaceUAV, 4);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iParticleDrawArgsUAV, 5);

	m_FrameInfo.DeltaTime = Time::DeltaTime;
	InfernoEngine_Graphics::UpdateSubResource(m_iFrameInformationBuffer, &m_FrameInfo, sizeof(FrameInformation));

	InfernoEngine_Graphics::Disptach(align(MAX_PARTICLES, 256) / 256, 1, 1);

	InfernoEngine_Graphics::CopyStructureCount(m_iAliveBuffer, 0, m_iParticleIndexUAV);

	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, -1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 2);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 3);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 4);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 5);
	InfernoEngine_Graphics::ApplyPipelineState();

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif

	CullParticles();
}

void ParticleManager::CullParticles()
{
#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(L"Particle Culling");
	InfernoEngine_Graphics::StartEvent(L"Coarse Culling");
#endif

	// Start by doing the Coarse Culling
	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, m_iCoarseCullingComputeShader);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iViewSpaceSRV, 0);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iParticleIndexSRV, 1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iCoarseTiledIndexBufferUAV, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iCoarseTiledBufferCountersUAV, 1);

	InfernoEngine_Graphics::Disptach(align(MAX_PARTICLES, 256) / 256, 1, 1);

	// Next do fine grained Culling
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 1);
	InfernoEngine_Graphics::ApplyPipelineState();

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent(); // Coarse Culling

	InfernoEngine_Graphics::StartEvent(L"Fine-Grained Culling");
#endif

	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, m_iCullingComputeShader);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iCoarseTiledIndexBufferSRV, 1);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iCoarseTiledBufferCountersSRV, 2);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, 0, 3);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iTiledIndexBufferUAV, 0);

	InfernoEngine_Graphics::Disptach(m_TilingConstantBuffer.NumTilesX, m_TilingConstantBuffer.NumTilesY, 1);

	// Next do the Tiled Rendering
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::ApplyPipelineState();

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent(); // Fine-Grained Culling

	InfernoEngine_Graphics::StartEvent(L"Tile Rendering");
#endif

	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, m_iTiledRenderingcomputeShader);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iPartASRV, 0);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iViewSpaceSRV, 1);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iTiledIndexBufferSRV, 2);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, 0, 3);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, m_iParticleSRVArray, 4);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iParticleTextureBufferUAV, 0);
	InfernoEngine_Graphics::SetSamplerStates(COMPUTE_SHADER, m_iSamplerID, 0);

	InfernoEngine_Graphics::Disptach(m_TilingConstantBuffer.NumTilesX, m_TilingConstantBuffer.NumTilesY, 1);

	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, -1);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 1);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 2);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 3);
	InfernoEngine_Graphics::SetImage(COMPUTE_SHADER, -1, 4);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetSamplerStates(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::ApplyPipelineState();

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent(); // Tile Rendering
	InfernoEngine_Graphics::EndEvent();	// Particle Culling
#endif
}

// IN: void
// OUT: void
//
// Renders all particles
void ParticleManager::Render()
{
#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(L"Particle Rendering");
#endif

	//InfernoEngine_Graphics::SetShader(VERTEX_SHADER, m_iVertexShader);
	//InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, m_iGeometryShader);
	//InfernoEngine_Graphics::SetShader(PIXEL_SHADER, m_iPixelShader);
	//InfernoEngine_Graphics::SetTopology(POINT_TOPOLOGY);
	//InfernoEngine_Graphics::SetImage(VERTEX_SHADER, m_iPartASRV, 0);
	//InfernoEngine_Graphics::SetImage(VERTEX_SHADER, m_iViewSpaceSRV, 1);
	//InfernoEngine_Graphics::SetImage(VERTEX_SHADER, m_iParticleIndexSRV, 2);
	//InfernoEngine_Graphics::SetImage(PIXEL_SHADER, -1, 0);
	//InfernoEngine_Graphics::SetSamplerStates(PIXEL_SHADER, m_iSamplerID, 0);
	InfernoEngine_Graphics::SetBlendState(m_iBlendState);
	InfernoEngine_Graphics::SetRasterizerState(m_iRasterizerState);

	//for (size_t image = 1; image <= m_vImageIds.size(); image++)
	//	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_vImageIds[image - 1], (int)image);

	//InfernoEngine_Graphics::ApplyPipelineState();

	//// Call Draw Indirect
	//InfernoEngine_Graphics::DrawIndirect(m_iParticleDrawArgsBuffer, 0);

	//// Clear the pipeline
	//InfernoEngine_Graphics::SetShader(VERTEX_SHADER, -1);
	//InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);
	//InfernoEngine_Graphics::SetShader(PIXEL_SHADER, -1);
	//InfernoEngine_Graphics::SetImage(VERTEX_SHADER, -1, 0);
	//InfernoEngine_Graphics::SetImage(VERTEX_SHADER, -1, 1);
	//InfernoEngine_Graphics::SetImage(VERTEX_SHADER, -1, 2);
	//InfernoEngine_Graphics::SetImage(PIXEL_SHADER, -1, 0);
	//InfernoEngine_Graphics::SetSamplerStates(PIXEL_SHADER, -1, 0);

	//for (size_t image = 1; image <= m_vImageIds.size(); image++)
	//	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, -1, (int)image);

	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, m_iVertexShader);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, m_iGeometryShader);
	InfernoEngine_Graphics::SetShader(PIXEL_SHADER, m_iPixelShader);
	InfernoEngine_Graphics::SetTopology(POINT_TOPOLOGY);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, m_iParticleTextureBufferSRV, 0);

	InfernoEngine_Graphics::ApplyPipelineState();
	InfernoEngine_Graphics::Draw(1u, 0u);

	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, -1);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);
	InfernoEngine_Graphics::SetShader(PIXEL_SHADER, -1);
	InfernoEngine_Graphics::SetImage(PIXEL_SHADER, -1, 0);
	InfernoEngine_Graphics::SetDepthStencilView(0);

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif
}

// IN: void
// OUT: void
//
// Destroys all particles that are alive
void ParticleManager::CleanupParticles()
{
#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(L"Particle Cleanup");
#endif

	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, -1);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, -1);
	InfernoEngine_Graphics::SetShader(PIXEL_SHADER, -1);
	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, m_iDestroyParticlesComputeShader);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iPartAUAV, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, m_iDeadListUAV, 1);

	InfernoEngine_Graphics::Disptach(align(MAX_PARTICLES, 256) / 256, 1, 1);

	InfernoEngine_Graphics::SetShader(COMPUTE_SHADER, -1);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 0);
	InfernoEngine_Graphics::SetUnorderedAccessView(COMPUTE_SHADER, -1, 1);

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif
}

void ParticleManager::OnCameraMove(void *pData)
{
	m_FrameInfo.CameraPostion = CameraManager::Main()->transform->GetPosition();
}
void ParticleManager::OnResolutionChange(void *pData)
{
	UINT width = (UINT)*(static_cast<float*>(pData));
	UINT height = (UINT)*(static_cast<float*>(pData)+1);

	m_FrameInfo.ScreenWidth = width;
	m_FrameInfo.ScreenHeight = height;

	m_TilingConstantBuffer.NumTilesX = align(width, CULLING_TILE_SIZE_X) / CULLING_TILE_SIZE_X;
	m_TilingConstantBuffer.NumTilesY = align(height, CULLING_TILE_SIZE_X) / CULLING_TILE_SIZE_X;
	m_TilingConstantBuffer.NumCullingTilesPerCoarseTileX = align(m_TilingConstantBuffer.NumTilesX, m_TilingConstantBuffer.NumCoarseCullingTilesX) / m_TilingConstantBuffer.NumCoarseCullingTilesX;
	m_TilingConstantBuffer.NumCullingTilesPerCoarseTileY = align(m_TilingConstantBuffer.NumTilesY, m_TilingConstantBuffer.NumCoarseCullingTilesY) / m_TilingConstantBuffer.NumCoarseCullingTilesY;
	
	InfernoEngine_Graphics::UpdateSubResource(m_iTilingConstantBuffer, &m_TilingConstantBuffer, sizeof(TilingConstantBuffer));
}