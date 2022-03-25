#include "Graphics2D.h"

bool Graphics2D::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    m_device = device;
    m_deviceContext = deviceContext;

    return true;
}

void Graphics2D::SetViewport(float width, float height, float nearZ, float farZ)
{
    m_orthoMatrix = DirectX::XMMatrixOrthographicOffCenterLH(0.f, width, height, 0.f, nearZ, farZ);
}

