#include "Files.h"

#include <fstream>

#include <stb_image.h>

#include <Utils/Loaders/GltfLoader.h>
#include <Utils/Loaders/ObjLoader.h>
#include <Utils/Logger.hpp>

namespace Lucid
{

std::vector<char>
Files::LoadFile(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("Can't open file: " + path.string());
    }

    std::size_t fileSize = static_cast<std::size_t>(file.tellg());
    std::vector<char> pixels(fileSize);

    file.seekg(0);
    file.read(pixels.data(), static_cast<std::streamsize>(fileSize));
    file.close();

    return pixels;
}

Core::TexturePtr
Files::LoadTexture(const std::filesystem::path& path)
{
    if (!std::filesystem::exists(path))
    {
        throw std::runtime_error("Can't load texture");
    }

    int width, height, channels;
    stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if (pixels == nullptr)
    {
        throw std::runtime_error("Can't load texture, pixels == nullptr");
    }

    if (width == 0 || height == 0)
    {
        throw std::runtime_error("Can't load texture, width or height == 0");
    }

    std::size_t size = static_cast<std::size_t>(width * height * 4);
    std::vector<unsigned char> result(size);
    std::memcpy(result.data(), pixels, size);

    std::uint32_t mipLevels = static_cast<std::uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

    Core::Texture texture { { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) },
                            result,
                            mipLevels };

    return std::make_shared<Core::Texture>(texture);
}

Core::Scene::NodePtr
Files::LoadModel(const std::filesystem::path& path)
{
    LoggerInfo << "Loading model" << path.string().c_str();

    if (path.extension() == ".obj")
    {
        return Loaders::ObjLoader::Load(path);
    }
    else if (path.extension() == ".gltf" || path.extension() == ".glb")
    {
        return Loaders::GltfLoader::Load(path);
    }

    throw std::runtime_error("Can't determine model format");
}

} // namespace Lucid
