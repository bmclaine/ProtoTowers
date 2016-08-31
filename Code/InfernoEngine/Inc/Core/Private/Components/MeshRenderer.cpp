#include "Components\MeshRenderer.h"
#include "..\..\Interface\Public\ModuleInterface.h"
#include "Components\Transform.h"
#include "Assets\Mesh.h"
#include "Assets\Material.h"
#include "..\..\Interface\Public\Engine.h"
#include "Physics\Collision.h"

MeshRenderer::MeshRenderer(GameObject* const gameObject, Transform* const transform, bool skinned) : Renderer(gameObject, transform)
{
	m_pMesh = nullptr;
	
	// Call the defualts function to set defaults based on type
	SetDefaults();

	// Set defaults that are constant throughout this type of renderer
	m_iVertexBuffer = InfernoEngine_Graphics::GetBuffer(VERTEX_BUFFER, L"VertexBuffer");
	m_uiStride = sizeof(Vertex);
	m_iIndexBuffer = InfernoEngine_Graphics::GetBuffer(INDEX_BUFFER, L"IndexBuffer");
	m_iDepthStencilState = InfernoEngine_Graphics::GetDepthStencilState(L"ObjectStencilState");
	m_cColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
}
MeshRenderer::~MeshRenderer()
{
	InfernoEngine_Graphics::UnregisterRenderComponent(this);

	size_t numMats = m_vMaterials.size();
	for (size_t mat = 0; mat < numMats; mat++)
	{
		delete m_vMaterials[mat];
	}
	m_vMaterials.clear();
}

IComponent& MeshRenderer::operator=(IComponent& _rhs)
{
	MeshRenderer* rhsMeshRend = dynamic_cast<MeshRenderer*>(&_rhs);
	if (rhsMeshRend == nullptr) {
		return *this;
	}

	return operator=(*rhsMeshRend);
}

MeshRenderer& MeshRenderer::operator=(const MeshRenderer& _rhs)
{
	// === Copy over the Members
	m_pMesh = _rhs.m_pMesh;
	m_fcFlags = _rhs.m_fcFlags;

	// === Clean up any Materials
	for (size_t i = 0; i < m_vMaterials.size(); ++i) {
		delete m_vMaterials[i];
	}
	m_vMaterials.clear();

	m_vMaterials.resize(_rhs.m_vMaterials.size());
	for (size_t i = 0; i < _rhs.m_vMaterials.size(); i++)
	{
		m_vMaterials[i] = new Material();
		m_vMaterials[i]->DiffuseMapID = _rhs.m_vMaterials[i]->DiffuseMapID;
		m_vMaterials[i]->NormalMapID = _rhs.m_vMaterials[i]->NormalMapID;
		m_vMaterials[i]->OcclusionMapID = _rhs.m_vMaterials[i]->OcclusionMapID;
		m_vMaterials[i]->EffectID = _rhs.m_vMaterials[i]->EffectID;
		//*m_vMaterials[i] = *_rhs.m_vMaterials[i];
	}

	// === Copy over the Renderer Members
	m_iDepthStencilState = _rhs.m_iDepthStencilState;
	m_iGeometryShader = _rhs.m_iGeometryShader;
	m_iIndexBuffer = _rhs.m_iIndexBuffer;
	m_iInputLayout = _rhs.m_iInputLayout;
	m_iVertexBuffer = _rhs.m_iVertexBuffer;
	m_iVertexShader = _rhs.m_iVertexShader;
	m_cColor = _rhs.m_cColor;
	m_ttTopology = _rhs.m_ttTopology;
	m_uiStride = _rhs.m_uiStride;

	return *this;
}

IComponent* MeshRenderer::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	MeshRenderer* newRend = new MeshRenderer(gameObject, transform);
//	newRend->operator=(*this);
	(*newRend) = *this;

	return newRend;
}

// IN: void
// OUT: void
//
// Sets the defualt pipeline information this renderer will need
void MeshRenderer::SetDefaults()
{
	m_iInputLayout = InfernoEngine_Graphics::GetInputLayout(L"StaticMeshLayout");
	m_ttTopology = TopologyType::TRIANGLE_TOPOLOGY;
	m_iVertexShader = InfernoEngine_Graphics::GetShader(VERTEX_SHADER, L"StaticMeshVertexShader");
	m_iGeometryShader = -1;	// Using -1 for no shader
}

Mesh* MeshRenderer::GetMesh() const { return m_pMesh; }
RendererType MeshRenderer::GetType() const { return MESH_RENDERER; }

int MeshRenderer::GetNumVerts() { return 0; }
int MeshRenderer::GetStartVertex() { return 0; }

// IN: void
// OUT: void
//
// Sets up anything needed for drawling from the renderer side
void MeshRenderer::PreDraw()
{
	InfernoEngine_Graphics::SetWorldMatrix(transform->WorldMatrix());
	InfernoEngine_Graphics::SetColor(m_cColor);
}

