#include "Engine.h"
#include <Core/InputController.h>
#include <Utils/Files.h>
#include <Utils/Logger.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <Vulkan/VulkanRender.h>

#ifdef _WIN32
#include <D3D12/D3D12Render.h>
#endif

namespace Lucid::Core
{

Engine::Engine(const IWindow& window, API api)
{
    mScene = std::make_shared<Lucid::Core::Scene>();
 
    auto camera = std::make_shared<Lucid::Core::Camera>(glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.2f), glm::vec3(0.0f, 0.0f, 1.0f)));
    mScene->AddCamera(camera);

    switch (api)
    {
#ifdef _WIN32
        case API::D3D12:
            mRender = std::make_unique<Lucid::D3D12::D3D12Render>(window, *mScene.get());
            break;
#endif
        case API::Vulkan:
            mRender = std::make_unique<Lucid::Vulkan::VulkanRender>(window, *mScene.get());
            break;
        default:
            throw std::runtime_error("Unsupported API");
    }
}

void Engine::Update(float time)
{
    // Sync
    ProcessInput(time);

    // Render
    mRender->DrawFrame();
}

void Engine::AddAsset(const Core::Asset& asset)
{
    mScene->AddAsset(asset);
    mRender->AddAsset(asset);
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
