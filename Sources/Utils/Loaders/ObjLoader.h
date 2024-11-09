#pragma once

#include <filesystem>

#include <Core/SceneNode.h>

namespace Lucid::Loaders
{

class ObjLoader
{
public:
    static Core::Scene::NodePtr Load(const std::filesystem::path& path);

private:
};

} // namespace Lucid::Loaders
