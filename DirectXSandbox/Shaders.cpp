#include "Shaders.h"

VertexShader::~VertexShader()
{
    m_shader->Release();
    m_buffer->Release();
    m_inputLayout->Release();
}

bool VertexShader::Init(ID3D11Device* device, std::wstring path, D3D11_INPUT_ELEMENT_DESC* inputLayoutDesc,
    UINT inputElementCount)
{
    HRESULT hr;

    hr = D3DReadFileToBlob(path.c_str(), &m_buffer);
    if (FAILED(hr)) return false;

    hr = device->CreateVertexShader(m_buffer->GetBufferPointer(), m_buffer->GetBufferSize(), NULL, &m_shader);
    if (FAILED(hr)) return false;

    hr = device->CreateInputLayout(inputLayoutDesc, inputElementCount, m_buffer->GetBufferPointer(), m_buffer->GetBufferSize(), &m_inputLayout);
    if (FAILED(hr)) return false;

    return true;
}


PixelShader::~PixelShader()
{
    m_shader->Release();
    m_buffer->Release();
}

bool PixelShader::Init(ID3D11Device* device, std::wstring path)
{
    HRESULT hr;

    hr = D3DReadFileToBlob(path.c_str(), &m_buffer);
    if (FAILED(hr)) return false;

    hr = device->CreatePixelShader(m_buffer->GetBufferPointer(), m_buffer->GetBufferSize(), NULL, &m_shader);
    if (FAILED(hr)) return false;

    return true;
}
