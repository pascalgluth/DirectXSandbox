#include "ObjectManager.h"

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
            visibleObject->Render();
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
