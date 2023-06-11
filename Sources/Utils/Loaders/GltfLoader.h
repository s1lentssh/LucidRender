#pragma once

#include <filesystem>

#include <tiny_gltf.h>

#include <Core/Types.h>

namespace Lucid::Loaders
{

class GltfLoader
{
public:
    static Core::Node Load(const std::filesystem::path& path);

private:
    struct BufferData
    {
        const std::uint8_t* data;
        std::size_t stride;
        std::size_t count;
        std::int32_t type;
    };
    static BufferData GetBufferData(const tinygltf::Model& gltf, std::int32_t meshId, const std::string& attribute);

    static std::shared_ptr<Core::Node> TraverseFn(const tinygltf::Model& gltf, const tinygltf::Node& node);
    static std::shared_ptr<Core::Mesh> MeshFn(const tinygltf::Model& gltf, std::int32_t meshId);
    static std::shared_ptr<Core::Texture> TextureFn(const tinygltf::Model& gltf, std::int32_t meshId);
    static glm::mat4 TransformFn(const tinygltf::Node& node);
};

} // namespace Lucid::Loaders
