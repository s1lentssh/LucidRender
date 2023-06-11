#include "Scene.h"

namespace Lucid::Core
{

void
Scene::SetRootNode(const SceneNodePtr& node)
{
    mRootNode = node;

    // Add all nodes to index
    Traverse([this](const Core::SceneNodePtr& it) { mNodeIndex[it->GetId()] = it; }, GetRootNode());
}

void
Scene::AddCamera(const std::shared_ptr<Camera>& entity)
{
    mCamera = entity;
}

void
Scene::Traverse(const std::function<void(const SceneNodePtr&)>& fn, const SceneNodePtr& node) const
{
    for (const SceneNodePtr& child : node->GetChildren())
    {
        fn(child);
        Traverse(fn, child);
    }
}

const SceneNodePtr&
Scene::GetRootNode() const
{
    return mRootNode;
}

SceneNodePtr
Scene::GetNodeById(std::size_t id) const
{
    return mNodeIndex.at(id).lock();
}

const std::shared_ptr<Camera>&
Scene::GetCamera() const
{
    return mCamera;
}

} // namespace Lucid::Core
