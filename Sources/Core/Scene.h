#pragma once

#include <memory>
#include <vector>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <Core/Camera.h>
#include <Core/Entity.h>
#include <Core/Mesh.h>
#include <glm/glm.hpp>

namespace Lucid::Core
{

class Scene
{
public:
    Scene() = default;

    void AddNode(const Node& node);
    void AddCamera(const std::shared_ptr<Camera>& node);

    const std::vector<Node>& GetNodes() const;
    const std::shared_ptr<Camera>& GetCamera() const;

private:
    std::vector<Node> mNodes;
    std::shared_ptr<Camera> mCamera;
};

} // namespace Lucid::Core
