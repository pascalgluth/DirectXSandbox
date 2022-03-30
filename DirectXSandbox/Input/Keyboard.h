#pragma once

#include <functional>
#include <string>

enum KeyEventType { KE_PRESSED, KE_RELEASED };

namespace Keyboard
{
    void Bind(std::function<void(const char*, KeyEventType, bool)>& func);
    void AddInputBinding(const std::string& inputName, KeyEventType eventType, std::function<void()>& func);
    void AddActionBinding(const std::string& inputName, KeyEventType eventType, std::function<void()>& func);

    bool IsKeyDown(const std::string& inputName);

    void OnKeyDown(const char* inputName, bool repeat);
    void OnKeyUp(const char* inputName, bool repeat);
}