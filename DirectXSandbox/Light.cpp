#include "Light.h"

#include <map>

struct LightConstantBuffer
{
    LightCBuffer::PSAmbientLight ambientLight;
    LightCBuffer::PSPointLight pointLight;
};

ID3D11Device* device;
ID3D11DeviceContext* deviceContext;

ConstantBuffer<LightConstantBuffer> lightBuffer;

bool Light::Init(ID3D11Device* _device, ID3D11DeviceContext* _deviceContext)
{
    device = _device;
    deviceContext = _deviceContext;

    if (!lightBuffer.Init(device, deviceContext))
        return false;

    return true;
}

void Light::RenderLights(UINT cbslot)
{
    lightBuffer.ApplyChanges();
    deviceContext->PSSetConstantBuffers(cbslot, 1, lightBuffer.GetBuffer());
}

LightCBuffer::PSAmbientLight* Light::GetAmbientLight()
{
    return &lightBuffer.Data.ambientLight;
}

LightCBuffer::PSPointLight* Light::GetPointLight()
{
    return &lightBuffer.Data.pointLight;
}
