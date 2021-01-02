#include "Scene.h"
#include "Scene.h"
#include "Scene.h"

namespace Lucid::Core
{

void Scene::AddMesh(const std::shared_ptr<Mesh>& entity)
{
	mMeshes.push_back(entity);
}

void Scene::AddCamera(const std::shared_ptr<Camera>& entity)
{
	mCamera = entity;
}

const std::shared_ptr<Mesh>& Scene::GetMeshDebug() const
{
	return mMeshes.at(0);
}

const std::shared_ptr<Camera>& Scene::GetCamera() const
{
	return mCamera;
}

}