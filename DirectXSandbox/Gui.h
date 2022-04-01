#pragma once

#include "Graphics.h"

namespace Gui
{
    void Setup(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void Shutdown();
    void Render();

    enum MsgType { MSG_INFO, MSG_WARNING, MSG_ERROR };
    void AddLogLine(const std::string& line, MsgType msgType);

    void RenderSceneManager();
    void RenderObjectInspector();
    void RenderCreateObjectMenu();
    void RenderLoadFileDialog();
    void RenderSaveSceneMenu();
    void RenderLogWindow();
    void SetSelectedObject(const std::string& string, VisibleGameObject* pickedObj);
}
