#pragma once

#include <d3d11.h>
#include <map>
#include <string>
#include <type_traits>

#include "GameObject.h"
#include "VisibleGameObject.h"

class ObjectManager
{
public:
    ObjectManager(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    ~ObjectManager();

    void Update(float dt);
    void Render();

    std::map<std::string, GameObject*>* GetAllObjects() { return &m_objects; }

    template<class T>
    GameObject* CreateObject(const std::string& name)
    {
        static_assert(std::is_base_of<GameObject, T>::value, "Object type needs to be a GameObject");

        GameObject* newObj = new T();
        m_objects.emplace(std::pair<std::string, GameObject*>(name, newObj));

        return newObj;
    }

    template<class T>
    VisibleGameObject* CreateVisibleObject(const std::string& name, const std::string& model)
    {
        static_assert(std::is_base_of<VisibleGameObject, T>::value, "Object type needs to be a VisibleGameObject");

        VisibleGameObject* newObject = new T();
        if (!newObject) return nullptr;

        if (!newObject->InitAsync(model, m_device, m_deviceContext))
            return nullptr;

        m_objects.emplace(std::pair(name, newObject));
        
        return newObject;
    }

    template<class T>
    GameObject* GetObjectByName(const std::string& name)
    {
        static_assert(std::is_base_of<GameObject, T>::value, "Object type needs to be a GameObject");

        auto itr = m_objects.begin();
        while (itr != m_objects.end())
        {
            if (itr->first == name)
            {
                return dynamic_cast<T*>(itr->second);
            }
            
            ++itr;
        }

        return nullptr;
    }

    void DeleteObjectByName(const std::string& name);

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;

    std::map<std::string, GameObject*> m_objects;
    
};

