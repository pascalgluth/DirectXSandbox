#pragma once

#include <d3d11.h>
#include <string>
#include <wrl/client.h>
#include <assimp/material.h>

class Texture
{
public:
    Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath, UINT slot);
    Texture(ID3D11Device* device, UINT r, UINT g, UINT b, UINT a, UINT slot);
    ~Texture();

    UINT GetSlot() { return m_slot; }
    ID3D11ShaderResourceView* GetTextureResourceView() { return m_textureView.Get(); }
    ID3D11ShaderResourceView** GetTextureResourceViewAddress() { return m_textureView.GetAddressOf(); }
    
private:
    Microsoft::WRL::ComPtr<ID3D11Resource> m_texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;

    void CreateColorTexture(ID3D11Device* device, UINT r, UINT g, UINT b, UINT a);

    UINT m_slot;
    
};
