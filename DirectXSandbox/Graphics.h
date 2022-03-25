#pragma once

#include <Windows.h>
#include <dxgi.h>
#include <DirectXMath.h>
#include <Mouse.h>
#include <Audio.h>

#include "Camera.h"
#include "Camera3D.h"
#include "ConstantBuffer.h"
#include "Graphics2D.h"
#include "Light.h"
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

    const XMFLOAT3& GetCamPos() { return m_camera.GetPositionFloat3(); }
    const XMFLOAT3& GetCamDirection() { return m_camera.GetForwardFloat3(); }
    bool* GetVSyncState() { return &m_vsync; }
    UINT GetResWidth() const { return m_width; }
    UINT GetResHeight() const { return m_height; }

    Light<LightCBuffer::PSAmbientLight>* GetAmbientLight() { return &m_ambientLight; }
    Light<LightCBuffer::PSPointLight>* GetPointLight() { return &m_pointLight; }
    Light<LightCBuffer::PSSpotLight>* GetSpotLight() { return &m_spotLight; }
    
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
    ID3D11DepthStencilState* m_depthStencilState_drawMask = nullptr;

    ID3D11RasterizerState* m_rasterizerState = nullptr;

    Graphics2D gfx2d;

    // Scene
    VertexShader m_sceneVertexShader;
    PixelShader m_scenePixelShader;
    ConstantBuffer<GlobalCBuffer> m_globalCBuffer;
    Light<LightCBuffer::PSAmbientLight> m_ambientLight;
    Light<LightCBuffer::PSPointLight> m_pointLight;
    Light<LightCBuffer::PSSpotLight> m_spotLight;
    ID3D11SamplerState* m_samplerState;
    VisibleGameObject m_gameObject;

    DirectX::Mouse::ButtonStateTracker m_tracker;
    
    Camera3D m_camera3d;
    Camera m_camera;
    
    bool InitializeDirectX();
    void SetupD3D();
    bool SetupShaders();
    void DeleteScene();
    bool SetupScene();
    
};
