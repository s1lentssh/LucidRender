#include "GltfLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <Utils/Logger.hpp>

namespace Lucid::Loaders
{

Core::Node
GltfLoader::Load(const std::filesystem::path& path)
{
    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string error;
    std::string warn;

    if (!loader.LoadASCIIFromFile(&model, &error, &warn, path.string()))
    {
        throw std::runtime_error("Cant load model, error: " + error);
    }

    if (!error.empty())
    {
        throw std::runtime_error("Error in model loading: " + error);
    }

    if (!warn.empty())
    {
        throw std::runtime_error("Warn in model loading: " + warn);
    }

    if (model.scenes.size() != 1)
    {
        throw std::runtime_error("Gltf loader supports only one scene per file");
    }

    tinygltf::Scene scene = model.scenes.at(0);

    auto meshFn = [&model](std::int32_t meshId) -> std::shared_ptr<Core::Mesh>
    {
        if (meshId == -1)
        {
            return nullptr;
        }

        const tinygltf::Mesh& mesh = model.meshes.at(static_cast<std::size_t>(meshId));
        (void)mesh;
        return {};
    };

    std::function<std::shared_ptr<Core::Node>(const tinygltf::Node&)> traverseFn
        = [&model, &traverseFn, &meshFn](const tinygltf::Node& node)
    {
        std::shared_ptr<Core::Node> result = std::make_shared<Core::Node>();
        result->name = node.name;
        LoggerInfo << node.name << ": " << node.mesh;
        for (const auto childId : node.children)
        {
            const tinygltf::Node childNode = model.nodes.at(static_cast<std::size_t>(childId));
            std::shared_ptr<Core::Node> traversed = traverseFn(childNode);
            traversed->parent = result;
            traversed->mesh = meshFn(childNode.mesh);
            result->children.push_back(traversed);
        }
        return result;
    };

    Core::Node result;
    result.name = "/";

    for (const auto nodeId : scene.nodes)
    {
        const tinygltf::Node rootNode = model.nodes.at(static_cast<std::size_t>(nodeId));
        std::shared_ptr<Core::Node> traversed = traverseFn(rootNode);
        traversed->mesh = meshFn(rootNode.mesh);
        result.children.push_back(traversed);
    }

    LoggerInfo << result;

    /*for (const tinygltf::Mesh& gltfMesh : model.meshes)
    {
        std::string name = gltfMesh.name;

        auto getRawBuffer
            = [&model,
               &gltfMesh](const std::string& attribute) -> std::tuple<const std::uint8_t*, std::size_t, std::size_t>
        {
            std::size_t accessorId = 0;

            if (attribute == "INDEX")
            {
                accessorId = static_cast<std::size_t>(gltfMesh.primitives[0].indices);
            }
            else
            {
                accessorId = static_cast<std::size_t>(gltfMesh.primitives[0].attributes.at(attribute));
            }

            // Accessor
            tinygltf::Accessor& accessor = model.accessors[accessorId];
            std::uint32_t componentType = static_cast<std::uint32_t>(accessor.componentType);
            std::uint32_t type = static_cast<std::uint32_t>(accessor.type);
            std::size_t componentSize = static_cast<std::size_t>(tinygltf::GetComponentSizeInBytes(componentType));
            std::size_t componentCount = static_cast<std::size_t>(tinygltf::GetNumComponentsInType(type));
            std::size_t itemStride = componentSize * componentCount;
            std::size_t itemCount = accessor.count;

            if (attribute == "POSITION" && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
            {
                throw std::runtime_error(
                    "Loader supports only float positions, provided " + std::to_string(accessor.componentType));
            }

            if (attribute == "NORMAL" && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
            {
                throw std::runtime_error(
                    "Loader supports only float normals, provided " + std::to_string(accessor.componentType));
            }

            if (attribute == "INDEX" && accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            {
                throw std::runtime_error(
                    "Loader supports only unsigned int indices, provided " + std::to_string(accessor.componentType));
            }

            if (attribute == "TEXCOORD_0" && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
            {
                throw std::runtime_error(
                    "Loader supports only float UV's, provided " + std::to_string(accessor.componentType));
            }

            // Buffer view
            std::size_t bufferViewId = static_cast<std::size_t>(accessor.bufferView);
            tinygltf::BufferView& bufferView = model.bufferViews[bufferViewId];

            // Buffer
            std::size_t bufferId = static_cast<std::size_t>(bufferView.buffer);
            tinygltf::Buffer& buffer = model.buffers[bufferId];

            return { static_cast<const std::uint8_t*>(buffer.data.data() + accessor.byteOffset + bufferView.byteOffset),
                     itemStride,
                     itemCount };
        };

        auto [indexBuffer, indexStride, indexCount] = getRawBuffer("INDEX");
        auto [positionBuffer, positionStride, positionCount] = getRawBuffer("POSITION");
        auto [normalBuffer, normalStride, normalCount] = getRawBuffer("NORMAL");
        auto [uvBuffer, uvStride, uvCount] = getRawBuffer("TEXCOORD_0");

        if (!(positionCount == normalCount && normalCount == uvCount))
        {
            throw std::runtime_error("Wrong vertex info in file");
        }

        for (std::size_t i = 0; i < positionCount; i++)
        {
            Core::Vertex vertex;

            // Position
            const float* position = reinterpret_cast<const float*>(positionBuffer + (i * positionStride));
            vertex.position = { position[0], position[1], position[2] };

            // Normal
            const float* normal = reinterpret_cast<const float*>(normalBuffer + (i * normalStride));
            vertex.normal = { normal[0], normal[1], normal[2] };

            // UV
            const float* uv = reinterpret_cast<const float*>(uvBuffer + (i * uvStride));
            vertex.uv = { uv[0], uv[1] };

            // Index
            const std::uint32_t index = *reinterpret_cast<const uint32_t*>(indexBuffer + (i * indexStride));

            mesh.indices.push_back(index);
            mesh.vertices.push_back(vertex);
        }
    }*/

    return result;
}

} // namespace Lucid::Loaders
