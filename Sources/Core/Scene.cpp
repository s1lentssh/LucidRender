#include "Scene.h"

namespace Lucid::Core::Scene
{

void
Scene::SetRootNode(const NodePtr& node)
{
    mRootNode = node;

    // Add all nodes to index
    Traverse([this](const Core::Scene::NodePtr& it) { mNodeIndex[it->GetId()] = it; }, GetRootNode());
    mActiveCamera = std::make_shared<Lucid::Core::Scene::Camera>(glm::mat4{1.0f});
}

void
Scene::AddCamera(const NodePtr& node)
{
    mCameras.push_back(node);
    mActiveCamera = std::make_shared<Lucid::Core::Scene::Camera>(node->GetTransform());
}

void
Scene::Traverse(const std::function<void(const NodePtr&)>& fn, const NodePtr& node) const
{
    for (const NodePtr& child : node->GetChildren())
    {
        fn(child);
        Traverse(fn, child);
    }
}

const NodePtr&
Scene::GetRootNode() const
{
    return mRootNode;
}

NodePtr
Scene::GetNodeById(std::size_t id) const
{
    return mNodeIndex.at(id).lock();
}

const std::shared_ptr<Camera>&
Scene::GetCamera() const
{
    return mActiveCamera;
}

} // namespace Lucid::Core::Scene
