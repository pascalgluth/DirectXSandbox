#pragma once

#include "Graphics.h"

namespace Engine
{
    struct WindowSize
    {
        int width;
        int height;

        WindowSize()
            : width(0), height(0)
        {}
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
    ObjectManager* GetObjectManager();
    void LoadScene(const std::string& file);
    void SaveScene(const std::string& file);
}

