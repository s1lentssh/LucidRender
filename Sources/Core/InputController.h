#pragma once

#include <memory>
#include <set>

#include <Core/Camera.h>
#include <Core/Interfaces.h>

namespace Lucid::Core
{

class InputController
{
public:
    enum class Key
    {
        Left,
        Right,
        Up,
        Down
    };

    static InputController& Instance();

    void MouseMoved(float x, float y);
    void MouseScrolled(float x, float y);
    void KeyPressed(Key key);
    void KeyReleased(Key key);
    void MousePressed();
    void MouseReleased();
    void SetMouseDisabled(bool value);

    std::set<Key> GetPressedKeys();
    Vector2d<float> GetMouseDelta();
    Vector2d<float> GetScrollDelta();

private:
    InputController() = default;
    InputController(const InputController&) = delete;
    InputController(InputController&&) = delete;
    void operator=(const InputController&) = delete;
    void operator=(const InputController&&) = delete;

    std::set<Key> mPressedKeys;

    Vector2d<float> mMouseLastPosition;
    Vector2d<float> mMouseDelta;
    bool mMouseJustPressed = true;

    Vector2d<float> mScrollDelta;
    bool mMouseScrolledThisFrame = false;
    bool mMouseDisabled = false;
};

} // namespace Lucid::Core
