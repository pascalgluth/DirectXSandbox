#include "Graphics.h"

#include <Mouse.h>

#include "Keyboard.h"
#include "Engine.h"
#include "Logger.h"
#include "Gui.h"

Graphics::~Graphics()
{
    Gui::Shutdown();

    m_depthStencilBuffer->Release();
    m_depthStencilView->Release();
    m_depthStencilState->Release();
    m_deviceContext->Release();
    m_device->Release();
    m_swapChain->Release();
    m_renderTargetView->Release();
    m_rasterizerState->Release();
}

void Graphics::Initialize(HWND hWnd, int width, int height)
{
    OutputDebugStringW(L"Initializing graphics...\n");

    m_hwnd = hWnd;
    m_width = width;
    m_height = height;

    if (!InitializeDirectX()) exit(-1);

    if (!SetupShaders()) exit(-1);

    if (!SetupScene()) exit(-1);

    Gui::Setup(hWnd, m_device, m_deviceContext);
}

void Graphics::UpdateScene(float dt)
{
    auto kb = DirectX::Keyboard::Get().GetState();
    auto mouse = DirectX::Mouse::Get().GetState();
    m_tracker.Update(mouse);

    if (m_tracker.rightButton == Mouse::ButtonStateTracker::PRESSED)
    {
        DirectX::Mouse::Get().SetMode(Mouse::MODE_RELATIVE);
    }
    if (m_tracker.rightButton == Mouse::ButtonStateTracker::RELEASED)
    {
        DirectX::Mouse::Get().SetMode(Mouse::MODE_ABSOLUTE);
    }


    float speed = 0.1f;

    if (kb.LeftControl) speed *= 10;

    if (kb.A) m_camera.AdjustPosition(m_camera.GetLeftVector() * speed);
    if (kb.D) m_camera.AdjustPosition(m_camera.GetRightVector() * speed);
    if (kb.W) m_camera.AdjustPosition(m_camera.GetForwardVector() * speed);
    if (kb.S) m_camera.AdjustPosition(m_camera.GetBackwardVector() * speed);
    if (kb.Space) m_camera.AdjustPosition(0.f, speed, 0.f);
    if (kb.LeftShift) m_camera.AdjustPosition(0.f, -speed, 0.f);

    if (mouse.positionMode == Mouse::MODE_RELATIVE)
		m_camera.AdjustRotation((float)mouse.y * 0.005f, (float)mouse.x * 0.005f, 0.f);
}

void Graphics::RenderFrame()
{
    float clearColor[] = { 0.53f, 0.81f, 0.92f, 1.f };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    m_deviceContext->IASetInputLayout(m_sceneVertexShader.GetInputLayout());
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_deviceContext->RSSetState(m_rasterizerState);

    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
    m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
    m_deviceContext->VSSetShader(m_sceneVertexShader.GetShader(), NULL, 0);
    m_deviceContext->PSSetShader(m_scenePixelShader.GetShader(), NULL, 0);

    DirectX::XMMATRIX viewProjection = m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix();
    m_globalCBuffer.Data.vpMatrix = viewProjection;
    if (!m_globalCBuffer.ApplyChanges()) return;
    m_deviceContext->VSSetConstantBuffers(0, 1, m_globalCBuffer.GetBuffer());

    m_ambientLight.ApplyBuffer(0);
    m_pointLight.ApplyBuffer(1);
    m_spotLight.ApplyBuffer(2);

    m_gameObject.Render();

    Gui::Render();

    m_swapChain->Present(m_vsync, NULL);
}

bool Graphics::InitializeDirectX()
{
    LOG_INFO("Initializing DirectX...");
    HRESULT hr;

    DXGI_SWAP_CHAIN_DESC scd;
    scd.BufferDesc.Width = m_width;
    scd.BufferDesc.Height = m_height;
    scd.BufferDesc.RefreshRate.Numerator = 60;
    scd.BufferDesc.RefreshRate.Denominator = 1;
    scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scd.SampleDesc.Count = 1;
    scd.SampleDesc.Quality = 0;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 1;
    scd.OutputWindow = m_hwnd;
    scd.Windowed = true;
    scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scd.Flags = 0;

    hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, D3D11_CREATE_DEVICE_DEBUG,
        0, 0, D3D11_SDK_VERSION,
        &scd, &m_swapChain, &m_device, 0, &m_deviceContext);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create device and swapchain", hr);
        return false;
    }

    SetupD3D();

    D3D11_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create rasterizer state", hr);
        return false;
    }

    D3D11_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0.f;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerState);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create sampler state", hr);
        return false;
    }

    gfx2d.Init(m_device, m_deviceContext);
    gfx2d.SetViewport((float)m_width, (float)m_height, 0.f, 1.f);

    LOG_INFO("Graphics initialized.");

    return true;
}

