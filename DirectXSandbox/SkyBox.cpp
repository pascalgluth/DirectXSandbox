#include "SkyBox.h"
#include <DDSTextureLoader.h>

#include "Graphics.h"
#include "Logger.h"

bool SkyBox::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
    if (!VisibleGameObject::Init(device, deviceContext))
        return false;

    D3D11_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create rasterizer state", hr);
        return false;
    }

    D3D11_INPUT_ELEMENT_DESC vertexShaderInputLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };
    
    if (!m_vertexShader.Init(m_device, L"../bin/skyboxvs.cso", vertexShaderInputLayout, ARRAYSIZE(vertexShaderInputLayout)))
        return false;

    if (!m_pixelShader.Init(m_device, L"../bin/skyboxps.cso"))
        return false;

    hr = CreateDDSTextureFromFile(m_device, L"../bin/Content/Textures/skymap1.dds", NULL, &m_cubeMap);
    if (FAILED(hr)) return false;

    CreateSphere(10, 10);

    SetScale(10000.f, 10000.f, 10000.f);
    
    return true;
}

SkyBox::~SkyBox()
{
    m_cubeMap->Release();
    m_rasterizerState->Release();
}

void SkyBox::Render()
{
    m_deviceContext->IASetInputLayout(m_vertexShader.GetInputLayout());
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_deviceContext->PSSetShaderResources(0, 1, &m_cubeMap);
    
    m_deviceContext->VSSetShader(m_vertexShader.GetShader(), NULL, 0);
    m_deviceContext->PSSetShader(m_pixelShader.GetShader(), NULL, 0);

    m_deviceContext->RSSetState(m_rasterizerState);
    
    VisibleGameObject::Render();
}

void SkyBox::CreateSphere(int latlines, int longlines)
{
    int numSphereVertices = ((latlines-2) * longlines) + 2;
    int numSphereFaces  = ((latlines-3)*(longlines)*2) + (longlines*2);

    float sphereYaw = 0.0f;
    float spherePitch = 0.0f;

    XMMATRIX rotationX;
    XMMATRIX rotationY;

    std::vector<Vertex> vertices(numSphereVertices);

    XMVECTOR currVertPos = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    vertices[0].pos.x = 0.0f;
    vertices[0].pos.y = 0.0f;
    vertices[0].pos.z = 1.0f;

    for(int i = 0; i < latlines-2; ++i)
    {
        spherePitch = ((float)i+1.f) * (3.14f/((float)latlines-1.f));
        rotationX = XMMatrixRotationX(spherePitch);
        for(int j = 0; j < longlines; ++j)
        {
            sphereYaw = (float)j * (6.28f/(float)longlines);
            rotationY = XMMatrixRotationZ(sphereYaw);
            currVertPos = XMVector3TransformNormal( XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (rotationX * rotationY) );    
            currVertPos = XMVector3Normalize( currVertPos );
            vertices[i*longlines+j+1].pos.x = XMVectorGetX(currVertPos);
            vertices[i*longlines+j+1].pos.y = XMVectorGetY(currVertPos);
            vertices[i*longlines+j+1].pos.z = XMVectorGetZ(currVertPos);
        }
    }

    vertices[numSphereVertices-1].pos.x =  0.0f;
    vertices[numSphereVertices-1].pos.y =  0.0f;
    vertices[numSphereVertices-1].pos.z = -1.0f;


    std::vector<DWORD> indices(numSphereFaces * 3);

    int k = 0;
    for(int l = 0; l < longlines-1; ++l)
    {
        indices[k] = 0;
        indices[k+1] = l+1;
        indices[k+2] = l+2;
        k += 3;
    }

    indices[k] = 0;
    indices[k+1] = longlines;
    indices[k+2] = 1;
    k += 3;

    for(int i = 0; i < latlines-3; ++i)
    {
        for(int j = 0; j < longlines-1; ++j)
        {
            indices[k]   = i*longlines+j+1;
            indices[k+1] = i*longlines+j+2;
            indices[k+2] = (i+1)*longlines+j+1;

            indices[k+3] = (i+1)*longlines+j+1;
            indices[k+4] = i*longlines+j+2;
            indices[k+5] = (i+1)*longlines+j+2;

            k += 6; // next quad
        }

        indices[k]   = (i*longlines)+longlines;
        indices[k+1] = (i*longlines)+1;
        indices[k+2] = ((i+1)*longlines)+longlines;

        indices[k+3] = ((i+1)*longlines)+longlines;
        indices[k+4] = (i*longlines)+1;
        indices[k+5] = ((i+1)*longlines)+1;

        k += 6;
    }

    for(int l = 0; l < longlines-1; ++l)
    {
        indices[k] = numSphereVertices-1;
        indices[k+1] = (numSphereVertices-1)-(l+1);
        indices[k+2] = (numSphereVertices-1)-(l+2);
        k += 3;
    }

    indices[k] = numSphereVertices-1;
    indices[k+1] = (numSphereVertices-1)-longlines;
    indices[k+2] = numSphereVertices-2;


    m_meshes.emplace_back(new Mesh(m_device, m_deviceContext, vertices, indices, XMMatrixIdentity()));
}
