#include "Engine.h"

#include <Core/InputController.h>
#include <Utils/Files.h>
#include <Utils/Logger.hpp>
#include <Vulkan/VulkanRender.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Lucid::Core
{

Engine::Engine(const IWindow& window, API api)
{
    mScene = std::make_shared<Lucid::Core::Scene::Scene>();

    switch (api)
    {
    case API::Vulkan:
        mRender = std::make_unique<Lucid::Vulkan::VulkanRender>(window, *mScene.get());
        break;
    }
}

void
Engine::Update(float time)
{
    // Sync
    ProcessInput(time);
    ProcessDirtyNodes();

    // Render
    mRender->DrawFrame();
}

void
Engine::SetRootNode(const Scene::NodePtr& node)
{
    mScene->SetRootNode(node);

    // Set all nodes as dirty
    mScene->Traverse(
        [this](const Core::Scene::NodePtr& it) { mDirtyNodes.insert(it->GetId()); }, mScene->GetRootNode());
}

void
Engine::ProcessInput(float time)
{
    std::set<InputController::Key> pressedKeys = InputController::Instance().GetPressedKeys();

    if (pressedKeys.contains(InputController::Key::Up))
    {
        mScene->GetCamera()->Move(Scene::Camera::MoveDirection::Forward, time);
    }

    if (pressedKeys.contains(InputController::Key::Down))
    {
        mScene->GetCamera()->Move(Scene::Camera::MoveDirection::Backward, time);
    }

    if (pressedKeys.contains(InputController::Key::Left))
    {
        mScene->GetCamera()->Move(Scene::Camera::MoveDirection::Left, time);
    }

    if (pressedKeys.contains(InputController::Key::Right))
    {
        mScene->GetCamera()->Move(Scene::Camera::MoveDirection::Right, time);
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

void
Engine::ProcessDirtyNodes()
{
    for (const std::size_t id : mDirtyNodes)
    {
        const Core::Scene::NodePtr& node = mScene->GetNodeById(id);
        if (node->GetOptionalMesh().has_value())
        {
            mRender->AddNode(node);
        }

        if (node->GetOptionalCamera().has_value())
        {
            mScene->AddCamera(node);
        }
    }

    mDirtyNodes.clear();
}

bool
Engine::ShouldClose() const
{
    return mRender->ShouldClose();
}

} // namespace Lucid::Core
