#pragma once

#include <filesystem>
#include <vector>

#include <Core/SceneNode.h>

namespace Lucid
{

class Files
{
public:
    static std::vector<char> LoadFile(const std::filesystem::path& path);
    static Core::TexturePtr LoadTexture(const std::filesystem::path& path);
    static Core::SceneNodePtr LoadModel(const std::filesystem::path& path);
};

} // namespace Lucid
