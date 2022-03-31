#include "Stencil.h"

#include "Logger.h"

Stencil::Stencil(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Mode mode)
{
    m_device = device;
    m_deviceContext = deviceContext;
    
    CD3D11_DEPTH_STENCIL_DESC stencilDesc = CD3D11_DEPTH_STENCIL_DESC{CD3D11_DEFAULT{}};

    if (mode == Mode::WRITE)
    {
        stencilDesc.StencilEnable = TRUE;
        stencilDesc.StencilWriteMask = 0xFF;
        stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
    }
    else if (mode == Mode::MASK)
    {
        stencilDesc.DepthEnable = FALSE;
        stencilDesc.StencilEnable = TRUE;
        stencilDesc.StencilReadMask = 0xFF;
        stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
        stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    }

    HRESULT hr = m_device->CreateDepthStencilState(&stencilDesc, &m_stencil);
    if (FAILED(hr)) LOG_ERROR_HR("Failed to create depth stencil", hr);
}

Stencil::~Stencil()
{
    if (m_stencil) m_stencil->Release();
}

void Stencil::Bind()
{
    m_deviceContext->OMSetDepthStencilState(m_stencil, 0xFF);
}
