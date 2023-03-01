#include "Files.h"

#include <fstream>

#include <stb_image.h>
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>
#include <tiny_obj_loader.h>

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

#undef LoadImage
Core::Texture
Files::LoadImage(const std::filesystem::path& path)
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
    std::vector<char> result(size);
    std::memcpy(result.data(), pixels, size);

    std::uint32_t mipLevels = static_cast<std::uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

    return { { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) }, result, mipLevels };
}

Core::Mesh
Files::LoadModel(const std::filesystem::path& path)
{
    LoggerInfo << "Loading model " << path.string().c_str();

    if (path.extension() == ".obj")
    {
        return LoadObj(path);
    }
    else if (path.extension() == ".gltf")
    {
        return LoadGltf(path);
    }

    throw std::runtime_error("Can't determine model format");
}

Core::Mesh
Files::LoadObj(const std::filesystem::path& path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &error, path.string().c_str()))
    {
        throw std::runtime_error("Cant load model, error: " + error);
    }

    Core::Mesh mesh;

    for (const auto& shape : shapes)
    {
        for (const auto& index : shape.mesh.indices)
        {
            Core::Vertex vertex;

            vertex.position = { attrib.vertices[static_cast<std::size_t>(3 * index.vertex_index + 0)],
                                attrib.vertices[static_cast<std::size_t>(3 * index.vertex_index + 1)],
                                attrib.vertices[static_cast<std::size_t>(3 * index.vertex_index + 2)] };

            vertex.normal = { attrib.normals[static_cast<std::size_t>(3 * index.normal_index + 0)],
                              attrib.normals[static_cast<std::size_t>(3 * index.normal_index + 1)],
                              attrib.normals[static_cast<std::size_t>(3 * index.normal_index + 2)] };

            vertex.textureCoordinate
                = { attrib.texcoords[static_cast<std::size_t>(2 * index.texcoord_index + 0)],
                    1.0f - attrib.texcoords[static_cast<std::size_t>(2 * index.texcoord_index + 1)] };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            mesh.vertices.push_back(vertex);
            mesh.indices.push_back(static_cast<std::uint32_t>(mesh.indices.size()));
        }
    }

    return mesh;
}

Core::Mesh
Files::LoadGltf(const std::filesystem::path& path)
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

    Core::Mesh mesh;

    for (const tinygltf::Mesh& gltfMesh : model.meshes)
    {
        std::string name = gltfMesh.name;

        auto getRawBuffer
            = [&model, &gltfMesh](const std::string& attribute) -> std::tuple<std::uint8_t*, std::size_t, std::size_t>
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
            tinygltf::Accessor accessor = model.accessors[accessorId];
            std::uint32_t componentType = static_cast<std::uint32_t>(accessor.componentType);
            std::uint32_t type = static_cast<std::uint32_t>(accessor.type);
            std::size_t componentSize = static_cast<std::size_t>(tinygltf::GetComponentSizeInBytes(componentType));
            std::size_t componentCount = static_cast<std::size_t>(tinygltf::GetNumComponentsInType(type));
            std::size_t itemStride = componentSize * componentCount;
            std::size_t itemCount = static_cast<std::size_t>(accessor.count);

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
            tinygltf::BufferView bufferView = model.bufferViews[bufferViewId];

            // Buffer
            std::size_t bufferId = static_cast<std::size_t>(bufferView.buffer);
            tinygltf::Buffer buffer = model.buffers[bufferId];

            return { static_cast<std::uint8_t*>(buffer.data.data() + accessor.byteOffset + bufferView.byteOffset),
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
            float* castedPosition = reinterpret_cast<float*>(positionBuffer + (i * positionStride));
            vertex.position = { castedPosition[0], castedPosition[1], castedPosition[2] };

            // Normal
            float* castedNormal = reinterpret_cast<float*>(normalBuffer + (i * normalStride));
            vertex.normal = { castedNormal[0], castedNormal[1], castedNormal[2] };

            // UV
            float* castedUv = reinterpret_cast<float*>(uvBuffer + (i * uvStride));
            vertex.textureCoordinate = { castedUv[0], castedUv[1] };

            // Index
            std::uint32_t* castedIndex = reinterpret_cast<std::uint32_t*>(indexBuffer + (i * indexStride));

            mesh.indices.push_back(static_cast<std::uint32_t>(*castedIndex));
            mesh.vertices.push_back(vertex);
        }
    }

    return mesh;
}

} // namespace Lucid
