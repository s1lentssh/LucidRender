#include "Scene.h"

namespace Lucid::Core
{

void
Scene::AddNode(const Node& node)
{
    mRootNode = node;
}

void
Scene::AddCamera(const std::shared_ptr<Camera>& entity)
{
    mCamera = entity;
}

void
Scene::Traverse(const std::function<void(Node&)>& fn, const Node& node) const
{
    for (const std::shared_ptr<Node>& child : node.children)
    {
        fn(*child.get());
        Traverse(fn, *child.get());
    }
}

const Node&
Scene::GetRootNode() const
{
    return mRootNode;
}

const std::shared_ptr<Camera>&
Scene::GetCamera() const
{
    return mCamera;
}

} // namespace Lucid::Core
