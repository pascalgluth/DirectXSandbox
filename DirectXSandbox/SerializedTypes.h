#pragma once

#include <DirectXMath.h>
#include <string>
#include <vector>
#include <json/json_struct.h>

#include "GameObject.h"
#include "ObjectManager.h"
#include "VisibleGameObject.h"

struct SerializedFloat3
{
    float x;
    float y;
    float z;

    SerializedFloat3()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    SerializedFloat3(const DirectX::XMFLOAT3& directxf3)
    {
        x = directxf3.x;
        y = directxf3.y;
        z = directxf3.z;
    }

    DirectX::XMFLOAT3 DXF3()
    {
        return {x, y, z};
    }

    JS_OBJ(x, y, z);
};

struct SerializedObject
{
    std::string name;
    bool isVisibleObject;
    std::string model;
    std::string texture;
    SerializedFloat3 position;
    SerializedFloat3 rotation;
    SerializedFloat3 scale;

    SerializedObject() {}

    SerializedObject(const std::string& _name, GameObject* object)
        : position(object->GetPositonF3()),
          rotation(object->GetRotationF3()),
          scale(object->GetScaleF3())
    {
        name = _name;
        isVisibleObject = false;
    }

    SerializedObject(const std::string& _name, VisibleGameObject* object)
        : position(object->GetPositonF3()),
          rotation(object->GetRotationF3()),
          scale(object->GetScaleF3())
    {
        name = _name;
        isVisibleObject = true;
        model = object->GetModelFile();
        texture = object->GetTextureFile();
    }

    JS_OBJ(name, isVisibleObject, model, texture, position, rotation, scale);
};

struct SerializedScene
{
    std::vector<SerializedObject> objects;

    SerializedScene() {}

    SerializedScene(ObjectManager* objManager)
    {
        std::map<std::string, GameObject*>* allObjects = objManager->GetAllObjects();

        auto itr = allObjects->begin();
        while (itr != allObjects->end())
        {
            if (VisibleGameObject* visibleObject = dynamic_cast<VisibleGameObject*>(itr->second))
                objects.emplace_back(itr->first, visibleObject);
            else
                objects.emplace_back(itr->first, itr->second);
            ++itr;
        }
    }

    JS_OBJ(objects);
};
