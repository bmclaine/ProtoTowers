// Author: Jonathan Gyurkovics
//
// Handles the data needed to renderer a skinned mesh
#pragma once

#include "MeshRenderer.h"
#include "Assets\Bone.h"

class SkinnedMeshRenderer : public MeshRenderer
{
private:
	std::vector<Bone> m_vBones;
	std::vector<Matrix4> m_vBoneOffsets;
	//std::vector<Matrix4> m_vScaledBones;
	std::vector<unsigned int> m_vSkinnedVertOffsets;

	int m_iSkinnedVertexShader;
	int m_iSkinnedGeometryShader;
	int m_iSkinnedInputLayout;
	int m_iStreamOutEffect;
	int m_iBoneConstantBufferID;

	// IN: void
	// OUT: void
	//
	// Sets up anything needed for creating skinned verts
	void PreSkinnedDraw();

	// IN: void
	// OUT: void
	//
	// Calls draw to do the skinning and store to the Skinnded Vertex Buffer
	void DrawSkinned();

	// IN: void
	// OUT: void
	//
	// Calls the draw function without changing the pipeline for doing the depth pre-pass
	virtual void DepthPrePassDraw() override;

	//// IN: void
	//// OUT: void
	////
	//// Sets up anything needed for drawing from the renderer side
	//virtual void PreDraw() override;

	// IN: void
	// OUT: void
	//
	// Calls the overloaded draw call for the current renderer
	virtual void Draw() override;

	// IN: void
	// OUT: void
	//
	// Sets the defualt pipeline information this renderer will need
	virtual void SetDefaults() override;

	virtual ~SkinnedMeshRenderer();

	virtual IComponent& operator=(IComponent& _rhs) override;
	SkinnedMeshRenderer& operator=(const SkinnedMeshRenderer& _rhs);
	virtual IComponent* CreateCopy(GameObject* const gameObject, Transform* const transform) override;

public:
	SkinnedMeshRenderer(GameObject* const gameObject, Transform* const transform);

	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes any private, non object variables
	virtual void Init() override;
	// IN: void
	// OUT: map<unsigned int,Object*> - map of all the gameobjects and components in the scene
	//
	// Initializes object variables
	virtual void PostInit(std::map<unsigned int, Object*>& objectMap, std::map<unsigned int, ObjectData*>& dataMap);
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes enabled, whether its from just the component being Enabled, or the GameObject it's attached to.
	virtual void OnEnable() final;
	// IN: void
	// OUT: void
	//
	// Called when the Component becaomes disabled, whether its from just the component being Disabled, or the GameObject it's attached to.
	virtual void OnDisable() final;

	// IN: Bone*        - array of bones to use for mesh
	//     unsigned int - number of bones in array
	// OUT: void
	//
	// Creates a copy of the bones passed in
	void ApplySkeleton(Bone* bones, unsigned int boneCount);

	// IN: Mesh* - pointer to the mesh to render
	// OUT: void
	//
	// Sets the mesh to use for rendering
	virtual void SetMesh(Mesh* mesh) override;
	
	// IN: void
	// OUT: void
	//
	// Draws wire spheres at Skinned Mesh Bone locations
	virtual void OnDrawGizmos() override;

	// IN: void
	// OUT: size_t - number of bones this renderer has
	//
	// Returns the number of bones the renderer currently has
	size_t GetNumBones() const;

	virtual RendererType GetType() const override;

	// IN: _name - the name of the bone desired
	// OUT: _index - the index to the desired bone
	//
	// Returns a matrix to the requested bone in local space
	Matrix4 GetBoneLocal(std::string _name);
	Matrix4 GetBoneLocal(unsigned int _index);

	// IN: _name - the name of the bone desired
	// OUT: _index - the index to the desired bone
	//
	// Returns a matrix to the requested bone in calculated world space
	Matrix4 GetBoneWorld(std::string _name);
	Matrix4 GetBoneWorld(unsigned int _index);

	virtual Sphere GetBoundingSphere() const;

	friend class GraphicsManager;
	friend class Animator;
	friend class AnimationController;
	friend SkinnedMeshRenderer* GameObject::AddComponent();
};