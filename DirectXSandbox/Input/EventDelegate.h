#pragma once
#include <functional>

#include "../Logger.h"

struct EventDelegate
{
    void Invoke()
    {
        if (func) func();
        else LOG_WARN("Event without binding invoked.");
    }
    
    void Bind(std::function<void()> _func)
    {
        func = _func;
    }

    std::function<void()> func;
};