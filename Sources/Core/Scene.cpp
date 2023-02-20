#include "Scene.h"

namespace Lucid::Core
{

void
Scene::AddAsset(const Asset& entity)
{
    mMeshes.push_back(entity);
}

void
Scene::AddCamera(const std::shared_ptr<Camera>& entity)
{
    mCamera = entity;
}

const std::vector<Asset>&
Scene::GetAssets() const
{
    return mMeshes;
}

const std::shared_ptr<Camera>&
Scene::GetCamera() const
{
    return mCamera;
}

} // namespace Lucid::Core