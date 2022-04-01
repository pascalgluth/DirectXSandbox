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
#include "RigidBodyObject.h"
#include "Shaders.h"
#include "SkyBox.h"
#include "VisibleGameObject.h"

namespace r3d = reactphysics3d;

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 texCoord;
};

class Graphics
{
public:
    friend class VisibleGameObject;
    friend class ObjectManager;
    
    ~Graphics();
    void Initialize(HWND hWnd, int width, int height);
    void Resize(int width, int height);
    void UpdateScene(float dt);
    void RenderFrame(class ObjectManager* pObjectManager);

    const XMFLOAT3& GetCamPos() { return m_camera.GetPositionFloat3(); }
    const XMFLOAT3& GetCamDirection() { return m_camera.GetForwardFloat3(); }
    bool* GetVSyncState() { return &m_vsync; }
    UINT GetResWidth() const { return m_width; }
    UINT GetResHeight() const { return m_height; }

    ID3D11Device* GetDevice() { return m_device; }
    ID3D11DeviceContext* GetDeviceContext() { return m_deviceContext; }

    ID3D11DepthStencilState* GetDefaultDepthStencil() { return m_depthStencilState; }
    void ClearDepthBuffer() { m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0); }
    
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
    PixelShader m_solidColorPS;
    ConstantBuffer<GlobalCBuffer> m_globalCBuffer;
    ID3D11SamplerState* m_samplerState;
    SkyBox m_skyBox;
    // Physics
    r3d::PhysicsCommon m_physicsCommon;
    r3d::PhysicsWorld* m_physicsWorld = nullptr;
    RigidBodyObject m_rigidBodyObject;
    // Scene collider
    RigidBody* m_worldRigidBody = nullptr;
    TriangleMesh* m_worldTriangleMesh = nullptr;
    ConcaveMeshShape* m_concaveMeshShape = nullptr;
    Collider* m_worldCollider = nullptr;
    BoxShape* m_boxCollider = nullptr;

    DirectX::Mouse::ButtonStateTracker m_tracker;
    
    Camera3D m_camera3d;
    Camera m_camera;
    
    bool InitializeDirectX();
    void SetupD3D();
    bool SetupShaders();
    void DeleteScene();
    bool SetupScene();
    
};
