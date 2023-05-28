#include "Scene.h"

namespace Lucid::Core
{

void
Scene::AddNode(const Node& node)
{
    mNodes.push_back(node);
}

void
Scene::AddCamera(const std::shared_ptr<Camera>& entity)
{
    mCamera = entity;
}

const std::vector<Node>&
Scene::GetNodes() const
{
    return mNodes;
}

const std::shared_ptr<Camera>&
Scene::GetCamera() const
{
    return mCamera;
}

} // namespace Lucid::Core
