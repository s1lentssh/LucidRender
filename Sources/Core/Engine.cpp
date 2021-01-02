#include "Engine.h"
#include <Core/InputController.h>
#include <Utils/Files.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Vulkan/VulkanRender.h>

namespace Lucid::Core
{

Engine::Engine(const IWindow& window)
{
    mScene = std::make_shared<Lucid::Core::Scene>();

    auto mesh = std::make_shared<Lucid::Core::Mesh>(Lucid::Files::LoadModel("Resources/Models/VikingRoom.obj"));
    mScene->AddMesh(mesh);
 
    auto camera = std::make_shared<Lucid::Core::Camera>(glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.2f), glm::vec3(0.0f, 0.0f, 1.0f)));
    mScene->AddCamera(camera);

    mRender = std::make_unique<Lucid::Vulkan::VulkanRender>(window, *mScene.get());
}

void Engine::Update(float time)
{
    // Sync
    ProcessInput(time);

    // Render
    mRender->DrawFrame();
}

void Engine::ProcessInput(float time)
{
    std::set<InputController::Key> pressedKeys = InputController::Instance().GetPressedKeys();

    if (pressedKeys.contains(InputController::Key::Up))
    {
        mScene->GetCamera()->Move(Camera::MoveDirection::Forward, time);
    }

    if (pressedKeys.contains(InputController::Key::Down))
    {
        mScene->GetCamera()->Move(Camera::MoveDirection::Backward, time);
    }

    if (pressedKeys.contains(InputController::Key::Left))
    {
        mScene->GetCamera()->Move(Camera::MoveDirection::Left, time);
    }

    if (pressedKeys.contains(InputController::Key::Right))
    {
        mScene->GetCamera()->Move(Camera::MoveDirection::Right, time);
    }

    Vector2d<float> mouseDelta = InputController::Instance().GetMouseDelta();
    if (!mouseDelta.IsZero())
    {
        mScene->GetCamera()->Rotate(mouseDelta);
    }

    Vector2d<float> scrollDelta = InputController::Instance().GetScrollDelta();
    if (!scrollDelta.IsZero())
    {
        mScene->GetCamera()->AdjustFieldOfView(scrollDelta.y);
    }
}

}
