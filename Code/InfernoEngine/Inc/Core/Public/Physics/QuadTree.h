#pragma once

#include "Collider.h"
#include "Collision.h"
#include <vector>
#include "Color.h"

#define MAX_LEVEL 5
#define MAX_OBJECTS 5
#define NODE_COUNT 4
#define ROOT_LEVEL 0

class QuadTree{

public:
	struct NodeData{
		int level;
		Box2D bounds;

		NodeData(int _level, Box2D _bounds){
			level = _level;
			bounds = _bounds;
		}
	};

private:
	int m_iLevel;
	Box2D m_bounds;
	QuadTree* m_nodes[NODE_COUNT];
	std::vector<Collider*> m_staticColliders;
	std::vector<Collider*> m_dynColliders;

	void split();

public:
	QuadTree(int level, const Box2D& bounds);
	~QuadTree();

	void Update(QuadTree* root);

	void clear();
	bool insert(Collider* collider);
	void RetrieveStaticColliders(std::vector<Collider*>& returnColliders, const Box2D& bounds);
	void RetrieveDynamicColliders(std::vector<Collider*>& returnColliders, const Box2D& bounds);
	void RetrieveStaticColliders(std::vector<Collider*>& returnColliders, Collider* collider);
	void RetrieveDynamicColliders(std::vector<Collider*>& returnColliders, Collider* collider);
	bool remove(Collider* collider);
	void GetAllBounds(std::vector<NodeData>& returnData);
	void OnDrawGizmos(Color* color);
	bool ContainsCollider(Collider* collider);
};
