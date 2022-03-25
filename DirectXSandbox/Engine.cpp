#include "Engine.h"
#include <Keyboard.h>
#include <Mouse.h>
#include "GameplayStatics.h"

namespace Engine
{
    Graphics* gfx;
    HWND hWnd;
    int newWidth;
    int newHeight;
    bool resizeQueued = false;

    std::unique_ptr<DirectX::Keyboard> keyboard;
    std::unique_ptr<DirectX::Mouse> mouse;

    GameplayStatics::Sound* mySound;
    
    void Initialize(HWND _hWnd, int width, int height)
    {
        resizeQueued = false;
        hWnd = _hWnd;

        keyboard = std::make_unique<DirectX::Keyboard>();
        mouse = std::make_unique<DirectX::Mouse>();
        mouse->SetWindow(hWnd);

        gfx = new Graphics();
        gfx->Initialize(hWnd, width, height);

        GameplayStatics::Init();

        mySound = new GameplayStatics::Sound("../bin/dog_sound.wav");
        //GameplayStatics::PlaySound2D(mySound);
        //GameplayStatics::PlaySound3D(mySound, gfx->GetCamPos());
    }

    void QueueResize(int width, int height)
    {
        newWidth = width;
        newHeight = height;
        resizeQueued = true;
    }

    void Shutdown()
    {
        delete gfx;
    }

    void Update()
    {
        gfx->UpdateScene(0);
    }

    void Render()
    {
        gfx->RenderFrame();
    }

    bool ResizeQueued()
    {
        return resizeQueued;
    }

    WindowSize GetNewWindowSize()
    {
        WindowSize newSize;
        newSize.width = newWidth;
        newSize.height = newHeight;
        return newSize;
    }

    void GFXResize()
    {
        gfx->Resize(newWidth, newHeight);
    }

    void SetResizeComplete()
    {
        resizeQueued = false;
    }

    Graphics* GetGFX()
    {
        return gfx;
    }
}
