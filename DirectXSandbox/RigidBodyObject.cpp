#include "RigidBodyObject.h"

RigidBodyObject::~RigidBodyObject()
{
}

void RigidBodyObject::SetupPhysics(PhysicsCommon* physicsCommon, PhysicsWorld* physicsWorld)
{
    DirectX::XMFLOAT3 pos = GetPositonF3();
    Vector3 position(pos.x, pos.y, pos.z);
    DirectX::XMFLOAT3 rot = GetRotationF3();
    Transform transform(position, Quaternion::fromEulerAngles(rot.x, rot.y, rot.z));
    
    m_rigidBody = physicsWorld->createRigidBody(transform);
    m_boxShape = physicsCommon->createBoxShape({5.f, 10.f, 2.f});
    m_collider = m_rigidBody->addCollider(m_boxShape, transform);
    m_rigidBody->setMass(500.f);
}

void RigidBodyObject::Update(float dt)
{
    const Transform& transform = m_rigidBody->getTransform();
    const Vector3& position = transform.getPosition();
    const Quaternion& rotation = transform.getOrientation();
    const Vector3& rotationVec = rotation.getVectorV();
    
    SetPosition(DirectX::XMFLOAT3(position.x, position.y, position.z));
    SetRotation(DirectX::XMFLOAT3(rotationVec.x, rotationVec.y, rotationVec.z));
}

void RigidBodyObject::SetPosition(const DirectX::XMFLOAT3& pos)
{
    VisibleGameObject::SetPosition(pos);

    if (!m_rigidBody) return;
    
    Vector3 position(pos.x, pos.y, pos.z);
    Quaternion orientation = m_rigidBody->getTransform().getOrientation();
    Transform newTransform(position, orientation);
    m_rigidBody->setTransform(newTransform);
}
