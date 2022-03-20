#pragma once

#include <Windows.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <Mouse.h>

#include "Camera.h"
#include "Camera3D.h"
#include "ConstantBuffer.h"
#include "Shaders.h"
#include "VisibleGameObject.h"

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 texCoord;
};

class Graphics
{
public:
    ~Graphics();
    void Initialize(HWND hWnd, int width, int height);
    void Resize(int width, int height);
    void UpdateScene(float dt);
    void RenderFrame();

private:
    HWND m_hwnd;
    int m_width;
    int m_height;
    bool m_vsync = false;

    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;

    ID3D11Texture2D* m_depthStencilBuffer = nullptr;
    ID3D11DepthStencilView* m_depthStencilView = nullptr;
    ID3D11DepthStencilState* m_depthStencilState = nullptr;

    ID3D11RasterizerState* m_rasterizerState = nullptr;

    // Scene
    VertexShader m_sceneVertexShader;
    PixelShader m_scenePixelShader;
    ConstantBuffer<GlobalCBuffer> m_globalCBuffer;
    ConstantBuffer<PSAmbientLightCBuffer> m_psAmbientLightCBuffer;
    ConstantBuffer<PSpointLightCBuffer> m_psPointLightCBuffer;
    ID3D11SamplerState* m_samplerState;
    VisibleGameObject m_gameObject;

    DirectX::Mouse::ButtonStateTracker m_tracker;
    
    Camera3D m_camera3d;
    Camera m_camera;
    
    bool InitializeDirectX();
    bool SetupShaders();
    void DeleteScene();
    bool SetupScene();
    
};
