#pragma once

#include "ConstantBuffer.h"
#include "DirectXMath.h"

#include <type_traits>
#include <string>

#include "ImGui/imgui.h"

namespace LightCBuffer
{
    struct LightCBuffer
    {
    };

    struct PSAmbientLight : LightCBuffer
    {
        DirectX::XMFLOAT3 ambientLightColor;
        float ambientLightStrength;
    };

    struct PSPointLight : LightCBuffer
    {
        DirectX::XMFLOAT3 pointLightColor;
        float pointLightStrength;
        DirectX::XMFLOAT3 pointLightPosition;
        float pointLightAttenuation;
        float pointLightMaximumCalcDistance;
    };

    struct PSSpotLight : LightCBuffer
    {
        DirectX::XMFLOAT3 spotLightPosition;
        float spotLightStrength;
        DirectX::XMFLOAT3 spotLightDirection;
        float spotLightDistance;
    };
}

template <typename LightType>
class Light
{
public:
    void Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    {
        static_assert(std::is_base_of<LightCBuffer::LightCBuffer, LightType>::value, "LightType must be a LightCBuffer."
        );

        m_device = device;
        m_deviceContext = deviceContext;

        m_lightCBuffer.Init(m_device, m_deviceContext);
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
                ImGui::DragFloat("##ambientLightStrength", &pLight->GetCBuffer()->Data.ambientLightStrength, 0.01f, 0.f, 1.f, "%.2f");

                ImGui::Text("Color:    ");
                ImGui::SameLine();
                ImGui::DragFloat3("##ambientLightColor", &pLight->GetCBuffer()->Data.ambientLightColor.x, 0.01f, 0.f, 1.f, "%.2f");
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
                ImGui::DragFloat("##pointLightStrength", &pLight->GetCBuffer()->Data.pointLightStrength, 0.01f, 0.f, 100.f,
                                 "%.2f");

                ImGui::Text("Position:    ");
                ImGui::SameLine();
                ImGui::DragFloat3("##pointLightPosition", &pLight->GetCBuffer()->Data.pointLightPosition.x, 0.1f);

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
                ImGui::DragFloat("##spotLightStrength", &pLight->GetCBuffer()->Data.spotLightStrength, 0.01f, 0.f, 100.f,
                                 "%.2f");

                ImGui::Text("Position:    ");
                ImGui::SameLine();
                ImGui::DragFloat3("##spotLightPosition", &pLight->GetCBuffer()->Data.spotLightPosition.x, 0.1f);

                ImGui::Text("Direction:    ");
                ImGui::SameLine();
                ImGui::DragFloat3("##spotLightDirection", &pLight->GetCBuffer()->Data.spotLightDirection.x, 0.1f);
            }
        }
        ImGui::End();
    }
}
