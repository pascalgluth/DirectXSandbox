#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct GlobalCBuffer
{
    DirectX::XMMATRIX vpMatrix;
    DirectX::XMFLOAT3 eyePos;
    float pad;
};

struct ObjectCBuffer
{
    DirectX::XMMATRIX worldMatrix;
};



template<class T>
class ConstantBuffer
{
public:
    ConstantBuffer() {}

    T Data;

    ID3D11Buffer* const* GetBuffer() const { return &m_buffer; }

    bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
    {
        m_deviceContext = deviceContext;

        if (m_buffer)
        {
            m_buffer->Release();
        }

        CD3D11_BUFFER_DESC bufferDesc(static_cast<UINT>(sizeof(T) + (16 - (sizeof(T) % 16))),
            D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
        HRESULT hr = device->CreateBuffer(&bufferDesc, 0, &m_buffer);
        if (FAILED(hr)) return false;

        return true;
    }

    bool ApplyChanges()
    {
        D3D11_MAPPED_SUBRESOURCE resource;
        HRESULT hr = m_deviceContext->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
        if (FAILED(hr)) return false;

        CopyMemory(resource.pData, &Data, sizeof(T));
        m_deviceContext->Unmap(m_buffer, 0);

        return true;
    }

private:
    ID3D11Buffer* m_buffer = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    
};
