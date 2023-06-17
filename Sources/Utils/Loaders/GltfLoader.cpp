#include "GltfLoader.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include <Utils/Logger.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Lucid::Loaders
{

Core::SceneNodePtr
GltfLoader::Load(const std::filesystem::path& path)
{
    tinygltf::Model gltf;
    tinygltf::TinyGLTF loader;
    std::string error;
    std::string warn;

    if (path.extension() == ".gltf" && !loader.LoadASCIIFromFile(&gltf, &error, &warn, path.string()))
    {
        throw std::runtime_error("Cant load gltf, error: " + error);
    }

    if (path.extension() == ".glb" && !loader.LoadBinaryFromFile(&gltf, &error, &warn, path.string()))
    {
        throw std::runtime_error("Cant load gltf, error: " + error);
    }

    if (!error.empty())
    {
        throw std::runtime_error("Error in gltf loading: " + error);
    }

    if (!warn.empty())
    {
        throw std::runtime_error("Warn in gltf loading: " + warn);
    }

    if (gltf.scenes.size() != 1)
    {
        throw std::runtime_error("Gltf loader supports only one scene per file");
    }

    tinygltf::Scene scene = gltf.scenes.at(0);
    Core::SceneNodePtr result = Core::SceneNode::Create("Root", nullptr);

    for (const auto nodeId : scene.nodes)
    {
        const tinygltf::Node rootNode = gltf.nodes.at(static_cast<std::size_t>(nodeId));
        Core::SceneNodePtr traversed = GltfLoader::TraverseFn(gltf, rootNode, nullptr);
        result->AddChildren(traversed);
    }

    return result;
}

std::optional<GltfLoader::BufferData>
GltfLoader::GetBufferData(const tinygltf::Model& gltf, std::int32_t meshId, const std::string& attribute)
{
    const tinygltf::Mesh& mesh = gltf.meshes.at(static_cast<std::size_t>(meshId));

    std::size_t accessorId = 0;

    if (attribute == "INDEX")
    {
        accessorId = static_cast<std::size_t>(mesh.primitives[0].indices);
    }
    else
    {
        try
        {
            accessorId = static_cast<std::size_t>(mesh.primitives[0].attributes.at(attribute));
        }
        catch (const std::exception& ex)
        {
            LoggerError << "No attribute " << attribute << ": " << ex.what();
            return std::nullopt;
        }
    }

    // Accessor
    const tinygltf::Accessor& accessor = gltf.accessors[accessorId];
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

    if (attribute == "INDEX"
        && ((accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            && (accessor.componentType != TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)))
    {
        throw std::runtime_error(
            "Loader supports only unsigned short indices, provided " + std::to_string(accessor.componentType));
    }

    if (attribute == "TEXCOORD_0" && accessor.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
    {
        throw std::runtime_error("Loader supports only float UV's, provided " + std::to_string(accessor.componentType));
    }

    // Buffer view
    std::size_t bufferViewId = static_cast<std::size_t>(accessor.bufferView);
    const tinygltf::BufferView& bufferView = gltf.bufferViews[bufferViewId];

    // Buffer
    std::size_t bufferId = static_cast<std::size_t>(bufferView.buffer);
    const tinygltf::Buffer& buffer = gltf.buffers[bufferId];

    return { { buffer.data.data() + accessor.byteOffset + bufferView.byteOffset,
               itemStride,
               itemCount,
               accessor.componentType } };
}

Core::SceneNodePtr
GltfLoader::TraverseFn(const tinygltf::Model& gltf, const tinygltf::Node& node, Core::SceneNodePtr parent)
{
    Core::SceneNodePtr result = Core::SceneNode::Create(node.name, parent);
    result->SetMesh(GltfLoader::MeshFn(gltf, node.mesh));
    result->SetTransform(GltfLoader::TransformFn(node));

    for (const auto childId : node.children)
    {
        const tinygltf::Node childNode = gltf.nodes.at(static_cast<std::size_t>(childId));
        Core::SceneNodePtr traversed = GltfLoader::TraverseFn(gltf, childNode, result);
        result->AddChildren(traversed);
    }

    return result;
}

Core::MeshPtr
GltfLoader::MeshFn(const tinygltf::Model& gltf, std::int32_t meshId)
{
    if (meshId == -1)
    {
        return nullptr;
    }

    auto indexBuffer = GltfLoader::GetBufferData(gltf, meshId, "INDEX");
    auto vertexBuffer = GltfLoader::GetBufferData(gltf, meshId, "POSITION");
    auto normalBuffer = GltfLoader::GetBufferData(gltf, meshId, "NORMAL");
    auto uvBuffer = GltfLoader::GetBufferData(gltf, meshId, "TEXCOORD_0");

    Core::Mesh result;

    if (!vertexBuffer.has_value())
    {
        return std::make_shared<Core::Mesh>(result);
    }

    // Vertices
    for (std::size_t i = 0; i < vertexBuffer.value().count; i++)
    {
        Core::Vertex vertex;

        // Position
        if (vertexBuffer.has_value())
        {
            const float* position
                = reinterpret_cast<const float*>(vertexBuffer.value().data + (i * vertexBuffer.value().stride));
            vertex.position = { position[0], position[1], position[2] };
        }

        // Normal
        if (normalBuffer.has_value())
        {
            const float* normal
                = reinterpret_cast<const float*>(normalBuffer.value().data + (i * normalBuffer.value().stride));
            vertex.normal = { normal[0], normal[1], normal[2] };
        }

        // UV
        if (uvBuffer.has_value())
        {
            const float* uv = reinterpret_cast<const float*>(uvBuffer.value().data + (i * uvBuffer.value().stride));
            vertex.uv = { uv[0], uv[1] };
        }

        result.vertices.push_back(vertex);
    }

    if (!indexBuffer.has_value())
    {
        return std::make_shared<Core::Mesh>(result);
    }

    // Indices
    for (std::size_t i = 0; i < indexBuffer.value().count; i++)
    {
        // Index
        if (indexBuffer.value().type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
        {
            const std::uint16_t index
                = *reinterpret_cast<const uint16_t*>(indexBuffer.value().data + (i * indexBuffer.value().stride));
            result.indices.push_back(index);
        }
        else if (indexBuffer.value().type == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
        {
            const std::uint32_t index
                = *reinterpret_cast<const uint32_t*>(indexBuffer.value().data + (i * indexBuffer.value().stride));
            result.indices.push_back(index);
        }
    }

    result.texture = GltfLoader::TextureFn(gltf, meshId);

    return std::make_shared<Core::Mesh>(result);
}

Core::TexturePtr
GltfLoader::TextureFn(const tinygltf::Model& gltf, std::int32_t meshId)
{
    if (static_cast<std::size_t>(meshId) >= gltf.meshes.size() || meshId < 0)
    {
        return nullptr;
    }

    const tinygltf::Mesh& mesh = gltf.meshes.at(static_cast<std::size_t>(meshId));

    if (mesh.primitives.empty())
    {
        return nullptr;
    }

    std::int32_t materialId = mesh.primitives.at(0).material;
    if (materialId < 0)
    {
        return nullptr;
    }

    if (static_cast<std::size_t>(materialId) >= gltf.materials.size())
    {
        return nullptr;
    }

    const tinygltf::Material& material = gltf.materials.at(static_cast<std::size_t>(materialId));

    std::int32_t textureId = material.pbrMetallicRoughness.baseColorTexture.index;
    if (textureId == -1)
    {
        return nullptr;
    }

    const tinygltf::Image image = gltf.images.at(static_cast<std::size_t>(textureId));

    Core::Texture result;
    result.size = { static_cast<std::uint32_t>(image.width), static_cast<std::uint32_t>(image.height) };
    result.pixels = image.image;

    return std::make_shared<Core::Texture>(result);
}

glm::mat4
GltfLoader::TransformFn(const tinygltf::Node& node)
{
    glm::mat4 result(1.0f);

    // If no matrix, check TRS
    if (node.matrix.empty())
    {
        if (!node.scale.empty())
        {
            result = glm::scale(
                result,
                glm::vec3 { static_cast<float>(node.scale.at(0)),
                            static_cast<float>(node.scale.at(1)),
                            static_cast<float>(node.scale.at(2)) });
        }

        if (!node.rotation.empty())
        {
            result *= glm::mat4_cast(glm::quat { static_cast<float>(node.rotation.at(0)),
                                                 static_cast<float>(node.rotation.at(1)),
                                                 static_cast<float>(node.rotation.at(2)),
                                                 static_cast<float>(node.rotation.at(3)) });
        }

        if (!node.translation.empty())
        {
            result = glm::translate(
                result,
                glm::vec3 { static_cast<float>(node.translation.at(0)),
                            static_cast<float>(node.translation.at(1)),
                            static_cast<float>(node.translation.at(2)) });
        }

        return result;
    }

    // If we have matrix use it
    for (std::int32_t i = 0; i < 4; ++i)
    {
        for (std::int32_t j = 0; j < 4; ++j)
        {
            result[i][j] = static_cast<float>(node.matrix.at(static_cast<std::size_t>(i * 4 + j)));
        }
    }

    return result;
}

} // namespace Lucid::Loaders
