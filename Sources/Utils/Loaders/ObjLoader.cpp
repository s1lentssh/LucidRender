#include "ObjLoader.h"

#include <tiny_obj_loader.h>

namespace Lucid::Loaders
{

Core::Scene::NodePtr
ObjLoader::Load(const std::filesystem::path& path)
{
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string error, warning;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, path.string().c_str()))
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

            vertex.uv = { attrib.texcoords[static_cast<std::size_t>(2 * index.texcoord_index + 0)],
                          1.0f - attrib.texcoords[static_cast<std::size_t>(2 * index.texcoord_index + 1)] };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            mesh.vertices.push_back(vertex);
            mesh.indices.push_back(static_cast<std::uint32_t>(mesh.indices.size()));
        }
    }

    Core::Scene::NodePtr node = Core::Scene::Node::Create("Root", nullptr);
    node->SetMesh(std::make_shared<Core::Mesh>(mesh));
    return node;
}

} // namespace Lucid::Loaders
