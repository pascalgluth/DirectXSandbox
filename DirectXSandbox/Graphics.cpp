#include "Graphics.h"

#include <Mouse.h>
#include <fmt/format.h>

#include "Keyboard.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx11.h"
#include "Engine.h"
#include "Logger.h"

Graphics::~Graphics()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(m_device, m_deviceContext);
    ImGui::StyleColorsLight();
    ImGui::GetStyle().ScaleAllSizes(2);
    io.FontGlobalScale = 1.5f;
}

void Graphics::Resize(int width, int height)
{
    LOG_INFO("Window resized...");

    m_width = width;
    m_height = height;

    LOG_INFO("Deleting scene...");
    DeleteScene();

    LOG_INFO("Shutting down ImGui...");
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    m_deviceContext->OMSetRenderTargets(0, 0, 0);

    m_depthStencilState->Release();
    m_depthStencilView->Release();
    m_depthStencilBuffer->Release();
    m_renderTargetView->Release();
    
	m_deviceContext->Flush();

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

    m_camera.SetProjectionValues(90.0f, static_cast<float>(m_width) / static_cast<float>(m_height), 0.1f, 100000.0f);

    LOG_INFO("Intializing ImGui...");

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(m_hwnd);
    ImGui_ImplDX11_Init(m_device, m_deviceContext);
    ImGui::StyleColorsLight();
    ImGui::GetStyle().ScaleAllSizes(2);
    io.FontGlobalScale = 1.5f;

    LOG_INFO("Resize complete.");
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

    m_psAmbientLightCBuffer.ApplyChanges();
    m_deviceContext->PSSetConstantBuffers(0, 1, m_psAmbientLightCBuffer.GetBuffer());

    m_psPointLightCBuffer.ApplyChanges();
    m_deviceContext->PSSetConstantBuffers(1, 1, m_psPointLightCBuffer.GetBuffer());

    m_gameObject.Render();

#pragma region ImGui

    static bool showWindowControls = false;
    static bool showAmbientLight = false;
    static bool showPointLight = false;
    static bool showOverlay = true;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::BeginMainMenuBar();
    {
	    if (ImGui::BeginMenu("Window"))
	    {
            ImGui::Checkbox("Window Controls", &showWindowControls);
            ImGui::Checkbox("Ambient Light", &showAmbientLight);
            ImGui::Checkbox("Point Light", &showPointLight);
            ImGui::Checkbox("Overlay", &showOverlay);

            ImGui::EndMenu();
	    }

        if (ImGui::BeginMenu("Resolution"))
        {
            if (ImGui::MenuItem("900x600")) Engine::QueueResize(900, 600);
            if (ImGui::MenuItem("1920x1080")) Engine::QueueResize(1920, 1080);
            if (ImGui::MenuItem("2560x1440")) Engine::QueueResize(2560, 1440);
            if (ImGui::MenuItem("2560x1600")) Engine::QueueResize(2560, 1600);
            if (ImGui::MenuItem("3200x1800")) Engine::QueueResize(3200, 1800);
            if (ImGui::MenuItem("3840x2160")) Engine::QueueResize(3840, 2160);

            ImGui::EndMenu();
        }

        DirectX::XMFLOAT3 campos = m_camera.GetPositionFloat3();
        std::string posText = fmt::format("Position: X={:.2f} Y={:.2f} Z={:.2f}", campos.x, campos.y, campos.z);

        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize(posText.c_str()).x - 10.f);
        ImGui::Text(posText.c_str());

        ImGui::EndMainMenuBar();
    }


    if (showWindowControls)
    {
	    ImGui::Begin("Window Controls");
	    {
		    ImGui::Checkbox("V-Sync", &m_vsync);
	    }
	    ImGui::End();
    }

    if (showAmbientLight)
    {
        ImGui::Begin("Ambient Light");
        {
            ImGui::Text("Strength: ");
            ImGui::SameLine();
            ImGui::DragFloat("##ambientLightStrength", &m_psAmbientLightCBuffer.Data.ambientLightStrength, 0.01f, 0.f, 1.f, "%.2f");

            ImGui::Text("Color:    ");
            ImGui::SameLine();
            ImGui::DragFloat3("##ambientLightColor", &m_psAmbientLightCBuffer.Data.ambientLightColor.x, 0.01f, 0.f, 1.f, "%.2f");
        }
        ImGui::End();
    }

    if (showPointLight)
    {
	    ImGui::Begin("Point Light");
	    {
		    ImGui::Text("Strength:    ");
		    ImGui::SameLine();
		    ImGui::DragFloat("##pointLightStrength", &m_psPointLightCBuffer.Data.pointLightStrength, 0.01f, 0.f, 100.f, "%.2f");

		    ImGui::Text("Position:    ");
		    ImGui::SameLine();
		    ImGui::DragFloat3("##pointLightPosition", &m_psPointLightCBuffer.Data.pointLightPosition.x, 0.1f);

		    ImGui::Text("Attenuation: ");
		    ImGui::SameLine();
		    ImGui::DragFloat("##pointLightAttenuation", &m_psPointLightCBuffer.Data.pointLightAttenuation, 0.01f, 0.f, 10.f, "%.2f");

		    ImGui::Text("Max dist.:  ");
		    ImGui::SameLine();
		    ImGui::DragFloat("##pointLightMaxCalcDistance", &m_psPointLightCBuffer.Data.pointLightMaximumCalcDistance, 0.1f);
	    }
	    ImGui::End();
    }

    if (showOverlay)
    {
	    static int corner = 1;
	    static bool hm = true;
	    ImGuiIO& io = ImGui::GetIO();
	    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	    if (corner != -1)
	    {
		    const float PAD = 10.0f;
		    const ImGuiViewport* viewport = ImGui::GetMainViewport();
		    ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		    ImVec2 work_size = viewport->WorkSize;
		    ImVec2 window_pos, window_pos_pivot;
		    window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		    window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		    window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
		    window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
		    ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		    window_flags |= ImGuiWindowFlags_NoMove;
	    }
	    ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	    if (ImGui::Begin("##infoOverlay", &hm, window_flags))
	    {
		    ImGui::Text("Information");
		    ImGui::Separator();
		    if (ImGui::IsMousePosValid())
			    ImGui::Text("Mouse Position: (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
		    else
			    ImGui::Text("Mouse Position: <invalid>");

		    if (m_vsync)
			    ImGui::Text("FPS: %d (VSync ON)", (int)ImGui::GetIO().Framerate);
		    else
			    ImGui::Text("FPS: %d (VSync OFF)", (int)ImGui::GetIO().Framerate);

		    ImGui::Text("Frametime: %.3f ms", ImGui::GetIO().DeltaTime*1000.f);

		    ImGui::Text("Resolution: %dx%d", m_width, m_height);
        
	    }
	    ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

#pragma endregion

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

    ID3D11Texture2D* backBuffer;
    m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
    hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_renderTargetView);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create render target view", hr);
        return false;
    }
    backBuffer->Release();

    CD3D11_VIEWPORT vp(0.f, 0.f, (float)m_width, (float)m_height);
    m_deviceContext->RSSetViewports(1, &vp);

    CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, m_width, m_height, 1, 1);
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    hr = m_device->CreateTexture2D(&depthStencilDesc, 0, &m_depthStencilBuffer);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create depth stencil buffer", hr);
        return false;
    }

    hr = m_device->CreateDepthStencilView(m_depthStencilBuffer, 0, &m_depthStencilView);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create depth stencil view", hr);
        return false;
    }

    D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc{};
    depthStencilStateDesc.DepthEnable = true;
    depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;

    hr = m_device->CreateDepthStencilState(&depthStencilStateDesc, &m_depthStencilState);
    if (FAILED(hr))
    {
        LOG_ERROR_HR("Failed to create depth stencil state", hr);
        return false;
    }

    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

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

    LOG_INFO("Graphics initialized.");

    return true;
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

    if (!m_psAmbientLightCBuffer.Init(m_device, m_deviceContext))
    {
        LOG_ERROR("Failed to initialize light buffer");
        return false;
    }

    m_psAmbientLightCBuffer.Data.ambientLightColor.x = 1.f;
    m_psAmbientLightCBuffer.Data.ambientLightColor.y = 1.f;
    m_psAmbientLightCBuffer.Data.ambientLightColor.z = 1.f;
    m_psAmbientLightCBuffer.Data.ambientLightStrength = 1.f;

    if (!m_psPointLightCBuffer.Init(m_device, m_deviceContext))
    {
        LOG_ERROR("Failed to initialize point light buffer");
        return false;
    }

    m_psPointLightCBuffer.Data.pointLightColor = DirectX::XMFLOAT3(1.f, 1.f, 1.f);
    m_psPointLightCBuffer.Data.pointLightPosition = DirectX::XMFLOAT3(0.f, 0.f, 0.f);
    m_psPointLightCBuffer.Data.pointLightStrength = 1.f;
    m_psPointLightCBuffer.Data.pointLightAttenuation = 1.f;
    m_psPointLightCBuffer.Data.pointLightMaximumCalcDistance = 1000.f;

    std::string textures[] = { "../bin/Textures/Palette.jpg" };

    if (!m_gameObject.Init("../bin/Lowpoly_City_Free_Pack.fbx", textures, 1, m_device, m_deviceContext))
    {
        LOG_ERROR("Failed to initialize model");
        return false;
    }

    return true;
}
