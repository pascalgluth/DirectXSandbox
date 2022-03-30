#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <string>

#include <Keyboard.h>
#include <Mouse.h>

#include "Engine.h"
#include "ImGui/imgui_impl_sdl.h"

SDL_Window* window;
HWND hWnd;
std::string windowTitle = "DirectX Sandbox";
int width = 1920;
int height = 1080;

void ProcessWMMsg(UINT msg, LPARAM lParam, WPARAM wParam);

int main(int argv, char** args)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
    window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN);
    SDL_SetWindowResizable(window, SDL_TRUE);

    int realWidth, realHeight;
    SDL_GetWindowSize(window, &realWidth, &realHeight);

    SDL_SysWMinfo sysWMInfo;
    SDL_VERSION(&sysWMInfo.version);
    SDL_GetWindowWMInfo(window, &sysWMInfo);
    hWnd = sysWMInfo.info.win.window;

    Engine::Initialize(hWnd, window, realWidth, realHeight);

    
    bool run = true;
    while (run)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);

            /*if (event.type == SDL_SYSWMEVENT)
            {
                ProcessWMMsg(event.syswm.msg->msg.win.msg,
                             event.syswm.msg->msg.win.lParam,
                             event.syswm.msg->msg.win.wParam);
            }*/
            
            if (event.type == SDL_QUIT)
                run = false;

            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    Engine::QueueResize(event.window.data1, event.window.data2);
                }
            }
        }

        if (Engine::ResizeQueued())
        {
            Engine::WindowSize newsize = Engine::GetNewWindowSize();
            SDL_SetWindowSize(window, newsize.width, newsize.height);
            Engine::GFXResize();
            Engine::SetResizeComplete();
        }

        Engine::Update();
        Engine::Render();
    }

    Engine::Shutdown();

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void ProcessWMMsg(UINT msg, LPARAM lParam, WPARAM wParam)
{
    switch (msg)
    {
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
        DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
        DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
        break;

    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000)
        {
            
        }
        DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
        break;

    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        DirectX::Keyboard::ProcessMessage(msg, wParam, lParam);
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
        DirectX::Mouse::ProcessMessage(msg, wParam, lParam);
        break;
    default:
        break;
    }
}
