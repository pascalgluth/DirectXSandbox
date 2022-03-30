#include "Graphics.h"

#include <Mouse.h>

#include "Keyboard.h"
#include "Logger.h"
#include "Gui.h"
#include "ObjectManager.h"
#include "DirectoryHelperMacros.h"

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

void Graphics::Initialize(HWND hWnd, SDL_Window* sdlWindow, int width, int height)
{
    LOG_INFO("Initializing graphics...");

    m_hwnd = hWnd;
    m_sdlWindow = sdlWindow;
    m_width = width;
    m_height = height;

    if (!InitializeDirectX()) exit(-1);

    if (!SetupShaders()) exit(-1);

    if (!SetupScene()) exit(-1);

    Gui::Setup(m_sdlWindow, m_device, m_deviceContext);
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

    if (kb.C) m_rigidBodyObject.SetPosition(m_camera.GetPositionFloat3());

    if (mouse.positionMode == Mouse::MODE_RELATIVE)
		m_camera.AdjustRotation((float)mouse.y * 0.005f, (float)mouse.x * 0.005f, 0.f);

    m_skyBox.SetPosition(m_camera.GetPositionFloat3());

    m_physicsWorld->update(ImGui::GetIO().DeltaTime);
    m_rigidBodyObject.Update(dt);

    if (m_physicsWorld->testOverlap(m_worldCollider->getBody(), m_rigidBodyObject.GetCollider()->getBody()))
    {
        LOG_INFO("Hello we collide");
    }
}

void Graphics::RenderFrame(ObjectManager* pObjectManager)
{
    float clearColor[] = { 0.f, 0.f, 0.f, 0.f };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
    m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);

    DirectX::XMMATRIX viewProjection = m_camera.GetViewMatrix() * m_camera.GetProjectionMatrix();
    m_globalCBuffer.Data.vpMatrix = viewProjection;
    m_globalCBuffer.Data.eyePos = m_camera.GetPositionFloat3();
    if (!m_globalCBuffer.ApplyChanges()) return;
    m_deviceContext->VSSetConstantBuffers(0, 1, m_globalCBuffer.GetBuffer());

    m_skyBox.Render();

    m_deviceContext->IASetInputLayout(m_sceneVertexShader.GetInputLayout());
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_deviceContext->RSSetState(m_rasterizerState);

    m_deviceContext->VSSetShader(m_sceneVertexShader.GetShader(), NULL, 0);
    m_deviceContext->PSSetShader(m_scenePixelShader.GetShader(), NULL, 0);

    Light::RenderLights(0);

    pObjectManager->Render();

    m_rigidBodyObject.Render();

    //m_ambientLight.Render();
    //m_spotLight.Render();
    //m_pointLight.Render();


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
    scd.SampleDesc.Count = 4;
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
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
    samplerDesc.MipLODBias = 0.f;
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
    depthStencilDesc.SampleDesc.Count = 4;
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

    m_camera.SetProjectionValues(90.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 1000000.f);

    LOG_INFO("Intializing ImGui...");
    Gui::Setup(m_sdlWindow, m_device, m_deviceContext);


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

    m_physicsWorld = m_physicsCommon.createPhysicsWorld();

    m_camera.SetPosition(0.f, 100.f, -2.f);
    m_camera.SetProjectionValues(90.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 100000.0f);

    if (!m_globalCBuffer.Init(m_device, m_deviceContext))
    {
        LOG_ERROR("Failed to initialize global constant buffer");
        return false;
    }

    Light::Init(m_device, m_deviceContext);
    
    /*std::vector<float> vertices;
    std::vector<int> indices;

    std::vector<Mesh*>* meshes = m_gameObject.GetMeshes();
    
    for (int j = 0; j < meshes->size(); ++j)
    {
        for (int i = 0; i < (*meshes)[j]->Vertices.size(); ++i)
        {
            vertices.push_back((*meshes)[j]->Vertices[i].pos.x);
            vertices.push_back((*meshes)[j]->Vertices[i].pos.y);
            vertices.push_back((*meshes)[j]->Vertices[i].pos.z);
        }

        for (int i = 0; i < (*meshes)[j]->Indices.size(); ++i)
        {
            indices.push_back((*meshes)[j]->Indices[i]);
        }
    }

    const int nbVertices = vertices.size(); 
    const int nbTriangles = indices.size()/3;
    
    TriangleVertexArray* triangleArray = new TriangleVertexArray(nbVertices, vertices.data(), 3 * sizeof(float), nbTriangles, indices.data(), 3 * sizeof(int),
        TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE, TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

    m_worldTriangleMesh = m_physicsCommon.createTriangleMesh();
    m_worldTriangleMesh->addSubpart(triangleArray);

    m_concaveMeshShape = m_physicsCommon.createConcaveMeshShape(m_worldTriangleMesh);*/

    Vector3 position(0.f, 0.f, 0.f);
    Quaternion orientation = Quaternion::identity();
    Transform transform(position, orientation);

    m_worldRigidBody = m_physicsWorld->createRigidBody(transform);
    m_worldRigidBody->enableGravity(false);

    m_boxCollider = m_physicsCommon.createBoxShape({1000.f, 5.f, 1000.f});
    m_worldCollider = m_worldRigidBody->addCollider(m_boxCollider, transform);
    m_worldRigidBody->setType(BodyType::KINEMATIC);
    m_worldCollider->setIsTrigger(false);
    

    if (!m_skyBox.Init(m_device, m_deviceContext))
    {
        LOG_ERROR("Failed to init skybox");
        return false;
    }

    // -100 -15 500

    m_rigidBodyObject.Init(FILE_MODEL("bottle.obj"), m_device, m_deviceContext);
    m_rigidBodyObject.SetPosition(XMFLOAT3(-100.f, -15.f, 500.f));
    m_rigidBodyObject.SetupPhysics(&m_physicsCommon, m_physicsWorld);
    
    return true;
}
