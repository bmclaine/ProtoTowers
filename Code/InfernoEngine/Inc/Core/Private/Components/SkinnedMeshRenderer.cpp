#include "Components\SkinnedMeshRenderer.h"
#include "..\..\Interface\Public\ModuleInterface.h"
#include "..\..\Interface\Public\Engine.h"
#include "Components\Transform.h"
#include "Assets\Mesh.h"
#include "Assets\Material.h"
#include "Gizmos.h"
#include "Physics\Collision.h"

SkinnedMeshRenderer::SkinnedMeshRenderer(GameObject* const gameObject, Transform* const transform) : MeshRenderer(gameObject, transform, true)
{
	SetDefaults();
}
SkinnedMeshRenderer::~SkinnedMeshRenderer()
{
	InfernoEngine_Graphics::UnregisterRenderComponent(this);

	if (m_pMesh)
	{
		if (m_vSkinnedVertOffsets.size())
			InfernoEngine_Graphics::ReleaseSkinningVerts(m_pMesh, m_vSkinnedVertOffsets[0]);
	}
}

IComponent& SkinnedMeshRenderer::operator=(IComponent& _rhs)
{
	SkinnedMeshRenderer* rhsSkinRend = dynamic_cast<SkinnedMeshRenderer*>(&_rhs);
	if (rhsSkinRend == nullptr) {
		return *this;
	}

	return operator=(*rhsSkinRend);
}

SkinnedMeshRenderer& SkinnedMeshRenderer::operator=(const SkinnedMeshRenderer&_rhs)
{
	// === Copy over the Members
	m_vBones = _rhs.m_vBones;
	m_vBoneOffsets = _rhs.m_vBoneOffsets;
	m_iSkinnedVertexShader = _rhs.m_iSkinnedVertexShader;
	m_iSkinnedInputLayout = _rhs.m_iSkinnedInputLayout;
	m_iStreamOutEffect = _rhs.m_iStreamOutEffect;
	m_iSkinnedGeometryShader = _rhs.m_iSkinnedGeometryShader;
	m_iBoneConstantBufferID = _rhs.m_iBoneConstantBufferID;

	Mesh* oldMesh = m_pMesh;

	// === Copy over the MeshRenderer Members
	MeshRenderer::operator=(_rhs);

	// If we have a valid mesh, request verts in the skinning vertex buffer
	if (m_pMesh && m_pMesh != oldMesh)
	{
		if (oldMesh)
		{
			if (m_vSkinnedVertOffsets.size())
				InfernoEngine_Graphics::ReleaseSkinningVerts(m_pMesh, m_vSkinnedVertOffsets[0]);
		}

		m_vSkinnedVertOffsets = InfernoEngine_Graphics::RequestSkinningVerts(m_pMesh);
	}

	return *this;
}

// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes any private, non object variables
void SkinnedMeshRenderer::Init()
{

}
// IN: void
// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
//
// Initializes object variables
void SkinnedMeshRenderer::PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap){
	ObjectData* data = dataMap.find(GetInstanceID())->second;

	std::string propName = "Mesh";
	Property* prop = data->GetProperty(propName);
	SetMesh(Inferno::GetMeshByName(prop->value.c_str()));

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

	m_cColor = Color((float)atof(values[0].c_str()), (float)atof(values[1].c_str()), (float)atof(values[2].c_str()), 1.0f);
	if (m_pMesh && m_pMesh->GetBones().size() > 0){
		ApplySkeleton(&m_pMesh->GetBones()[0], (UINT)m_pMesh->GetBones().size());
	}
}
// IN: void
// OUT: void
//
// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
void SkinnedMeshRenderer::OnEnable()
{
	InfernoEngine_Graphics::RegisterRenderComponent(this);
	IComponent::OnEnable();
}
// IN: void
// OUT: void
//
// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
void SkinnedMeshRenderer::OnDisable()
{
	InfernoEngine_Graphics::UnregisterRenderComponent(this);
	IComponent::OnDisable();
}

IComponent* SkinnedMeshRenderer::CreateCopy(GameObject* const gameObject, Transform* const transform)
{
	SkinnedMeshRenderer* newRend = new SkinnedMeshRenderer(gameObject, transform);
	(*newRend) = *this;

	return newRend;
}

