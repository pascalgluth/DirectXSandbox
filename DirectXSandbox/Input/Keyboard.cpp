#include "Keyboard.h"

#include <vector>

#include "EventDelegate.h"

struct KeyBinding
{
    std::string inputName;
    KeyEventType eventType;
    EventDelegate func;
};

std::vector<KeyBinding> keyBindings;
std::vector<KeyBinding> actionBindings;
std::vector<std::function<void(const char*, KeyEventType, bool)>> generalBindings;
std::vector<const char*> keysDown;

void Keyboard::Bind(std::function<void(const char*, KeyEventType, bool)>& func)
{
    generalBindings.push_back(func);
}

void Keyboard::AddInputBinding(const std::string& inputName, KeyEventType eventType, std::function<void()>& func)
{
    KeyBinding binding{};
    binding.inputName = inputName;
    binding.eventType = eventType;
    binding.func.Bind(func);
    keyBindings.push_back(binding);
}

void Keyboard::AddActionBinding(const std::string& inputName, KeyEventType eventType, std::function<void()>& func)
{
    KeyBinding binding{};
    binding.inputName = inputName;
    binding.eventType = eventType;
    binding.func.Bind(func);
    actionBindings.push_back(binding);
}

bool Keyboard::IsKeyDown(const std::string& inputName)
{
    for (int i = 0; i < keysDown.size(); ++i)
    {
        if (strcmp(keysDown[i], inputName.c_str()) == 0) return true;
    }

    return false;
}

void Keyboard::OnKeyDown(const char* inputName, bool repeat)
{
    if (!IsKeyDown(inputName))
        keysDown.push_back(inputName);
    
    for (int i = 0; i < generalBindings.size(); ++i)
    {
        generalBindings[i](inputName, KE_PRESSED, repeat);
    }

    for (int i = 0; i < actionBindings.size(); ++i)
    {
        if (actionBindings[i].eventType == KE_PRESSED && strcmp(actionBindings[i].inputName.c_str(), inputName) == 0)
        {
            actionBindings[i].func.Invoke();
        }
    }
    
    if (!repeat)
    {
        for (int i = 0; i < keyBindings.size(); ++i)
        {
            if (keyBindings[i].eventType == KE_PRESSED && strcmp(keyBindings[i].inputName.c_str(), inputName) == 0)
            {
                keyBindings[i].func.Invoke();
            }
        }
    }
}

void Keyboard::OnKeyUp(const char* inputName, bool repeat)
{
    for (int i = 0; i < keysDown.size(); ++i)
    {
        if (strcmp(keysDown[i], inputName) == 0)
            keysDown.erase(keysDown.begin() + i);
    }
    
    for (int i = 0; i < generalBindings.size(); ++i)
    {
        generalBindings[i](inputName, KE_RELEASED, repeat);
    }

    for (int i = 0; i < actionBindings.size(); ++i)
    {
        if (actionBindings[i].eventType == KE_RELEASED && strcmp(actionBindings[i].inputName.c_str(), inputName) == 0)
        {
            actionBindings[i].func.Invoke();
        }
    }
    
    if (!repeat)
    {
        for (int i = 0; i < keyBindings.size(); ++i)
        {
            if (keyBindings[i].eventType == KE_RELEASED && strcmp(keyBindings[i].inputName.c_str(), inputName) == 0)
            {
                keyBindings[i].func.Invoke();
            }
        }
    }
}
