#pragma once
#include <vector>
#include "Components\Renderer.h"
#include "Physics\Collision.h"

#define MAX_LEVEL 5
#define MAX_RENDERERS 10
#define NODE_COUNT 4
#define ROOT_LEVEL 0

class MeshRenderer;
class SkinnedMeshRenderer;
class SpriteRenderer;

class RenderTree{

private:
	int m_iLevel;
	AABB m_bounds;
	RenderTree* m_nodes[NODE_COUNT];
	std::vector<Renderer*> m_staticRenderers;
	std::vector<Renderer*> m_dynRenderers;

	void split();

public:
	RenderTree(int level, const AABB& bounds);
	~RenderTree();

	void Update(RenderTree* root);

	void clear();
	bool insert(Renderer* renderer);
	void RetrieveVisibleObjects(std::vector<Renderer*>& opagueRenderers, std::vector<SkinnedMeshRenderer*>& skinnedMeshes, std::vector<Renderer*>& alphaRenderers, const Frustum& frustum);
	bool remove(Renderer* renderer);
	bool ContainsRenderer(Renderer* renderer);
};