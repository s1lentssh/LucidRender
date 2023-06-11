#pragma once

#include <map>
#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Core/Camera.h>
#include <Core/SceneNode.h>
#include <glm/glm.hpp>

namespace Lucid::Core
{

class Scene
{
public:
    Scene() = default;

    void SetRootNode(const SceneNodePtr& node);
    void AddCamera(const std::shared_ptr<Camera>& node);
    void Traverse(const std::function<void(const SceneNodePtr&)>& fn, const SceneNodePtr& node) const;

    const SceneNodePtr& GetRootNode() const;
    SceneNodePtr GetNodeById(std::size_t id) const;
    const std::shared_ptr<Camera>& GetCamera() const;

private:
    SceneNodePtr mRootNode;
    std::map<std::size_t, SceneNodeWeakPtr> mNodeIndex;
    std::shared_ptr<Camera> mCamera;
};

} // namespace Lucid::Core
