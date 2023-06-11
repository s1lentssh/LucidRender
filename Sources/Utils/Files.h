#pragma once

#include <filesystem>
#include <vector>

#include <Core/SceneNode.h>

#undef LoadImage

namespace Lucid
{

class Files
{
public:
    static std::vector<char> LoadFile(const std::filesystem::path& path);
    static Core::TexturePtr LoadImage(const std::filesystem::path& path);
    static Core::SceneNodePtr LoadModel(const std::filesystem::path& path);
};

} // namespace Lucid
