#pragma once

#include <filesystem>
#include <vector>

#include <Core/Mesh.h>
#include <Core/Texture.h>

#undef LoadImage

namespace Lucid
{

class Files
{
public:
    static std::vector<char> LoadFile(const std::filesystem::path& path);
    static Core::Texture LoadImage(const std::filesystem::path& path);
    static Core::Mesh LoadModel(const std::filesystem::path& path);
};

} // namespace Lucid
