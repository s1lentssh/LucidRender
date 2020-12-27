#include "Files.h"

#include <fstream>
#include <stb_image.h>
#include <tiny_obj_loader.h>

#include <Utils/Logger.hpp>

namespace Lucid
{

std::vector<char> Files::LoadFile(const std::filesystem::path& path)
{
	std::ifstream file(path, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Can't open file: " + path.string());
	}

	std::size_t fileSize = file.tellg();
	std::vector<char> pixels(fileSize);

	file.seekg(0);
	file.read(pixels.data(), fileSize);
	file.close();

	return pixels;
}

#undef LoadImage
Core::Texture Files::LoadImage(const std::filesystem::path& path)
{
	int width, height, channels;
	stbi_uc* pixels = stbi_load(path.string().c_str(), &width, &height, &channels, STBI_rgb_alpha);

	std::size_t size = static_cast<std::size_t>(width) * height * 4;
	std::vector<char> result(size);
	std::memcpy(result.data(), pixels, size);

	std::uint32_t mipLevels = static_cast<std::uint32_t>(std::floor(std::log2(std::max(width, height)))) + 1;

	return { 
		{ static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) }, 
		result,
		mipLevels
	};
}

Core::Mesh Files::LoadModel(const std::filesystem::path& path)
{
	Logger::Info("Loading model {}", path.string().c_str());

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

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.textureCoordinate = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.color = { 1.0f, 1.0f, 1.0f };

			mesh.vertices.push_back(vertex);
			mesh.indices.push_back(static_cast<std::uint32_t>(mesh.indices.size()));
		}
	}

	return mesh;
}

}
