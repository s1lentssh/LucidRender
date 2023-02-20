#include "InputController.h"

#include <iostream>

namespace Lucid::Core
{

InputController&
InputController::Instance()
{
    static InputController instance;
    return instance;
}

void
InputController::MouseMoved(float x, float y)
{
    mMouseDelta = Vector2d<float> { x - mMouseLastPosition.x, mMouseLastPosition.y - y };
    mMouseLastPosition = Vector2d<float> { x, y };
    mMouseMovedThisFrame = true;
}

void
InputController::MouseScrolled(float x, float y)
{
    mScrollDelta = Vector2d<float> { x, y };
    mMouseScrolledThisFrame = true;
}

void
InputController::KeyReleased(Key key)
{
    mPressedKeys.erase(key);
}

void
InputController::KeyPressed(Key key)
{
    mPressedKeys.insert(key);
}

std::set<InputController::Key>
InputController::GetPressedKeys()
{
    return mPressedKeys;
}

Vector2d<float>
InputController::GetMouseDelta()
{
    if (!mMouseMovedThisFrame)
    {
        mMouseDelta = Vector2d<float> { 0, 0 };
    }

    mMouseMovedThisFrame = false;

    return mMouseDelta;
}

Vector2d<float>
InputController::GetScrollDelta()
{
    if (!mMouseScrolledThisFrame)
    {
        mScrollDelta = Vector2d<float> { 0, 0 };
    }

    mMouseScrolledThisFrame = false;

    return mScrollDelta;
}

} // namespace Lucid::Core