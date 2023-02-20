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

    void AddAsset(const Asset& entity);
    void AddCamera(const std::shared_ptr<Camera>& entity);

    const std::vector<Asset>& GetAssets() const;
    const std::shared_ptr<Camera>& GetCamera() const;

private:
    std::vector<Asset> mMeshes;
    std::shared_ptr<Camera> mCamera;
};

} // namespace Lucid::Core
