#include "Camera.h"

#include <algorithm>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>

namespace Lucid::Core
{

Camera::Camera(const glm::mat4& transform)
    : Entity(transform)
{
}

void
Camera::Rotate(const Vector2d<float>& value)
{
    mYaw -= value.x * mCameraSensitivity;
    mPitch += value.y * mCameraSensitivity;
    mPitch = std::clamp(mPitch, -89.0f, 89.0f);

    mCameraFront.x = std::cos(glm::radians(mYaw)) * std::cos(glm::radians(mPitch));
    mCameraFront.y = std::sin(glm::radians(mYaw)) * std::cos(glm::radians(mPitch));
    mCameraFront.z = std::sin(glm::radians(mPitch));

    mCameraFront = glm::normalize(mCameraFront);

    mTransofrm = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
}

void
Camera::Move(MoveDirection direction, float deltaTime)
{
    switch (direction)
    {
    case MoveDirection::Forward:
        mCameraPos += mCameraFront * mCameraSpeed * deltaTime;
        break;

    case MoveDirection::Backward:
        mCameraPos -= mCameraFront * mCameraSpeed * deltaTime;
        break;

    case MoveDirection::Left:
        mCameraPos -= glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed * deltaTime;
        break;

    case MoveDirection::Right:
        mCameraPos += glm::normalize(glm::cross(mCameraFront, mCameraUp)) * mCameraSpeed * deltaTime;
        break;
    }

    mTransofrm = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
}

void
Camera::AdjustFieldOfView(const float value)
{
    mFov += value;
    mFov = std::clamp(mFov, 1.0f, 360.0f);
}

float
Camera::FieldOfView() const
{
    return mFov;
}

} // namespace Lucid::Core
