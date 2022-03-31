#include "Mouse.h"

std::vector<std::function<void(float, float)>> rawMovementBindings;
float mouseX, mouseY;

void Mouse::AddRawMovementBinding(std::function<void(float, float)>& func)
{
    rawMovementBindings.push_back(func);
}

void Mouse::OnRawMove(float xdiff, float ydiff, float currentPosX, float currentPosY)
{
    mouseX = currentPosX;
    mouseY = currentPosY;
    
    for (int i = 0; i < rawMovementBindings.size(); ++i)
    {
        rawMovementBindings[i](xdiff, ydiff);
    }
}