// IN: void
// OUT: void
//
// Calls the draw function without changing the pipeline for doing the depth pre-pass
void MeshRenderer::DepthPrePassDraw()
{
#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(Inferno::ToWideString(gameObject->GetName()).c_str());
#endif

	for (size_t sMesh = 0; sMesh < m_pMesh->GetSubMeshes().size(); sMesh++)
	{
		SubMesh& subMesh = m_pMesh->GetSubMeshes()[sMesh];

		// First check to make sure we have a material to draw
		if (m_vMaterials.size() == 0)
			continue;

		Material* mat;

		if (sMesh < m_vMaterials.size())
			mat = m_vMaterials[sMesh];
		else
			mat = m_vMaterials[0];

		InfernoEngine_Graphics::SetImage(PIXEL_SHADER, mat->DiffuseMapID, 0);
		InfernoEngine_Graphics::SetUVOffsets(mat->GetOffsets(), mat->GetTiling());
		InfernoEngine_Graphics::ApplyPipelineState();
		InfernoEngine_Graphics::DrawIndexed(subMesh.indiceCount, subMesh.startIndex, subMesh.startVertex);
	}

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif
}

// IN: void
// OUT: void
//
// Calls the overloaded draw call for the current renderer
void MeshRenderer::Draw()
{
	for (size_t sMesh = 0; sMesh < m_pMesh->GetSubMeshes().size(); sMesh++)
	{
		SubMesh& subMesh = m_pMesh->GetSubMeshes()[sMesh];

		// First check to make sure we have a material to draw
		if (m_vMaterials.size() == 0)
			continue;

		Material* mat;

		if (sMesh < m_vMaterials.size())
			mat = m_vMaterials[sMesh];
		else
			mat = m_vMaterials[0];

		InfernoEngine_Graphics::SetImage(PIXEL_SHADER, mat->DiffuseMapID, 0);
		InfernoEngine_Graphics::SetUVOffsets(mat->GetOffsets(), mat->GetTiling());
		InfernoEngine_Graphics::DrawIndexed(mat->EffectID, subMesh.startIndex, subMesh.indiceCount, subMesh.startVertex);
	}
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void MeshRenderer::Init()
{
	
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
/*virtual*/ void MeshRenderer::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap)
{
	ObjectData* data = dataMap.find(GetInstanceID())->second;
	
	std::string propName = "Mesh";
	Property* prop = data->GetProperty(propName);

	m_pMesh = Inferno::GetMeshByName(prop->value.c_str());

	// === Clear out any old materials
	size_t count = m_vMaterials.size();
	for (size_t i = 0; i < count; ++i) {
		delete m_vMaterials[i];
	}
	m_vMaterials.clear();

	propName = "MaterialCount";
	prop = data->GetProperty(propName);
	int materialCount = atoi(prop->value.c_str());
	for (int i = 0; i < materialCount; ++i){
		propName = "Material" + std::to_string(i);
		prop = data->GetProperty(propName);
		if (prop == nullptr) continue;
		Material* mat = Inferno::GetMaterial(prop->value);
		if (mat)
		{
			Material* cMat = new Material();
			*cMat = *mat;
			m_vMaterials.push_back(cMat);
		}
	}

	propName = "TilingCount";
	prop = data->GetProperty(propName);
	int tilingCount = min(prop ? atoi(prop->value.c_str()) : 0, materialCount);
	for (int i = 0; i < tilingCount; ++i){
		propName = "Tiling" + std::to_string(i);
		prop = data->GetProperty(propName);
		if (prop == nullptr) continue;
		std::vector<std::string> tilingInfo = Inferno::StringParse(prop->value, ',');
		if (tilingInfo.size() < 2) continue;
		m_vMaterials[i]->SetTiling(Vector2((float)atof(tilingInfo[0].c_str()), (float)atof(tilingInfo[1].c_str())));
	}

	propName = "Color";
	prop = data->GetProperty(propName);
	std::vector<std::string> values = Inferno::StringParse(prop->value, ',');
	m_cColor = Color((float)atof(values[0].c_str()), (float)atof(values[1].c_str()), (float)atof(values[2].c_str()),(float)atof(values[3].c_str()));
}
// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void MeshRenderer::OnEnable()
{
	IComponent::OnEnable();
	InfernoEngine_Graphics::RegisterRenderComponent(this);
}
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void MeshRenderer::OnDisable()
{
	IComponent::OnDisable();
	InfernoEngine_Graphics::UnregisterRenderComponent(this);
}

// IN: void
// OUT: Material* - first material pointer
//
// Returns the first attached material
Material* MeshRenderer::GetMaterial() const
{
	if (m_vMaterials.size() > 0)
		return m_vMaterials[0];
	
	return nullptr;
}
// IN: void
// OUT: std::vector<Material*>& - refrence to the list of materials
//
// Returns the list of materials attached to the renderer
std::vector<Material*> const& MeshRenderer::GetMaterials() const { return m_vMaterials; }

// IN: void
// OUT: int - ID of the effect being used
//
// Returns the ID of the effect being used
int MeshRenderer::GetEffectID() const
{
	if (m_vMaterials.size() == 0)
		return -1;

	return m_vMaterials[0]->EffectID;
}

// IN: Mesh* - pointer to the mesh to render
// OUT: void
//
// Sets the mesh to use for rendering
void MeshRenderer::SetMesh(Mesh* mesh)
{
	m_pMesh = mesh;
}

Sphere MeshRenderer::GetBoundingSphere() const{
	Sphere BoundingSphere;

	if (m_pMesh)
	{
		BoundingSphere = m_pMesh->GetBoundingSphere();
		BoundingSphere.center = transform->WorldMatrix() * BoundingSphere.center;
		BoundingSphere.radius *= transform->GetScale().x;
	}
	else
		BoundingSphere = Sphere(transform->GetPosition(), 0.5f);

	return BoundingSphere;
	//return m_pMesh != nullptr ? m_pMesh->GetBoundingSphere() : Sphere(transform->GetPosition(), 0.5f);
}