// IN: void
// OUT: void
//
// Sets the defualt pipeline information this renderer will need
void SkinnedMeshRenderer::SetDefaults()
{
	m_iStreamOutEffect = Inferno::GetEffectByName("StreamOutEffect");
	m_iSkinnedGeometryShader = InfernoEngine_Graphics::GetShader(GEOMETRY_SHADER, L"StreamOutputSkinnedGeometryShader");
	m_iVertexBuffer = InfernoEngine_Graphics::GetBuffer(VERTEX_BUFFER, L"SkinnedVertexBuffer");
	m_uiStride = sizeof(Vertex);
	m_iSkinnedInputLayout = InfernoEngine_Graphics::GetInputLayout(L"SkinnedMeshLayout");
	m_iSkinnedVertexShader = InfernoEngine_Graphics::GetShader(VERTEX_SHADER, L"SkinnedMeshVertexShader");
	m_iBoneConstantBufferID = InfernoEngine_Graphics::GetConstantBuffer(L"Bones");

	//m_iInputLayout = InfernoEngine_Graphics::GetInputLayout(L"SkinnedMeshLayout");
	//m_ttTopology = TopologyType::TRIANGLE_TOPOLOGY;
	//m_iVertexShader = InfernoEngine_Graphics::GetShader(VERTEX_SHADER, L"SkinnedMeshVertexShader");
	//m_iGeometryShader = -1;	// Using -1 for no shader
}

RendererType SkinnedMeshRenderer::GetType() const
{
	return SKINNED_MESH_RENDERER;
}

// IN: void
// OUT: void
//
// Sets up anything needed for creating skinned verts
void SkinnedMeshRenderer::PreSkinnedDraw()
{
	InfernoEngine_Graphics::SetShader(VERTEX_SHADER, m_iSkinnedVertexShader);
	InfernoEngine_Graphics::SetShader(GEOMETRY_SHADER, m_iSkinnedGeometryShader);
	InfernoEngine_Graphics::SetInputLayout(m_iSkinnedInputLayout);

	if (m_vBoneOffsets.size() > 0)
		InfernoEngine_Graphics::UpdateSubResource(m_iBoneConstantBufferID, &m_vBoneOffsets[0], (UINT)m_vBoneOffsets.size() * sizeof(Matrix4));
}

// IN: void
// OUT: void
//
// Calls draw to do the skinning and store to the Skinnded Vertex Buffer
void SkinnedMeshRenderer::DrawSkinned()
{
#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::StartEvent(Inferno::ToWideString(gameObject->GetName()).c_str());
#endif

	// Just draw the verts as they will be drawn as points and then modified and saved off in the proper locations
	for (size_t sMesh = 0; sMesh < m_pMesh->GetSubMeshes().size(); sMesh++)
		InfernoEngine_Graphics::Draw(m_iStreamOutEffect, m_vSkinnedVertOffsets[sMesh], m_pMesh->GetSubMeshes()[sMesh].vertexCount);

#if defined(DEBUG) || defined(_DEBUG)
	InfernoEngine_Graphics::EndEvent();
#endif
}

// IN: void
// OUT: void
//
// Calls the draw function without changing the pipeline for doing the depth pre-pass
void SkinnedMeshRenderer::DepthPrePassDraw()
{
	for (size_t sMesh = 0; sMesh < m_pMesh->GetSubMeshes().size(); sMesh++)
	{
		SubMesh& subMesh = m_pMesh->GetSubMeshes()[sMesh];

		// First check to make sure we have a material to draw
		if (m_vMaterials.size() == 0 || sMesh >= m_vSkinnedVertOffsets.size())
			continue;

		Material* mat;

		if (sMesh < m_vMaterials.size())
			mat = m_vMaterials[sMesh];
		else
			mat = m_vMaterials[0];

		InfernoEngine_Graphics::SetImage(PIXEL_SHADER, mat->DiffuseMapID, 0);
		InfernoEngine_Graphics::SetUVOffsets(mat->GetOffsets(), mat->GetTiling());
		InfernoEngine_Graphics::ApplyPipelineState();

		if (sMesh < m_vSkinnedVertOffsets.size())
			InfernoEngine_Graphics::DrawIndexed(subMesh.indiceCount, subMesh.startIndex, m_vSkinnedVertOffsets[sMesh]);
	}
}

//// IN: void
//// OUT: void
////
//// Sets up anything needed for drawling from the renderer side
//void SkinnedMeshRenderer::PreDraw()
//{
//	InfernoEngine_Graphics::SetColor(m_cColor);
//
//	if (m_vBones.size() > 0)
//	{
//		Matrix4 world = transform->WorldMatrix();
//
//		for (size_t i = 0; i < m_vBones.size(); i++)
//			m_vScaledBones[i] = m_vBones[i].GetBoneOffset() * world;
//
//		InfernoEngine_Graphics::UpdateSubResource(m_iBoneConstantBufferID, &m_vScaledBones[0], (UINT)m_vBones.size() * sizeof(Matrix4));
//	}
//}

// IN: void
// OUT: void
//
// Calls the overloaded draw call for the current renderer
void SkinnedMeshRenderer::Draw()
{
	for (size_t sMesh = 0; sMesh < m_pMesh->GetSubMeshes().size(); sMesh++)
	{
		SubMesh& subMesh = m_pMesh->GetSubMeshes()[sMesh];

		// First check to make sure we have a material to draw
		if (m_vMaterials.size() == 0 || sMesh >= m_vSkinnedVertOffsets.size())
			continue;

		Material* mat;

		if (sMesh < m_vMaterials.size())
			mat = m_vMaterials[sMesh];
		else
			mat = m_vMaterials[0];

		InfernoEngine_Graphics::SetImage(PIXEL_SHADER, mat->DiffuseMapID, 0);
		InfernoEngine_Graphics::SetUVOffsets(mat->GetOffsets(), mat->GetTiling());
		InfernoEngine_Graphics::DrawIndexed(mat->EffectID, subMesh.startIndex, subMesh.indiceCount, m_vSkinnedVertOffsets[sMesh]);
	}
}

