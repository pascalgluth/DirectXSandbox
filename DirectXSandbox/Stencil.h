#pragma once
#include <d3d11.h>

class Stencil
{
public:
    enum Mode { WRITE, MASK };

    Stencil(ID3D11Device* device, ID3D11DeviceContext* deviceContext, Mode mode);
    ~Stencil();

    void Bind();
    
private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;

    ID3D11DepthStencilState* m_stencil = nullptr;
    
};
