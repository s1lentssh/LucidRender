#pragma once

#include <filesystem>

#include <Core/Types.h>

namespace Lucid::Loaders
{

class GltfLoader
{
public:
    static Core::Node Load(const std::filesystem::path& path);

private:
};

} // namespace Lucid::Loaders
