#include "Camera.h"

#include <algorithm>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Lucid::Core
{

Camera::Camera(const glm::mat4& transform)
    : Entity(transform)
{
    glm::mat4 viewMatrix = glm::inverse(transform);

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(viewMatrix, scale, rotation, translation, skew, perspective);

    mCameraPos = translation;
    mCameraFront = -glm::normalize(glm::vec3(viewMatrix[2]));
    mYaw = glm::degrees(glm::roll(rotation)) + 90.0f;
    mPitch = -(90.0f - glm::degrees(glm::pitch(rotation)));
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
    mTransform = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
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

    mTransform = glm::lookAt(mCameraPos, mCameraPos + mCameraFront, mCameraUp);
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
