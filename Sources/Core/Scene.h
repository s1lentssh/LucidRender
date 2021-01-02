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

	void AddMesh(const std::shared_ptr<Mesh>& entity);
	void AddCamera(const std::shared_ptr<Camera>& entity);

	const std::shared_ptr<Mesh>& GetMeshDebug() const;
	const std::shared_ptr<Camera>& GetCamera() const;

private:
	std::vector<std::shared_ptr<Mesh>> mMeshes;
	std::shared_ptr<Camera> mCamera;
};

}