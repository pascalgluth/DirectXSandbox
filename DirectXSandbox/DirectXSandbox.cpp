// DirectXSandbox.cpp : Definiert den Einstiegspunkt für die Anwendung.
//

#include <Windows.h>
#include <string>
#include <Keyboard.h>
#include <Mouse.h>

#include "Engine.h"

HINSTANCE hInst;
HWND hWnd;
std::wstring wideTitle = L"DirectX Sandbox";
std::wstring wideWindowClass = L"MyWindowClass";
int width = 1920;
int height = 1080;

void RegisterWindowClass(HINSTANCE hInstance);
BOOL InitWindow(HINSTANCE, int);
void ResizeWindow();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool ProcessMessages();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_ LPWSTR    lpCmdLine,
                      _In_ int       nCmdShow)
{
    hInst = hInstance;

    RegisterWindowClass(hInstance);

    if (!InitWindow(hInstance, nCmdShow))
    {
        return FALSE;
    }

    Engine::Initialize(hWnd, width, height);

    while (ProcessMessages())
    {
        if (Engine::ResizeQueued())
        {
            ResizeWindow();
        }
        Engine::Update();
        Engine::Render();
    }

    Engine::Shutdown();

    return 0;
}

bool ProcessMessages()
{
    MSG msg{};

    while (PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    if (msg.message == WM_NULL)
    {
        if (!IsWindow(hWnd))
        {
            hWnd = NULL;
            UnregisterClass(wideWindowClass.c_str(), hInst);
            return false;
        }
    }

    return true;
}

void RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEX wc;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hIconSm = NULL;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL; 
    wc.lpszClassName = wideWindowClass.c_str();
    wc.cbSize = sizeof(WNDCLASSEX);
    
    RegisterClassEx(&wc);
}

BOOL InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance;

    int screenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
    int screenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

    RECT wr;
    wr.left = screenCenterX;
    wr.top = screenCenterY;
    wr.right = wr.left + width;
    wr.bottom = wr.top + height;
    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

    hWnd = CreateWindowEx(0, wideWindowClass.c_str(), wideTitle.c_str(),
                                WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
                                wr.left, wr.top,
                                wr.right - wr.left, wr.bottom - wr.top,
                                NULL, NULL, hInst, NULL);

    if (!hWnd)
    {
      return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    SetForegroundWindow(hWnd);
    UpdateWindow(hWnd);

    return TRUE;
}

void ResizeWindow()
{
    Engine::WindowSize newSize = Engine::GetNewWindowSize();
    width = newSize.width;
    height = newSize.height;

    int screenCenterX = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
    int screenCenterY = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

    RECT wr;
    wr.left = screenCenterX;
    wr.top = screenCenterY;
    wr.right = wr.left + width;
    wr.bottom = wr.top + height;
    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

    SetWindowPos(hWnd, 0, wr.left, wr.top, wr.right - wr.left, wr.bottom - wr.top, SWP_SHOWWINDOW | SWP_FRAMECHANGED);
    Engine::GFXResize();

    Engine::SetResizeComplete();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam)) return true;

    static bool isResizing = false;
    static bool isMinimized = false;

    switch (message)
    {
    case WM_DESTROY:
        DestroyWindow(hWnd);
        break;
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
	    DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        DirectX::Mouse::ProcessMessage(message, wParam, lParam);
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            
        }
        DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
	    DirectX::Keyboard::ProcessMessage(message, wParam, lParam);
        break;
    case WM_INPUT:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        DirectX::Mouse::ProcessMessage(message, wParam, lParam);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}