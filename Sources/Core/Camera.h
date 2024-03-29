#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Core/Types.h>
#include <glm/glm.hpp>

namespace Lucid::Core
{

class Camera
{
public:
    enum class MoveDirection
    {
        Forward,
        Backward,
        Left,
        Right
    };

    Camera() = default;
    Camera(const glm::mat4& transform);

    void Rotate(const Vector2d<float>& value);
    void Move(MoveDirection direction, float deltaTime);
    void AdjustFieldOfView(const float value);
    glm::mat4 Transform() const;
    float FieldOfView() const;

private:
    const glm::vec3 mCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    const float mCameraSpeed = 300.0f;
    const float mCameraSensitivity = 0.1f;

    glm::mat4 mTransform { 1.0f };
    glm::vec3 mCameraPos;
    glm::vec3 mCameraFront;
    float mYaw = 0.0;
    float mPitch = 0.0f;
    float mFov = 37.8f;
};

} // namespace Lucid::Core
