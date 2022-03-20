#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <string>

class VertexShader
{
public:
    ~VertexShader();
    bool Init(ID3D11Device* device, std::wstring path, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc, UINT inputElementCount);
    ID3D11VertexShader* GetShader() const { return m_shader; }
    ID3D10Blob* GetBuffer() const { return m_buffer; }
    ID3D11InputLayout* GetInputLayout() const { return m_inputLayout; }

private:
    ID3D11VertexShader* m_shader = nullptr;
    ID3D10Blob* m_buffer = nullptr;
    ID3D11InputLayout* m_inputLayout = nullptr;
    
};

class PixelShader
{
public:
    ~PixelShader();
    bool Init(ID3D11Device* device, std::wstring path);
    ID3D11PixelShader* GetShader() const { return m_shader; }
    ID3D10Blob* GetBuffer() const { return m_buffer; }

private:
    ID3D11PixelShader* m_shader = nullptr;
    ID3D10Blob* m_buffer = nullptr;
    
};