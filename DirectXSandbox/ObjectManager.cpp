#include "ObjectManager.h"

#include "Engine.h"
#include "DirectXCollision.h"
#include "Logger.h"

ObjectManager::ObjectManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_device = device;
    m_deviceContext = deviceContext;
}

ObjectManager::~ObjectManager()
{
    auto itr = m_objects.begin();
    while (itr != m_objects.end())
    {
        delete itr->second;
        ++itr;
    }
}

void ObjectManager::Update(float dt)
{
    return;

    // todo
    
    auto itr = m_objects.begin();
    while (itr != m_objects.end())
    {
        
        ++itr;
    }
}

void ObjectManager::Render()
{
    auto itr = m_objects.begin();
    while (itr != m_objects.end())
    {
        if (VisibleGameObject* visibleObject = dynamic_cast<VisibleGameObject*>(itr->second))
        {
            visibleObject->Render(0);
        }
        
        ++itr;
    }

    Engine::GetGFX()->ClearDepthBuffer();

    itr = m_objects.begin();
    while (itr != m_objects.end())
    {
        if (VisibleGameObject* visibleObject = dynamic_cast<VisibleGameObject*>(itr->second))
        {
            visibleObject->Render(1);
        }
        
        ++itr;
    }
}

void ObjectManager::DeleteObjectByName(const std::string& name)
{
    auto result = m_objects.find(name);
    if (result != m_objects.end())
    {
        delete result->second;
        m_objects.erase(result);
    }
}

VisibleGameObject* ObjectManager::Pick(int mouseX, int mouseY)
{
    XMMATRIX projection = Engine::GetGFX()->m_camera.GetProjectionMatrix();
    XMFLOAT4X4 projection4x4;
    XMStoreFloat4x4(&projection4x4, projection);
    
    XMMATRIX view = Engine::GetGFX()->m_camera.GetViewMatrix();

    float vx = (2.f * mouseX / (float)Engine::GetGFX()->m_width - 1.f) / projection4x4(0,0);
    float vy = (-2.f * mouseY / (float)Engine::GetGFX()->m_height + 1.f) / projection4x4(1,1);

    XMVECTOR rayOrigin = XMVectorSet(0.f, 0.f, 0.f, 1.f);
    XMVECTOR rayDirection = XMVectorSet(vx, vy, 1.f, 0.f);

    XMVECTOR viewDeterm = XMMatrixDeterminant(view);
    XMMATRIX inverseView = XMMatrixInverse(&viewDeterm, view);

    auto itr = m_objects.begin();
    while (itr != m_objects.end())
    {
        if (VisibleGameObject* visObj = dynamic_cast<VisibleGameObject*>(itr->second))
        {
            std::vector<Mesh*>* meshes = visObj->GetMeshes();
            for (int i = 0; i < meshes->size(); ++i)
            {
                XMVECTOR worldMDeterm = XMMatrixDeterminant((*meshes)[i]->GetTransform());
                XMMATRIX inverseWorld = XMMatrixInverse(&worldMDeterm, (*meshes)[i]->GetTransform());
                XMMATRIX toLocal = XMMatrixMultiply(inverseView, inverseWorld);

                rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
                rayDirection = XMVector3TransformNormal(rayDirection, toLocal);
                rayDirection = XMVector3Normalize(rayDirection);

                for (int j = 0; j < (*meshes)[i]->Vertices.size()-2; ++j)
                {
                    XMVECTOR v1 = XMLoadFloat3(&(*meshes)[i]->Vertices[j].pos);
                    XMVECTOR v2 = XMLoadFloat3(&(*meshes)[i]->Vertices[j+1].pos);
                    XMVECTOR v3 = XMLoadFloat3(&(*meshes)[i]->Vertices[j+2].pos);

                    DirectX::FXMVECTOR xv1(v1);
                    DirectX::GXMVECTOR xv2(v2);
                    DirectX::HXMVECTOR xv3(v3);

                    FXMVECTOR origin(rayOrigin);
                    FXMVECTOR dir(rayDirection);
                    
                    float dist = 0.f;
                    if (DirectX::TriangleTests::Intersects(origin, dir, xv1, xv2, xv3, dist))
                    {
                        LOG_INFO("Collides");
                        return visObj;
                    }
                }
            }
        }
        ++itr;
    }

    return nullptr;
}

std::string ObjectManager::GetObjectName(GameObject* object)
{
    auto itr = m_objects.begin();
    while (itr != m_objects.end())
    {
        if (itr->second == object)
            return itr->first;
        ++itr;
    }
    return "";
}
