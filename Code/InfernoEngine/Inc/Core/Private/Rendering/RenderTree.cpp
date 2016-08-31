#include "Rendering\RenderTree.h"
#include "Physics\CollisionLibrary.h"
#include "Components\MeshRenderer.h"
#include "Components\SkinnedMeshRenderer.h"
#include "Components\SpriteRenderer.h"

RenderTree::RenderTree(int level, const AABB& bounds) : m_iLevel(level), m_bounds(bounds){
	for (int i = 0; i < NODE_COUNT; ++i)
		m_nodes[i] = nullptr;
}

RenderTree::~RenderTree(){
	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		delete m_nodes[i];
	}
}

void RenderTree::Update(RenderTree* root){

	for (unsigned int i = 0; i < m_dynRenderers.size(); ++i){
		Renderer* renderer = m_dynRenderers[i];
		if ((!CollisionLibrary::AABBContainsSphere(m_bounds, renderer->GetBoundingSphere()) && m_iLevel != ROOT_LEVEL)){
			m_dynRenderers.erase(m_dynRenderers.begin() + i);
			root->insert(renderer);
		}
	}

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->Update(root);
	}

}

void RenderTree::clear(){
	m_staticRenderers.clear();
	m_dynRenderers.clear();

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->clear();
		delete m_nodes[i];
	}
}

void RenderTree::split(){
	Vector3 halfSize = m_bounds.size * 0.5f;
	Vector3 position = m_bounds.center;

	halfSize.y = m_bounds.size.y;

	m_nodes[0] = new RenderTree(m_iLevel + 1, AABB(Vector3(position.x + halfSize.x * 0.5f, position.y, position.z + halfSize.z * 0.5f), halfSize));
	m_nodes[1] = new RenderTree(m_iLevel + 1, AABB(Vector3(position.x - halfSize.x * 0.5f, position.y, position.z + halfSize.z * 0.5f), halfSize));
	m_nodes[2] = new RenderTree(m_iLevel + 1, AABB(Vector3(position.x + halfSize.x * 0.5f, position.y, position.z - halfSize.z * 0.5f), halfSize));
	m_nodes[3] = new RenderTree(m_iLevel + 1, AABB(Vector3(position.x - halfSize.x * 0.5f, position.y, position.z - halfSize.z * 0.5f), halfSize));
}

bool RenderTree::insert(Renderer* renderer){

	if ((!CollisionLibrary::AABBContainsSphere(m_bounds, renderer->GetBoundingSphere()) && m_iLevel != ROOT_LEVEL) || ContainsRenderer(renderer))
		return false;

	if (m_staticRenderers.size() + m_dynRenderers.size() < MAX_RENDERERS){
		(renderer->IsStatic() ? m_staticRenderers.push_back(renderer) : m_dynRenderers.push_back(renderer));
		return true;
	}

	if (m_nodes[0] == nullptr && m_iLevel < MAX_LEVEL)
		split();

	if (m_nodes[0] != nullptr){
		for (unsigned int i = 0; i < NODE_COUNT; ++i){
			if (m_nodes[i]->insert(renderer))
				return true;
		}
	}

	(renderer->IsStatic() ? m_staticRenderers.push_back(renderer) : m_dynRenderers.push_back(renderer));

	return true;
}

void RenderTree::RetrieveVisibleObjects(std::vector<Renderer*>& opagueRenderers, std::vector<SkinnedMeshRenderer*>& skinnedMeshes, std::vector<Renderer*>& alphaRenderers, const Frustum& frustum){

	if (!CollisionLibrary::FrustumToAABB(frustum, m_bounds)) return;

	Renderer* renderer = nullptr;
	for (unsigned int i = 0; i < (unsigned int)m_staticRenderers.size(); ++i){
		renderer = m_staticRenderers[i];
		if (CollisionLibrary::FrustumToSphere(frustum, m_staticRenderers[i]->GetBoundingSphere())){
			/*if (renderer->GetType() == RendererType::MESH_RENDERER)
				meshRenderers.push_back(static_cast<MeshRenderer*>(renderer));
			else if (renderer->GetType() == RendererType::SKINNED_MESH_RENDERER)
				skinnedMeshes.push_back(static_cast<SkinnedMeshRenderer*>(renderer));
			else if (renderer->GetType() == RendererType::SPRITE_RENDERER)
				spriteRenderers.push_back(static_cast<SpriteRenderer*>(renderer));*/

			if (renderer->GetType() == RendererType::SKINNED_MESH_RENDERER)
				skinnedMeshes.push_back(static_cast<SkinnedMeshRenderer*>(renderer));

			if (renderer->IsBlendingRenderer())
				alphaRenderers.push_back(renderer);
			else
				opagueRenderers.push_back(renderer);
		}
	}

	for (unsigned int i = 0; i < (unsigned int)m_dynRenderers.size(); ++i){
		renderer = m_dynRenderers[i];
		if (CollisionLibrary::FrustumToSphere(frustum, m_dynRenderers[i]->GetBoundingSphere())){
			/*if (renderer->GetType() == RendererType::MESH_RENDERER)
				meshRenderers.push_back(static_cast<MeshRenderer*>(renderer));
			else if (renderer->GetType() == RendererType::SKINNED_MESH_RENDERER)
				skinnedMeshes.push_back(static_cast<SkinnedMeshRenderer*>(renderer));
			else if (renderer->GetType() == RendererType::SPRITE_RENDERER)
				spriteRenderers.push_back(static_cast<SpriteRenderer*>(renderer));*/

			if (renderer->GetType() == RendererType::SKINNED_MESH_RENDERER)
				skinnedMeshes.push_back(static_cast<SkinnedMeshRenderer*>(renderer));

			if (renderer->IsBlendingRenderer())
				alphaRenderers.push_back(renderer);
			else
				opagueRenderers.push_back(renderer);
		}
	}

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->RetrieveVisibleObjects(opagueRenderers, skinnedMeshes, alphaRenderers, frustum);
	}
}

bool RenderTree::remove(Renderer* renderer){
	std::vector<Renderer*>& renderers = renderer->IsStatic() ? m_staticRenderers : m_dynRenderers;
	for (unsigned int i = 0; i < renderers.size(); ++i){
		if (renderers[i] == renderer){
			renderers.erase(renderers.begin() + i);
			return true;
		}
	}

	if (m_nodes[0] == nullptr)
		return false;

	bool removeValue = false;
	for (int i = 0; i < NODE_COUNT; ++i){
		if (m_nodes[i]->remove(renderer))
			removeValue = true;
	}

	return removeValue;
}

bool RenderTree::ContainsRenderer(Renderer* renderer){
	std::vector<Renderer*>& renderers = renderer->IsStatic() ? m_staticRenderers : m_dynRenderers;
	for (unsigned int i = 0; i < renderers.size(); ++i){
		if (renderers[i] == renderer) return true;
	}

	return false;
}



