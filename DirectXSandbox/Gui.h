#pragma once

#include "Graphics.h"

namespace Gui
{
    void Setup(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void Shutdown();
    void Render();

    void RenderSceneManager();
    void RenderObjectInspector();
    void RenderCreateObjectMenu();
    void RenderLoadFileDialog();
    void RenderSaveSceneMenu();
}
