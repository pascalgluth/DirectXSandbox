#pragma once

#include <reactphysics3d/reactphysics3d.h>

#include "VisibleGameObject.h"

using namespace reactphysics3d;

class RigidBodyObject : public VisibleGameObject
{
public:
	~RigidBodyObject() override;
	void SetupPhysics(PhysicsCommon* physicsCommon, PhysicsWorld* physicsWorld);
	void Update(float dt);

	Collider* GetCollider() { return m_collider; }

	void SetPosition(const DirectX::XMFLOAT3& pos) override;
	
private:
	RigidBody* m_rigidBody = nullptr;
	BoxShape* m_boxShape = nullptr;
	Collider* m_collider = nullptr;
	
};
