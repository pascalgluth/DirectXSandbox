#pragma once

#include "ConstantBuffer.h"
#include "DirectXMath.h"

#include <type_traits>
#include <string>

#include "DirectoryHelperMacros.h"
#include "VisibleGameObject.h"
#include "ImGui/imgui.h"

namespace LightCBuffer
{
    struct LightCBuffer
    {
        DirectX::XMFLOAT3 position;
        float strength = 10.f;
        DirectX::XMFLOAT3 direction;
        float padding;
        DirectX::XMFLOAT3 color;
    };

    struct PSAmbientLight : LightCBuffer
    {
    };

    struct PSPointLight : LightCBuffer
    {
        float pointLightAttenuation;
        float pointLightMaximumCalcDistance;
    };

    struct PSSpotLight : LightCBuffer
    {
        float spotLightDistance;
        float spotLightAttenuation = 1.f;
    };
}

template <typename LightType>
class Light : public VisibleGameObject
{
public:
    bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override
    {
        static_assert(std::is_base_of<LightCBuffer::LightCBuffer, LightType>::value, "LightType must be a LightCBuffer.");

        std::string textures[1];
        std::string model;
        
        if (std::is_same<LightCBuffer::PSPointLight, LightType>::value)
        {
            textures[0] = FILE_TEXTURE("point_light.png");
            model = FILE_MODEL("point_light.obj");
        }
        else if (std::is_same<LightCBuffer::PSAmbientLight, LightType>::value)
        {
            textures[0] = FILE_TEXTURE("ambient_light.png");
            model = FILE_MODEL("ambient_light.obj");
        }
        else if (std::is_same<LightCBuffer::PSSpotLight, LightType>::value)
        {
            textures[0] = FILE_TEXTURE("spot_light.png");
            model = FILE_MODEL("spot_light.obj");
        }
        else
        {
            textures[0] = FILE_TEXTURE("default_light.png");
            model = FILE_MODEL("default_light.obj");
        }

        if (!VisibleGameObject::Init(model, &textures[0], 1, device, deviceContext))
            return false;

        m_device = device;
        m_deviceContext = deviceContext;

        if (!m_lightCBuffer.Init(m_device, m_deviceContext))
            return false;

        AddRotationOffset(1.f, 0.f, -1.f);

        return true;
    }

    void ApplyBuffer(UINT slot)
    {
        GetCBuffer()->Data.position = GetPositonF3();
        GetCBuffer()->Data.direction = GetUpVectorF3();
        
        m_lightCBuffer.ApplyChanges();
        m_deviceContext->PSSetConstantBuffers(slot, 1, m_lightCBuffer.GetBuffer());
    }
    
    ConstantBuffer<LightType>* GetCBuffer() { return &m_lightCBuffer; }

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;

    ConstantBuffer<LightType> m_lightCBuffer;
    
};

namespace LightGui
{
    inline void RenderAmbientLightGui(Light<LightCBuffer::PSAmbientLight>* pLight)
    {
        ImGui::Begin("Ambient Light");
        {
            if (pLight)
            {
                ImGui::Text("Strength: ");
                ImGui::SameLine();
                ImGui::DragFloat("##ambientLightStrength", &pLight->GetCBuffer()->Data.strength, 0.01f, 0.f, 1.f, "%.2f");

                ImGui::Text("Color:    ");
                ImGui::SameLine();
                ImGui::DragFloat3("##ambientLightColor", &pLight->GetCBuffer()->Data.color.x, 0.01f, 0.f, 1.f, "%.2f");
            }
        }
        ImGui::End();
    }

    inline void RenderPointLightGui(Light<LightCBuffer::PSPointLight>* pLight)
    {
        ImGui::Begin("Point Light");
        {
            if (pLight)
            {
                ImGui::Text("Strength:    ");
                ImGui::SameLine();
                ImGui::DragFloat("##pointLightStrength", &pLight->GetCBuffer()->Data.strength, 0.01f, 0.f, 100.f,
                                 "%.2f");

                static DirectX::XMFLOAT3 position = pLight->GetPositonF3();

                ImGui::Text("Position:    ");
                ImGui::SameLine();
                ImGui::DragFloat3("##pointLightPosition", &position.x, 0.1f);

                pLight->SetPosition(position);

                ImGui::Text("Attenuation: ");
                ImGui::SameLine();
                ImGui::DragFloat("##pointLightAttenuation", &pLight->GetCBuffer()->Data.pointLightAttenuation, 0.01f, 0.f, 10.f,
                                 "%.2f");

                ImGui::Text("Max dist.:  ");
                ImGui::SameLine();
                ImGui::DragFloat("##pointLightMaxCalcDistance", &pLight->GetCBuffer()->Data.pointLightMaximumCalcDistance,
                                 0.1f);
            }
        }
        ImGui::End();
    }

    inline void RenderSpotLightGui(Light<LightCBuffer::PSSpotLight>* pLight)
    {
        ImGui::Begin("Spot Light");
        {
            if (pLight)
            {
                ImGui::Text("Strength:    ");
                ImGui::SameLine();
                ImGui::DragFloat("##spotLightStrength", &pLight->GetCBuffer()->Data.strength, 0.01f, 0.f, 100.f,
                                 "%.2f");

                static DirectX::XMFLOAT3 position = pLight->GetPositonF3();
                
                ImGui::Text("Position:    ");
                ImGui::SameLine();
                ImGui::DragFloat3("##spotLightPosition", &position.x, 0.1f);

                pLight->SetPosition(position);

                static DirectX::XMFLOAT3 rotation = pLight->GetRotationF3();
                
                ImGui::Text("Direction:    ");
                ImGui::SameLine();
                ImGui::DragFloat3("##spotLightDirection", &rotation.x, 0.1f);

                pLight->SetRotation(rotation);

                ImGui::Text("Attenuation: ");
                ImGui::SameLine();
                ImGui::DragFloat("##spotLightAttenuation", &pLight->GetCBuffer()->Data.spotLightAttenuation, 0.01f, 0.01f, 10.f,
                                 "%.2f");

                ImGui::Text("Radius: ");
                ImGui::SameLine();
                ImGui::DragFloat("##spotLightAngle", &pLight->GetCBuffer()->Data.spotLightDistance, 0.01f, 0.01f, 10.f,
                                 "%.2f");
            }
        }
        ImGui::End();
    }
}
