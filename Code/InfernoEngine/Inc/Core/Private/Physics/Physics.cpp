#include "Physics\Physics.h"
#include "GameObject.h"
#include "Components\Transform.h"
#include "Physics\CollisionLibrary.h"
#include "Gizmos.h"
#include "GameObject.h"

#define TREE_SIZE_X 200.0f
#define TREE_SIZE_Y 200.0f

Flags<char> Physics::s_LayerMatrix((int)CollisionLayers::LAYER_COUNT * (int)CollisionLayers::LAYER_COUNT);

Vector3 Physics::m_f3Gravity = Vector3(0.0f, -20.0f, 0.1f);

Vector3 Physics::Gravity() {
	return m_f3Gravity;
}

Physics::Physics() {

#pragma region Set up Collision
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_DEFAULT, (unsigned int)CollisionLayers::LAYER_STATIC_WORLD, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_DEFAULT, (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_DEFAULT, (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_DEFAULT, (unsigned int)CollisionLayers::LAYER_ENEMY, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_DEFAULT, (unsigned int)CollisionLayers::LAYER_FRIENDLY, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_DEFAULT, (unsigned int)CollisionLayers::LAYER_DEFAULT, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_FRIENDLY, (unsigned int)CollisionLayers::LAYER_STATIC_WORLD, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_FRIENDLY, (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_FRIENDLY, (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_FRIENDLY, (unsigned int)CollisionLayers::LAYER_ENEMY, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_FRIENDLY, (unsigned int)CollisionLayers::LAYER_FRIENDLY, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_ENEMY, (unsigned int)CollisionLayers::LAYER_STATIC_WORLD, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_ENEMY, (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_ENEMY, (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_ENEMY, (unsigned int)CollisionLayers::LAYER_ENEMY, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, (unsigned int)CollisionLayers::LAYER_STATIC_WORLD, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, (unsigned int)CollisionLayers::LAYER_STATIC_WORLD, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_STATIC_WORLD, (unsigned int)CollisionLayers::LAYER_STATIC_WORLD, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_BARRIER, (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_BARRIER, (unsigned int)CollisionLayers::LAYER_STATIC_WORLD, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_BARRIER, (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_BARRIER, (unsigned int)CollisionLayers::LAYER_ENEMY, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_BARRIER, (unsigned int)CollisionLayers::LAYER_FRIENDLY, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_BARRIER, (unsigned int)CollisionLayers::LAYER_DEFAULT, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_BARRIER, (unsigned int)CollisionLayers::LAYER_BARRIER, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_CAMERA, (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_CAMERA, (unsigned int)CollisionLayers::LAYER_STATIC_WORLD, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_CAMERA, (unsigned int)CollisionLayers::LAYER_FRIENDLY_PROJ, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_CAMERA, (unsigned int)CollisionLayers::LAYER_ENEMY, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_CAMERA, (unsigned int)CollisionLayers::LAYER_FRIENDLY, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_CAMERA, (unsigned int)CollisionLayers::LAYER_DEFAULT, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_CAMERA, (unsigned int)CollisionLayers::LAYER_BARRIER, true);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_CAMERA, (unsigned int)CollisionLayers::LAYER_CAMERA, false);
	SetCollisionBetweenLayers((unsigned int)CollisionLayers::LAYER_PLAYER_SHIELD, (unsigned int)CollisionLayers::LAYER_ENEMY_PROJ, true);
#pragma endregion

	root = new QuadTree(ROOT_LEVEL, Box2D(Vector2(0, 0), Vector2(TREE_SIZE_X, TREE_SIZE_Y)));
	color[0] = Color(1.0f, 0.0f, 0.0f, 1.0f);
	color[1] = Color(0.0f, 1.0f, 0.0f, 1.0f);
	color[2] = Color(0.0f, 0.0f, 1.0f, 1.0f);
	color[3] = Color(0.0f, 1.0f, 1.0f, 1.0f);
}

Physics::~Physics(){
	delete root;
	root = nullptr;
}

// IN: void
// OUT: void
//
// Update only called every 1/60th of second
void Physics::FixedUpdate(){
	UpdateRigidbodies();
	UpdateRemoveList();
	UpdateAddList();
	UpdateTree();
	UpdateCollisions();
	CollisionResolution();
	CollisionEvents();
}

// IN: Rigidbody - pointer to rigidbody to add
// OUT: void
//
// Adds a rigidbody to the registry
void Physics::AddRigidbody(Rigidbody* body){
	m_bodies.push_back(body);
}

// IN: Rigidbody - pointer to rigidbody to remove
// OUT: void
//
// Removes a rigidbody from the registry
void Physics::RemoveRigidbody(Rigidbody* body){
	for (auto iter = m_bodies.begin(); iter != m_bodies.end(); ++iter){
		if ((*iter) == body){
			m_bodies.erase(iter);
			break;
		}
	}
}

void Physics::UpdateAddList(){
	for (unsigned int i = 0; i < (unsigned int)m_addList.size(); ++i){
		Collider* collider = m_addList[i];
		collider->IsStatic() ? m_staticColliders.push_back(collider) : m_dynColliders.push_back(collider);
		root->insert(collider);
	}

	m_addList.clear();
}

void Physics::UpdateRemoveList(){
	for (unsigned int i = 0; i < (unsigned int)m_removeList.size(); ++i){
		Collider* collider = m_removeList[i];

		std::vector<Collider*>& colliders = collider->IsStatic() ? m_staticColliders : m_dynColliders;

		root->remove(collider);

		// === Is there nothing registered?
		if (m_dynColliders.size() == 0 && m_staticColliders.size() == 0) {
			for (unsigned int i = 0; i < m_addList.size(); ++i) {
				if (m_addList[i] == collider) {
					m_addList.erase(m_addList.begin() + i);
				}
			}
		}

		for (unsigned int i = 0; i < m_dynColliders.size(); ++i){
			if (m_dynColliders[i] != collider)
				m_dynColliders[i]->RemoveCollision(collider);
		}

		for (unsigned int i = 0; i < m_staticColliders.size(); ++i){
			if (m_staticColliders[i] != collider)
				m_staticColliders[i]->RemoveCollision(collider);
		}

		for (auto iter = colliders.begin(); iter != colliders.end(); ++iter){
			if ((*iter) == collider){
				iter = colliders.erase(iter);
				break;
			}
		}
	}

	m_removeList.clear();
}


// IN: Collider - pointer to collider to add
// OUT: void
//
// Adds a Collider to the registry
void Physics::AddCollider(Collider* collider){
	if (!ContainsCollider(collider))
		m_addList.push_back(collider);
}

// IN: Collider - pointer to collider to remove
// OUT: void
//
// Removes a collider from the registry
void Physics::RemoveCollider(Collider* collider){
	m_removeList.push_back(collider);
}

// IN: Ray - ray to check against colliders
// OUT: RayCastHit - information about what the ray hit
// 
// Checks to see if a ray collides with an object
bool Physics::Raycast(Ray& ray, RayCastHit& hitInfo){

	bool hit = false;
	for (unsigned int i = 0; i < m_dynColliders.size(); ++i){
		Collider* collider = m_dynColliders[i];
		if (CollisionLibrary::CheckRayToCollider(ray, collider, hitInfo))
			hit = true;
	}

	for (unsigned int i = 0; i < m_staticColliders.size(); ++i){
		Collider* collider = m_staticColliders[i];
		if (CollisionLibrary::CheckRayToCollider(ray, collider, hitInfo))
			hit = true;
	}

	return hit;
}

bool Physics::SphereOverlap(const Vector3& origin, const float radius, std::vector<RayCastHit>& hitInfo){

	Collider* collider = nullptr;
	RayCastHit hit;

	Box2D bounds(Vector2(origin.x, origin.z), Vector2(radius * 2.0f, radius * 2.0f));

	std::vector<Collider*> colliders;
	root->RetrieveStaticColliders(colliders, bounds);
	root->RetrieveDynamicColliders(colliders, bounds);

	for (unsigned int i = 0; i < (unsigned int)colliders.size(); ++i){
		collider = colliders[i];
		if (CollisionLibrary::CheckOverlapSphereToCollider(origin, radius, collider, hit))
			hitInfo.push_back(hit);
	}

	return hitInfo.size() != 0;
}


// IN: int, int - layers to check
// OUT: bool - returns true if two layers can collide
//
// Returns true if two layers can collide
bool Physics::CheckLayer(int layerX, int layerY){

	if (layerY > layerX)
		std::swap(layerX, layerY);

	int bit = layerY * ((int)CollisionLayers::LAYER_COUNT) + layerX;

	return s_LayerMatrix.GetFlag(bit);
}

void Physics::UpdateRigidbodies(){
	for (unsigned int i = 0; i < m_bodies.size(); ++i){
		m_bodies[i]->FixedUpdate();
	}
}

void Physics::UpdateTree(){

	if (root){
		root->Update(root);
	}
}

void Physics::UpdateCollisions(){
	if (root == nullptr) return;

	CollisionData data;
	for (unsigned int i = 0; i < (unsigned int)m_dynColliders.size(); ++i){
		Collider* lCollider = m_dynColliders[i];
		Box2D lBox = lCollider->GetBox2D();

		m_dynColliders[i]->SetTimeStamp(i);

		std::vector<Collider*> nearStaticColliders;
		root->RetrieveStaticColliders(nearStaticColliders, lCollider);

		std::vector<Collider*> nearDynamicColliders;
		root->RetrieveDynamicColliders(nearDynamicColliders, lCollider);

		for (unsigned int j = 0; j < (unsigned int)nearStaticColliders.size(); ++j){
			Collider* rCollider = nearStaticColliders[j];
			Box2D rBox = rCollider->GetBox2D();

			if (CollisionLibrary::CheckCollision(lCollider, rCollider, data)){
				if (!lCollider->IsTrigger() && !rCollider->IsTrigger())
					m_CollisionData.push_back(data);

				lCollider->UpdateCollision(rCollider, COL_STAY);
			}
			else{
				lCollider->UpdateCollision(rCollider, COL_EXIT);
			}
		}

		for (unsigned int j = 0; j < (unsigned int)nearDynamicColliders.size(); ++j){
			Collider* rCollider = nearDynamicColliders[j];
			Box2D rBox = rCollider->GetBox2D();

			if (CollisionLibrary::CheckCollision(lCollider, rCollider, data)){
				if (!lCollider->IsTrigger() && !rCollider->IsTrigger())
					m_CollisionData.push_back(data);

				lCollider->UpdateCollision(rCollider, COL_STAY);
			}
			else{
				lCollider->UpdateCollision(rCollider, COL_EXIT);
			}
		}
	}
}

void Physics::CollisionResolution(){
	for (unsigned int i = 0; i < m_CollisionData.size(); ++i){
		CollisionData& collision = m_CollisionData[i];
		Rigidbody* lRigidbody = collision.lCollider->gameObject->GetComponent<Rigidbody>();
		Rigidbody* rRigidbody = collision.rCollider->gameObject->GetComponent<Rigidbody>();

		if (!lRigidbody && !rRigidbody) continue;

		Vector3 pentrationResolution = collision.contactNormal * collision.penetration;
		pentrationResolution.y = 0.0f;

		if (lRigidbody && rRigidbody){

			float totalMass = lRigidbody->GetMass() + rRigidbody->GetMass();
			float rRatio = lRigidbody->GetMass() / totalMass;
			float lRatio = rRigidbody->GetMass() / totalMass;

			Vector3 newPosition = collision.lCollider->transform->GetPosition() + (pentrationResolution * lRatio);
			lRigidbody->MovePosition(newPosition);

			newPosition = collision.rCollider->transform->GetPosition() - (pentrationResolution * rRatio);
			rRigidbody->MovePosition(newPosition);
		}
		else if (lRigidbody && !rRigidbody){
			Vector3 newPosition = collision.lCollider->transform->GetPosition() + pentrationResolution;
			lRigidbody->MovePosition(newPosition);
		}
		else if (!lRigidbody && rRigidbody){
			Vector3 newPosition = collision.rCollider->transform->GetPosition() - pentrationResolution;
			rRigidbody->MovePosition(newPosition);
		}
	}

	m_CollisionData.clear();
}

void Physics::CollisionEvents(){
	for (unsigned int i = 0; i < m_dynColliders.size(); ++i){
		m_dynColliders[i]->CollisionEvents();
	}

	for (unsigned int i = 0; i < m_staticColliders.size(); ++i){
		m_staticColliders[i]->CollisionEvents();
	}
}

void Physics::SetCollisionBetweenLayers(unsigned int _layerX, unsigned int _layerY, bool _collision)
{
	if (_layerY > _layerX)
		std::swap(_layerX, _layerY);

	int bit = _layerY * ((int)CollisionLayers::LAYER_COUNT) + _layerX;

	s_LayerMatrix.SetFlag(bit, (unsigned int)_collision);
}

bool Physics::ContainsCollider(Collider* collider){

	for (unsigned int i = 0; i < m_addList.size(); ++i){
		if (m_addList[i] == collider) return true;
	}

	std::vector<Collider*>& colliders = collider->IsStatic() ? m_staticColliders : m_dynColliders;

	for (unsigned int i = 0; i < colliders.size(); ++i){
		if (colliders[i] == collider) return true;
	}

	return false;
}

void Physics::OnGizmosDraw(){
	if (root) root->OnDrawGizmos(color);
}



