#pragma once

#include <d3d11.h>

#include "Shaders.h"
#include "VisibleGameObject.h"

class SkyBox : public VisibleGameObject
{
public:
    bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;
    ~SkyBox() override;
    
    void Render() override;
    
private:
    void CreateSphere(int latlines, int longlines);

    VertexShader m_vertexShader;
    PixelShader m_pixelShader;
    
    ID3D11ShaderResourceView* m_cubeMap = nullptr;
    ID3D11RasterizerState* m_rasterizerState = nullptr;
    
};
