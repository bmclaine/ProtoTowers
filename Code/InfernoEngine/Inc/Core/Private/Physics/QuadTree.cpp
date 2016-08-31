#include "../Public/Physics/QuadTree.h"
#include "../Public/Physics/CollisionLibrary.h"
#include "../Public/Physics/Physics.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "Gizmos.h"

QuadTree::QuadTree(int level, const Box2D& bounds) : m_iLevel(level), m_bounds(bounds){
	for (int i = 0; i < NODE_COUNT; ++i)
		m_nodes[i] = nullptr;

	//if (m_iLevel < MAX_LEVEL)
	//	split();
}

QuadTree::~QuadTree(){
	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		delete m_nodes[i];
	}
}

void QuadTree::Update(QuadTree* root){

	for (unsigned int i = 0; i < m_dynColliders.size(); ++i){
		Collider* collider = m_dynColliders[i];
		if (!m_bounds.Contains(collider->GetBox2D()) && m_iLevel != 0){
			m_dynColliders.erase(m_dynColliders.begin() + i);
			root->insert(collider);
		}
	}

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->Update(root);
	}
}

void QuadTree::clear(){
	m_staticColliders.clear();
	m_dynColliders.clear();

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->clear();
		delete m_nodes[i];
	}
}

void QuadTree::split(){
	Vector2 halfSize = m_bounds.size * 0.5f;
	Vector2 position = m_bounds.center;

	Vector2 actualSize = halfSize;
	actualSize.y = m_bounds.size.y;

	m_nodes[0] = new QuadTree(m_iLevel + 1, Box2D(Vector2(position.x + halfSize.x * 0.5f, position.y + halfSize.y * 0.5f), halfSize));
	m_nodes[1] = new QuadTree(m_iLevel + 1, Box2D(Vector2(position.x - halfSize.x * 0.5f, position.y + halfSize.y * 0.5f), halfSize));
	m_nodes[2] = new QuadTree(m_iLevel + 1, Box2D(Vector2(position.x + halfSize.x * 0.5f, position.y - halfSize.y * 0.5f), halfSize));
	m_nodes[3] = new QuadTree(m_iLevel + 1, Box2D(Vector2(position.x - halfSize.x * 0.5f, position.y - halfSize.y * 0.5f), halfSize));
}

bool QuadTree::insert(Collider* collider){
	if ((!m_bounds.Contains(collider->GetBox2D()) && m_iLevel != 0) || ContainsCollider(collider))
		return false;

	if (m_dynColliders.size() + m_staticColliders.size() < MAX_OBJECTS){
		(collider->IsStatic() ? m_staticColliders.push_back(collider) : m_dynColliders.push_back(collider));
		return true;
	}

	if (m_nodes[0] == nullptr && m_iLevel < MAX_LEVEL)
		split();

	if (m_nodes[0] != nullptr){
		for (unsigned int i = 0; i < NODE_COUNT; ++i){
			if (m_nodes[i]->insert(collider))
				return true;
		}
	}

	(collider->IsStatic() ? m_staticColliders.push_back(collider) : m_dynColliders.push_back(collider));

	return true;
}

void QuadTree::RetrieveStaticColliders(std::vector<Collider*>& returnColliders, const Box2D& bounds){

	if (!m_bounds.Intersects(bounds)) return;

	for (unsigned int i = 0; i < (unsigned int)m_staticColliders.size(); ++i){
		if (bounds.Intersects(m_staticColliders[i]->GetBox2D()))
			returnColliders.push_back(m_staticColliders[i]);
	}

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->RetrieveStaticColliders(returnColliders, bounds);
	}
}

void QuadTree::RetrieveDynamicColliders(std::vector<Collider*>& returnColliders, const Box2D& bounds){

	if (!m_bounds.Intersects(bounds)) return;

	for (unsigned int i = 0; i < (unsigned int)m_dynColliders.size(); ++i){
		if (bounds.Intersects(m_dynColliders[i]->GetBox2D()))
			returnColliders.push_back(m_dynColliders[i]);
	}

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->RetrieveDynamicColliders(returnColliders, bounds);
	}
}

void QuadTree::RetrieveStaticColliders(std::vector<Collider*>& returnColliders, Collider* collider){
	Box2D bounds = collider->GetBox2D();

	if (!m_bounds.Intersects(bounds)) return;

	int layerX = collider->gameObject->GetLayer();

	for (unsigned int i = 0; i < (unsigned int)m_staticColliders.size(); ++i){
		Collider* _collider = m_staticColliders[i];
		int layerY = _collider->gameObject->GetLayer();

		if (!Physics::CheckLayer(layerX, layerY) || collider == _collider || (collider->IsTrigger() && _collider->IsTrigger())) continue;

		if (collider->GetTimeStamp() < _collider->GetTimeStamp())
			returnColliders.push_back(_collider);
	}

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->RetrieveStaticColliders(returnColliders, collider);
	}
}

void QuadTree::RetrieveDynamicColliders(std::vector<Collider*>& returnColliders, Collider* collider){
	Box2D bounds = collider->GetBox2D();

	if (!m_bounds.Intersects(bounds)) return;

	int layerX = collider->gameObject->GetLayer();

	for (unsigned int i = 0; i < (unsigned int)m_dynColliders.size(); ++i){
		Collider* _collider = m_dynColliders[i];
		int layerY = _collider->gameObject->GetLayer();

		if (!Physics::CheckLayer(layerX, layerY) || collider == _collider || (collider->IsTrigger() && _collider->IsTrigger())) continue;

		if (collider->GetTimeStamp() < _collider->GetTimeStamp())
			returnColliders.push_back(_collider);
	}

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->RetrieveDynamicColliders(returnColliders, collider);
	}
}

void QuadTree::GetAllBounds(std::vector<NodeData>& returnData){
	returnData.push_back(NodeData(m_iLevel, m_bounds));

	if (m_nodes[0] == nullptr)
		return;

	for (int i = 0; i < NODE_COUNT; ++i){
		m_nodes[i]->GetAllBounds(returnData);
	}
}

bool QuadTree::remove(Collider* collider){

	//Box2D box = collider->GetBox2D();

	//if (!m_bounds.Intersects(box))return false;

	std::vector<Collider*>& colliders = (collider->IsStatic() ? m_staticColliders : m_dynColliders);
	for (unsigned int i = 0; i < (unsigned int)colliders.size(); ++i){
		if (colliders[i] == collider){
			colliders.erase(colliders.begin() + i);
			return true;
		}
	}

	if (m_nodes[0] == nullptr)
		return false;

	bool removeValue = false;
	for (int i = 0; i < NODE_COUNT; ++i){
		if (m_nodes[i]->remove(collider))
			removeValue = true;
	}

	return removeValue;
}

void QuadTree::OnDrawGizmos(Color* color){
	if (m_iLevel < MAX_LEVEL)
		Gizmos::SetColor(color[m_iLevel]);

	for (unsigned int i = 0; (unsigned int)i < m_staticColliders.size(); ++i){
		m_staticColliders[i]->OnDrawGizmos();
	}

	if (m_nodes[0] == nullptr) return;

	for (int i = 0; i < 4; ++i){
		m_nodes[i]->OnDrawGizmos(color);
	}
}

bool QuadTree::ContainsCollider(Collider* collider){
	std::vector<Collider*>& colliders = (collider->IsStatic() ? m_staticColliders : m_dynColliders);
	for (int i = 0; i < colliders.size(); ++i){
		if (colliders[i] == collider) return true;
	}

	return false;
}



