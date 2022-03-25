#pragma once

#include "Graphics.h"

namespace Engine
{
    struct WindowSize
    {
        int width;
        int height;
    };

    void Initialize(HWND _hWnd, int width, int height);
    void QueueResize(int width, int height);
    void Shutdown();
    void Update();
    void Render();
    bool ResizeQueued();
    WindowSize GetNewWindowSize();
    void GFXResize();
    void SetResizeComplete();
    Graphics* GetGFX();
}

