#pragma once

#include <functional>

namespace Mouse
{
    void AddRawMovementBinding(std::function<void(float, float)>& func);

    void OnRawMove(float xdiff, float ydiff, float currentPosX, float currentPosY);
}
