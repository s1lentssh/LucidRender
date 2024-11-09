#pragma once

#include <functional>
#include <map>
#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Core/Camera.h>
#include <Core/SceneNode.h>
#include <glm/glm.hpp>

namespace Lucid::Core::Scene
{

class Scene
{
public:
    Scene() = default;

    void SetRootNode(const NodePtr& node);
    void AddCamera(const NodePtr& node);
    void Traverse(const std::function<void(const NodePtr&)>& fn, const NodePtr& node) const;

    const NodePtr& GetRootNode() const;
    NodePtr GetNodeById(std::size_t id) const;
    const std::shared_ptr<Camera>& GetCamera() const;

private:
    NodePtr mRootNode;
    std::map<std::size_t, NodeWeakPtr> mNodeIndex;
    std::vector<NodeWeakPtr> mCameras;
    std::shared_ptr<Camera> mActiveCamera;
};

} // namespace Lucid::Core::Scene
