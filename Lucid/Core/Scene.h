#pragma once

#include <vector>
#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <Core/Entity.h>
#include <Core/Mesh.h>
#include <Core/Camera.h>

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

}