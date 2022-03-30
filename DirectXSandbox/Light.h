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
    struct PSAmbientLight
    {
        DirectX::XMFLOAT4 ambient = {0.3f, 0.3f, 0.3f, 1.f};
        DirectX::XMFLOAT4 diffuse = {0.7f, 0.7f, 0.7f, 1.f};
        DirectX::XMFLOAT4 specular = {0.7f, 0.7f, 0.7f, 1.f};
    };

    struct PSPointLight
    {
        DirectX::XMFLOAT4 ambient = {0.3f, 0.3f, 0.3f, 1.f};
        DirectX::XMFLOAT4 diffuse = {0.7f, 0.7f, 0.7f, 1.f};
        DirectX::XMFLOAT4 specular = {0.7f, 0.7f, 0.7f, 1.f};
        
        DirectX::XMFLOAT3 position = {0.f, 0.f, 0.f};
        float range = 10000.f;

        DirectX::XMFLOAT3 attenuation = {0003.f, 0.1f, 0.f};
        float padding;
    };

    struct PSSpotLight
    {
        DirectX::XMFLOAT3 position;
        float range;

        DirectX::XMFLOAT3 direction;
        float spot;

        DirectX::XMFLOAT3 attenuation;
        float padding;
    };
}

namespace Light
{
    bool Init(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext);
    void RenderLights(UINT cbslot);

    LightCBuffer::PSAmbientLight* GetAmbientLight();
    LightCBuffer::PSPointLight* GetPointLight();
}

/*template <typename LightType>
class Light : public VisibleGameObject
{
public:
    bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override
    {
        static_assert(std::is_base_of<LightCBuffer::LightCBuffer, LightType>::value, "LightType must be a LightCBuffer.");

        std::string textures[1];
        std::string model;
        
        if (std::is_same<LightCBuffer::PSPointLight, LightType>::value)
            model = FILE_MODEL("point_light.obj");
        else if (std::is_same<LightCBuffer::PSAmbientLight, LightType>::value)
            model = FILE_MODEL("ambient_light.obj");
        else if (std::is_same<LightCBuffer::PSSpotLight, LightType>::value)
            model = FILE_MODEL("spot_light.obj");
        else
            model = FILE_MODEL("default_light.obj");

        if (!VisibleGameObject::Init(model, device, deviceContext))
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
        m_lightCBuffer.ApplyChanges();
        m_deviceContext->PSSetConstantBuffers(slot, 1, m_lightCBuffer.GetBuffer());
    }
    
    ConstantBuffer<LightType>* GetCBuffer() { return &m_lightCBuffer; }

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;

    static ConstantBuffer<LightCBuffer> m_lightCBuffer;
    
};*/

namespace LightGui
{
    inline void RenderAmbientLightGui()
    {
        ImGui::Begin("Ambient Light");
        {
            ImGui::DragFloat4("Ambient", &Light::GetAmbientLight()->ambient.x, 0.01f);
        }
        ImGui::End();
    }

    inline void RenderPointLightGui()
    {
        ImGui::Begin("Point Light");
        {
            ImGui::DragFloat4("Ambient", &Light::GetPointLight()->ambient.x, 0.01f);
            ImGui::DragFloat4("Diffuse", &Light::GetPointLight()->diffuse.x, 0.01f);
            ImGui::DragFloat4("Specular", &Light::GetPointLight()->specular.x, 0.01f);
            ImGui::DragFloat3("Position", &Light::GetPointLight()->position.x, 0.1f);
            ImGui::DragFloat("Range", &Light::GetPointLight()->range, 0.1f);
            ImGui::DragFloat3("Attenuation", &Light::GetPointLight()->attenuation.x, 0.001f);
        }
        ImGui::End();
    }

    inline void RenderSpotLightGui()
    {
        /*ImGui::Begin("Spot Light");
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
        ImGui::End();*/
    }
}
