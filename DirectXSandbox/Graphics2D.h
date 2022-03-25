#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

class Graphics2D
{
public:
    bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void SetViewport(float width, float height, float nearZ, float farZ);

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;

    DirectX::XMMATRIX m_orthoMatrix;
    
};
