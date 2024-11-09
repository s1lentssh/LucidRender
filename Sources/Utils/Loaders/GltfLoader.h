#pragma once

#include <filesystem>

#include <tiny_gltf.h>

#include <Core/SceneNode.h>
#include <Core/Types.h>

namespace Lucid::Loaders
{

class GltfLoader
{
public:
    static Core::Scene::NodePtr Load(const std::filesystem::path& path);

private:
    struct BufferData
    {
        const std::uint8_t* data;
        std::size_t stride;
        std::size_t count;
        std::int32_t type;
    };
    static std::optional<BufferData>
    GetBufferData(const tinygltf::Model& gltf, std::int32_t meshId, const std::string& attribute);

    static Core::Scene::NodePtr
    TraverseFn(const tinygltf::Model& gltf, const tinygltf::Node& node, Core::Scene::NodePtr parent);
    static Core::MeshPtr MeshFn(const tinygltf::Model& gltf, std::int32_t meshId);
    static Core::CameraPtr CameraFn(const tinygltf::Model& gltf, std::int32_t cameraId);
    static Core::TexturePtr AlbedoFn(const tinygltf::Model& gltf, std::int32_t meshId);
    static Core::TexturePtr MetallicRoughnessFn(const tinygltf::Model& gltf, std::int32_t meshId);
    static Core::TexturePtr NormalFn(const tinygltf::Model& gltf, std::int32_t meshId);
    static Core::MaterialPtr MaterialFn(const tinygltf::Model& gltf, std::int32_t meshId);
    static glm::mat4 TransformFn(const tinygltf::Node& node);
};

} // namespace Lucid::Loaders