// IN: Bone*        - array of bones to use for mesh
//     unsigned int - number of bones in array
// OUT: void
//
// Creates a copy of the bones passed in
void SkinnedMeshRenderer::ApplySkeleton(Bone* bones, unsigned int boneCount)
{
	// Clear our vector incase we had a previous set of bones attached
	m_vBones.clear();

	// Check to see if we can support the number of bones coming in, and if not resize the vector
	if (m_vBones.capacity() < boneCount)
	{
		m_vBones.reserve(boneCount);
		m_vBoneOffsets.resize(boneCount);
		//m_vScaledBones.resize(boneCount);
	}

	// Make a copy of each bone
	for (unsigned int bone = 0; bone < boneCount; bone++)
	{
		m_vBones.push_back(bones[bone]);
		m_vBoneOffsets[bone] = bones[bone].GetBoneOffset();
	}
}

// IN: Mesh* - pointer to the mesh to render
// OUT: void
//
// Sets the mesh to use for rendering
void SkinnedMeshRenderer::SetMesh(Mesh* mesh)
{
	// If we had a mesh assigned to us, we need to make sure we didn't have requested skinning verts, and if so
	// they need to be released back to the manager
	if (m_pMesh)
	{
		if (m_vSkinnedVertOffsets.size())
			InfernoEngine_Graphics::ReleaseSkinningVerts(mesh, m_vSkinnedVertOffsets[0]);
	}

	m_pMesh = mesh;

	// If we have a valid mesh, request verts in the skinning vertex buffer
	if (mesh)
		m_vSkinnedVertOffsets = InfernoEngine_Graphics::RequestSkinningVerts(mesh);
}

// IN: void
// OUT: void
//
// Draws wired sphere at skinned mesh bones locations
void SkinnedMeshRenderer::OnDrawGizmos()
{
	for (size_t i = 0; i < m_vBoneOffsets.size(); i++)
	{
		Matrix4 mat = m_vBones[i].GetInverseGlobalBindPose().Inverse() * m_vBoneOffsets[i];
		mat *= transform->WorldMatrix();
		Vector3 pos = mat.Translation();

		Gizmos::DrawWireSphere(pos, 0.1f);
	}
}

// IN: void
// OUT: size_t - number of bones this renderer has
//
// Returns the number of bones the renderer currently has
size_t SkinnedMeshRenderer::GetNumBones() const { return m_vBones.size(); }

// IN: _name - the name of the bone desired
// OUT: _index - the index to the desired bone
//
// Returns a matrix to the requested bone in local space
Matrix4 SkinnedMeshRenderer::GetBoneLocal(std::string _name)
{
	Matrix4 retMat = Matrix4::Empty();

	unsigned int index = 0;
	auto iter = m_vBones.begin();
	while (iter != m_vBones.end())
	{
		if (iter->m_Name == _name)
			retMat = GetBoneLocal(index);

		++iter, ++index;
	}

	return retMat;
}

Matrix4 SkinnedMeshRenderer::GetBoneLocal(unsigned int _index)
{
	return m_vBoneOffsets[_index];
}

// IN: _name - the name of the bone desired
// OUT: _index - the index to the desired bone
//
// Returns a reference to the requested bone in world space
Matrix4 SkinnedMeshRenderer::GetBoneWorld(std::string _name)
{
	// If we don't find the requested bone, return empty
	Matrix4 retMat = Matrix4::Empty();

	unsigned int index = 0;
	auto iter = m_vBones.begin();
	while (iter != m_vBones.end())
	{
		if (iter->m_Name == _name)
		{
			retMat = GetBoneWorld(index);
			break;
		}

		++iter, ++index;
	}

	return retMat;
}

Matrix4 SkinnedMeshRenderer::GetBoneWorld(unsigned int _index)
{
	Matrix4 boneOffset = m_vBoneOffsets[_index];
	Matrix4 boneBindPose = m_vBones[_index].GetBindPose();

	Matrix4 boneWorld = boneBindPose * boneOffset;

	// temp code
	Matrix4 inverseBindPose = m_vBones[_index].GetInverseGlobalBindPose();
	Matrix4 temp = inverseBindPose * boneWorld;
	// end temp code

	Matrix4 worldMat = transform->LocalMatrix();

	Matrix4 retMat = boneWorld * worldMat;

	return retMat;
}

Sphere SkinnedMeshRenderer::GetBoundingSphere() const{
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