void Graphics::SetupD3D()
{
    HRESULT hr;

    hr = m_swapChain->ResizeBuffers(1, m_width, m_height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to resize buffers", hr);
    }

    ID3D11Texture2D* backBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create render target view", hr);
    }
    backBuffer->Release();

    CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, m_width, m_height, 1, 1);
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = m_device->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create depth stencil buffer", hr);
    }

    hr = m_device->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create depth stencil view", hr);
    }

    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
    depthStencilStateDesc.DepthEnable = true;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

    hr = m_device->CreateDepthStencilState(&depthStencilStateDesc, &m_depthStencilState);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create depth stencil state", hr);
    }

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    CD3D11_VIEWPORT vp(0.f, 0.f, (float)m_width, (float)m_height);
    m_deviceContext->RSSetViewports(1, &vp);
}

void Graphics::Resize(int width, int height)
{
    LOG_INFO("Window resized...");

    m_width = width;
    m_height = height;

    LOG_INFO("Deleting scene...");
    DeleteScene();

    LOG_INFO("Shutting down ImGui...");
    Gui::Shutdown();

    m_deviceContext->OMSetRenderTargets(0, 0, 0);

    m_depthStencilState->Release();
    m_depthStencilView->Release();
    m_depthStencilBuffer->Release();
    m_renderTargetView->Release();
    
    m_deviceContext->Flush();

    SetupD3D();

    m_camera.SetProjectionValues(90.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 100000.0f);

    LOG_INFO("Intializing ImGui...");
    Gui::Setup(m_hwnd, m_device, m_deviceContext);


    LOG_INFO("Resize complete.");
}

bool Graphics::SetupShaders()
{
    D3D11_INPUT_ELEMENT_DESC vertexShaderInputLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    if (!m_sceneVertexShader.Init(m_device, L"../bin/vertexshader.cso", vertexShaderInputLayout, ARRAYSIZE(vertexShaderInputLayout)))
        return false;

    if (!m_scenePixelShader.Init(m_device, L"../bin/pixelshader.cso"))
        return false;
    
    return true;
}

void Graphics::DeleteScene()
{
}

bool Graphics::SetupScene()
{
    HRESULT hr;

    m_camera.SetPosition(0.f, 100.f, -2.f);
    m_camera.SetProjectionValues(90.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 100000.0f);

    if (!m_globalCBuffer.Init(m_device, m_deviceContext))
    {
        LOG_ERROR("Failed to initialize global constant buffer");
        return false;
    }

    m_ambientLight.Init(m_device, m_deviceContext);

    m_ambientLight.GetCBuffer()->Data.ambientLightColor.x = 1.f;
    m_ambientLight.GetCBuffer()->Data.ambientLightColor.y = 1.f;
    m_ambientLight.GetCBuffer()->Data.ambientLightColor.z = 1.f;
    m_ambientLight.GetCBuffer()->Data.ambientLightStrength = 1.f;

    m_pointLight.Init(m_device, m_deviceContext);

    m_pointLight.GetCBuffer()->Data.pointLightColor = DirectX::XMFLOAT3(1.f, 1.f, 1.f);
    m_pointLight.GetCBuffer()->Data.pointLightPosition = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
    m_pointLight.GetCBuffer()->Data.pointLightStrength = 1.f;
    m_pointLight.GetCBuffer()->Data.pointLightAttenuation = 1.f;
    m_pointLight.GetCBuffer()->Data.pointLightMaximumCalcDistance = 1000.f;

    m_spotLight.Init(m_device, m_deviceContext);

    m_spotLight.GetCBuffer()->Data.spotLightPosition = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
    m_spotLight.GetCBuffer()->Data.spotLightDirection = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
    m_spotLight.GetCBuffer()->Data.spotLightStrength = 10.f;
    m_spotLight.GetCBuffer()->Data.spotLightDistance = 10000.f;

    
    std::string textures[] = { "../bin/Textures/Palette.jpg" };

    if (!m_gameObject.Init("../bin/Lowpoly_City_Free_Pack.fbx", textures, 1, m_device, m_deviceContext))
    {
        LOG_ERROR("Failed to initialize model");
        return false;
    }

    return true;
}
