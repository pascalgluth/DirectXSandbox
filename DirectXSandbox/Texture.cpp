#include "Texture.h"

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

#include "Logger.h"

bool checkFileExtension(const std::string& name, const std::string& extension)
{
    if (name.substr(name.find_last_of(".") + 1) == extension)
        return true;
    
    return false;
}

Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const std::string& filePath, UINT slot)
{
    m_slot = slot;
    
    HRESULT hr;
    std::wstring wfilepath(filePath.begin(), filePath.end());
    
    if (checkFileExtension(filePath, "dds"))
        hr = DirectX::CreateDDSTextureFromFile(device, wfilepath.c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
    else
    {
        hr = DirectX::CreateWICTextureFromFile(device, wfilepath.c_str(), m_texture.GetAddressOf(), m_textureView.GetAddressOf());
        //deviceContext->GenerateMips(m_textureView.Get()); 
    }

    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to load texture: " + filePath + ".", hr);
        CreateColorTexture(device, 255, 0, 0, 255);
        return;
    }
}

Texture::Texture(ID3D11Device* device, UINT r, UINT g, UINT b, UINT a, UINT slot)
{
    m_slot = slot;
    CreateColorTexture(device, r, g, b, a);
}

Texture::~Texture()
{
}

void Texture::CreateColorTexture(ID3D11Device* device, UINT r, UINT g, UINT b, UINT a)
{
    CD3D11_TEXTURE2D_DESC textureDesc(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1);
    ID3D11Texture2D* texture2D;
    D3D11_SUBRESOURCE_DATA initData{};

    // todo: colored texture
}
